#include "Renderer.h"

Renderer::Renderer()
{
	Init();
}

Renderer::~Renderer()
{
	Destroy();
}

void Renderer::SetGuiRenderFunction(const std::function<void(VkCommandBuffer commandBuffer)>& function)
{
	guiRenderFunction = function;
}

void Renderer::Render()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto swapChain = Vk::VulkanContext::GetContext()->GetSwapChain();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto presentQueue = device->GetQueue(Vk::QueueType::PRESENTATION);
	auto renderContext = RenderContext::GetContext();

	uint32_t framesInFlightIndex = RenderContext::GetContext()->GetFramesInFlightIndex();

	auto imageAvailableSemaphore = imageAvailableSemaphores[framesInFlightIndex];
	auto renderFinishedSemaphore = renderFinishedSemaphores[framesInFlightIndex];
	auto inFlightFence = inFlightFences[framesInFlightIndex];

	vkWaitForFences(device->Value(), 1, &inFlightFence->Value(), VK_TRUE, UINT64_MAX);

	//Resize framebuffers, only if it maches the current frame index for proper syncronization
	renderContext->ResizeMarkedFrameBuffers(framesInFlightIndex);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->Value(), swapChain->Value(), UINT64_MAX, imageAvailableSemaphore->Value(), VK_NULL_HANDLE, &imageIndex);

	vkResetFences(device->Value(), 1, &inFlightFence->Value());

	VkCommandBuffer commandBuffer = commandBuffers[framesInFlightIndex];

	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!");

	GeometryRenderer::Render(commandBuffer, shapes["cube"]->GetVertexBuffer(), shapes["cube"]->GetIndexBuffer(), shapes["cube"]->GetIndexCount());
	DeferredRenderer::Render(commandBuffer);

	auto frameBuffer = renderContext->GetFrameBuffer("main", framesInFlightIndex);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("main")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT);

	VkRenderingAttachmentInfo guiColorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(swapChain->GetImageViews()[imageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, nullptr);
	std::vector<VkRenderingAttachmentInfo> renderTargetAttachments2 = { guiColorAttachment };
	VkRenderingInfo guiRenderingInfo = Vk::DynamicRendering::BuildRenderingInfo(swapChain->GetExtent(), renderTargetAttachments2, nullptr);

	vkCmdBeginRendering(commandBuffer, &guiRenderingInfo);

	if (guiRenderFunction)
		guiRenderFunction(commandBuffer);

	vkCmdEndRendering(commandBuffer);	

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE);

	VK_CHECK_MESSAGE(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer!");

	VkSemaphoreSubmitInfo waitSemaphoreSubmitInfo{};
	waitSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waitSemaphoreSubmitInfo.semaphore = imageAvailableSemaphore->Value();
	waitSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	waitSemaphoreSubmitInfo.deviceIndex = 0;
	waitSemaphoreSubmitInfo.value = 1;
	waitSemaphoreSubmitInfo.pNext = nullptr;

	VkSemaphoreSubmitInfo signalSemaphoreSubmitInfo{};
	signalSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signalSemaphoreSubmitInfo.semaphore = renderFinishedSemaphore->Value();
	signalSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	signalSemaphoreSubmitInfo.deviceIndex = 0;
	signalSemaphoreSubmitInfo.value = 1;
	signalSemaphoreSubmitInfo.pNext = nullptr;

	VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
	commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmitInfo.commandBuffer = commandBuffer;
	commandBufferSubmitInfo.deviceMask = 0;
	commandBufferSubmitInfo.pNext = nullptr;

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreInfoCount = 1;
	submitInfo.pWaitSemaphoreInfos = &waitSemaphoreSubmitInfo;
	submitInfo.signalSemaphoreInfoCount = 1;
	submitInfo.pSignalSemaphoreInfos = &signalSemaphoreSubmitInfo;
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

	VK_CHECK_MESSAGE(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, inFlightFence->Value()), "Failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore->Value();
	VkSwapchainKHR swapChains[] = { swapChain->Value()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	RenderContext::GetContext()->UpdateFramesInFlightIndex();
}

void Renderer::Init()
{
	RenderContext::GetContext()->Init();

	commandPools.resize(FRAMES_IN_FLIGHT);
	commandBuffers.resize(FRAMES_IN_FLIGHT);
	imageAvailableSemaphores.resize(FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
	inFlightFences.resize(FRAMES_IN_FLIGHT);

	shapes["sphere"] = std::make_shared<Sphere>();
	shapes["quad"] = std::make_shared<Quad>();
	shapes["capsule"] = std::make_shared<Capsule>();
	shapes["cube"] = std::make_shared<Cube>();
	shapes["cylinder"] = std::make_shared<Cylinder>();
	shapes["cone"] = std::make_shared<Cone>();
	shapes["pyramid"] = std::make_shared<Pyramid>();
	shapes["torus"] = std::make_shared<Torus>();

	InitCommandPool();
	InitCommandBuffer();
	InitSyncronization();
	InitBuffers();

}

void Renderer::Destroy()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDeviceWaitIdle(device->Value());

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyCommandPool(device->Value(), commandPools[i], nullptr);
	}

	vkDestroyCommandPool(device->Value(), immediatePool, nullptr);
	
	commandPools.clear();
	commandBuffers.clear();
	imageAvailableSemaphores.clear();
	renderFinishedSemaphores.clear();
	inFlightFences.clear();

	RenderContext::DestroyContext();;
}


void Renderer::InitCommandPool()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto& queueFamilyIndices = Vk::VulkanContext::GetContext()->GetPhysicalDevice()->GetQueueFamilyIndices();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	for(uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
		VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &commandPools[i]), "Failed to create command pool!");

	VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &immediatePool), "Failed to create command pool!");
}

void Renderer::InitCommandBuffer()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPools[i];
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &commandBuffers[i]), "Failed to allocate command buffers!");
	}
}

void Renderer::InitSyncronization()
{
	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		inFlightFences[i] = std::make_shared<Vk::Fence>(true);
		imageAvailableSemaphores[i] = std::make_shared<Vk::Semaphore>();
		renderFinishedSemaphores[i] = std::make_shared<Vk::Semaphore>();
	}
}

void Renderer::InitBuffers()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	{
		const std::vector<Vertex> vertices = {
			Vertex(glm::vec3(-1.f, -1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f}),
			Vertex(glm::vec3(1.f, -1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f}),
			Vertex(glm::vec3(1.f, 1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f}),
			Vertex(glm::vec3(-1.f, 1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f})
		};

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		Vk::BufferConfig stagingConfig;
		stagingConfig.size = bufferSize;
		stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
		stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Vk::Buffer stagingBuffer{ stagingConfig };

		void* mappedBuffer = stagingBuffer.MapMemory();
		memcpy(mappedBuffer, vertices.data(), (size_t)bufferSize);
		stagingBuffer.UnmapMemory();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		vertexBuffer = std::make_shared<Vk::Buffer>(config);

		Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
			[&](VkCommandBuffer commandBuffer) -> void {
				Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), vertexBuffer->Value(), bufferSize);
			}
		);
	}

	{
		const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		Vk::BufferConfig stagingConfig;
		stagingConfig.size = bufferSize;
		stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
		stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Vk::Buffer stagingBuffer{ stagingConfig };

		void* mappedBuffer = stagingBuffer.MapMemory();
		memcpy(mappedBuffer, indices.data(), (size_t)bufferSize);
		stagingBuffer.UnmapMemory();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_INDEX_BUFFER_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		indexBuffer = std::make_shared<Vk::Buffer>(config);

		VkCommandBuffer commandBuffer = Vk::CommandBuffer::BeginSingleTimeCommandBuffer(immediatePool);
		Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), indexBuffer->Value(), bufferSize);
		Vk::CommandBuffer::EndSingleTimeCommandBuffer(commandBuffer, immediatePool, device->GetQueue(Vk::QueueType::GRAPHICS));
	}
}

void Renderer::RecreateSwapChain()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDeviceWaitIdle(device->Value());
	Vk::VulkanContext::GetContext()->GetSwapChain()->ReCreate();
}
