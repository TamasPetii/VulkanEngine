#include "Renderer.h"

Renderer::Renderer()
{
	Init();
}

Renderer::~Renderer()
{
	Destroy();
}

void Renderer::Render()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto swapChain = Vk::VulkanContext::GetContext()->GetSwapChain();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto presentQueue = device->GetQueue(Vk::QueueType::PRESENTATION);

	VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[framesInFlightIndex];
	VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[framesInFlightIndex];
	VkFence inFlightFence = inFlightFences[framesInFlightIndex];

	vkWaitForFences(device->Value(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->Value(), swapChain->Value(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetFences(device->Value(), 1, &inFlightFence);

	auto frameBuffer = frameBuffers[framesInFlightIndex];
	VkCommandBuffer commandBuffer = commandBuffers[framesInFlightIndex];

	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!");

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {1.0f, 1.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderingAttachmentInfo colorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("colorImage")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &clearValues[0]);
	VkRenderingAttachmentInfo depthAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("depth")->GetImageView(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, &clearValues[1]);
	VkRenderingInfo renderingInfo = Vk::DynamicRendering::BuildRenderingInfo(frameBuffer->GetSize(), &colorAttachment, &depthAttachment);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("colorImage")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("depth")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

	vkCmdBeginRendering(commandBuffer, &renderingInfo);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)frameBuffer->GetSize().width;
	viewport.height = (float)frameBuffer->GetSize().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = frameBuffer->GetSize();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRendering(commandBuffer);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("colorImage")->Value(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT);

	Vk::Image::CopyImageToImageDynamic(commandBuffer, frameBuffer->GetImage("colorImage")->Value(), frameBuffer->GetSize(), swapChain->GetImages()[imageIndex], swapChain->GetExtent());

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE);

	VK_CHECK_MESSAGE(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer!");

	VkSemaphoreSubmitInfo waitSemaphoreSubmitInfo{};
	waitSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waitSemaphoreSubmitInfo.semaphore = imageAvailableSemaphore;
	waitSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	waitSemaphoreSubmitInfo.deviceIndex = 0;
	waitSemaphoreSubmitInfo.value = 1;
	waitSemaphoreSubmitInfo.pNext = nullptr;

	VkSemaphoreSubmitInfo signalSemaphoreSubmitInfo{};
	signalSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signalSemaphoreSubmitInfo.semaphore = renderFinishedSemaphore;
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

	VK_CHECK_MESSAGE(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, inFlightFence), "Failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
	VkSwapchainKHR swapChains[] = { swapChain->Value()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	framesInFlightIndex = (framesInFlightIndex + 1) % FRAMES_IN_FLIGHT;
}

void Renderer::Init()
{
	commandPools.resize(FRAMES_IN_FLIGHT);
	commandBuffers.resize(FRAMES_IN_FLIGHT);
	imageAvailableSemaphores.resize(FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
	inFlightFences.resize(FRAMES_IN_FLIGHT);
	frameBuffers.resize(FRAMES_IN_FLIGHT);

	LoadShaders();
	InitRenderPass();
	InitFrameBuffers();
	InitGraphicsPipeline();
	InitCommandPool();
	InitCommandBuffer();
	InitSyncronization();
}

void Renderer::Destroy()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	vkDeviceWaitIdle(device->Value());

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyCommandPool(device->Value(), commandPools[i], nullptr);
		vkDestroySemaphore(device->Value(), imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device->Value(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device->Value(), inFlightFences[i], nullptr);
	}

	commandPools.clear();
	commandBuffers.clear();
	imageAvailableSemaphores.clear();
	renderFinishedSemaphores.clear();
	inFlightFences.clear();
}

void Renderer::LoadShaders()
{
	shaderModuls["BasicVert"] = std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaderModuls["BasicFrag"] = std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
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
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		VK_CHECK_MESSAGE(vkCreateSemaphore(device->Value(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]), "Failed to create image available semaphore!");
		VK_CHECK_MESSAGE(vkCreateSemaphore(device->Value(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]), "Failed to create render finished semaphore!");
		VK_CHECK_MESSAGE(vkCreateFence(device->Value(), &fenceInfo, nullptr, &inFlightFences[i]), "Failed to create in-flight fence!");
	}
}

void Renderer::InitRenderPass()
{
	Vk::RenderPassBuilder renderPassBuilder;
	renderPassBuilder.RegisterSubpass("mainSubpass", 0);
	renderPassBuilder.AttachImageDescription("colorImage", 0, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	renderPassBuilder.AttachImageReferenceToSubpass("mainSubpass", "colorImage", VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachDepthDescription(1, VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachDepthReferenceToSubpass("mainSubpass", VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachSubpassDependency("mainSubpass", Vk::DependencyStage::SRC, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_NONE);
	renderPassBuilder.AttachSubpassDependency("mainSubpass", Vk::DependencyStage::DST, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
	renderPass = renderPassBuilder.BuildRenderPass();
}

void Renderer::InitFrameBuffers()
{
	auto swapChainExtent = Vk::VulkanContext::GetContext()->GetSwapChain()->GetExtent();

	Vk::ImageSpecification colorImageSpec;
	colorImageSpec.type = VK_IMAGE_TYPE_2D;
	colorImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	colorImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	colorImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	colorImageSpec.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	colorImageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	colorImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::ImageSpecification depthImageSpec;
	depthImageSpec.type = VK_IMAGE_TYPE_2D;
	depthImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageSpec.format = VK_FORMAT_D32_SFLOAT;
	depthImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageSpec.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthImageSpec.aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::FrameBufferBuilder frameBufferBuilder;
	frameBufferBuilder.SetInitialSize(swapChainExtent.width, swapChainExtent.height);
	frameBufferBuilder.AttachImageSpec("colorImage", 0, colorImageSpec);
	frameBufferBuilder.AttachDepthSpec(1, depthImageSpec);

	/*
	for(uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
		frameBuffers[i] = frameBufferBuilder.BuildFrameBuffer(renderPass);
	*/

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
		frameBuffers[i] = frameBufferBuilder.BuildDynamicFrameBuffer();
}

void Renderer::InitGraphicsPipeline()
{
	std::vector<VkVertexInputBindingDescription> vertexBindingDescirption = { Vertex::GetBindingDescription() };
	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription = Vertex::GetAttributeDescriptions();

	Vk::GraphicsPipelineBuilder pipelineBuilder;
	graphicsPipeline = pipelineBuilder
					.AddShaderStage(shaderModuls["BasicVert"])
					.AddShaderStage(shaderModuls["BasicFrag"])
					.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
					.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR)
					.SetVertexInput({}, {})
					.SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
					.SetRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE)
					.SetMultisampling(VK_SAMPLE_COUNT_1_BIT)
					.SetDepthStencil(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS)
					.SetColorBlend(VK_FALSE)
					.AddColorBlendAttachment(VK_FALSE)
					.SetColorAttachmentFormats(VK_FORMAT_R16G16B16A16_SFLOAT, 0)
					.SetDepthAttachmentFormat(VK_FORMAT_D32_SFLOAT)
					.BuildDynamic();
}
