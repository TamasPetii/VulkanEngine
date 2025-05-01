#include "Renderer.h"
#include "Renderers/DeferredRenderer.h"
#include "Renderers/GeometryRenderer.h"
#include "Renderers/GuiRenderer.h"
#include "Renderers/BoundingVolumeRenderer.h"
#include "Engine/Vulkan/VulkanMutex.h"

Renderer::Renderer()
{
	Init();
}

Renderer::~Renderer()
{
	Destroy();
}

void Renderer::SetGuiRenderFunction(const std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)>& function)
{
	guiRenderFunction = function;
}

void Renderer::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	//Engine handles waiting for fence
	resourceManager->GetVulkanManager()->ResizeMarkedFrameBuffers(frameIndex);

	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto swapChain = Vk::VulkanContext::GetContext()->GetSwapChain();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto presentQueue = device->GetQueue(Vk::QueueType::PRESENTATION);

	auto inFlightFence = resourceManager->GetVulkanManager()->GetFrameDependentFence("InFlight", frameIndex);
	auto imageAvailableSemaphore = resourceManager->GetVulkanManager()->GetFrameDependentSemaphore("ImageAvailable", frameIndex);
	auto renderFinishedSemaphore = resourceManager->GetVulkanManager()->GetFrameDependentSemaphore("RenderFinished", frameIndex);
	
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->Value(), swapChain->Value(), UINT64_MAX, imageAvailableSemaphore->Value(), VK_NULL_HANDLE, &imageIndex);

	VkCommandBuffer commandBuffer = commandBuffers[frameIndex];

	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!");

	GeometryRenderer::Render(commandBuffer, registry, resourceManager, frameIndex);
	DeferredRenderer::Render(commandBuffer, registry, resourceManager, frameIndex);
	BoundingVolumeRenderer::Render(commandBuffer, registry, resourceManager, frameIndex);
	GuiRenderer::Render(commandBuffer, registry, resourceManager, frameIndex, imageIndex, guiRenderFunction);

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

	std::unique_lock<std::mutex> queueLock(VulkanMutex::graphicsQueueSubmitMutex);
	VK_CHECK_MESSAGE(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, inFlightFence->Value()), "Failed to submit draw command buffer!");
	queueLock.unlock();

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
}

void Renderer::Init()
{
	InitCommandPool();
	InitCommandBuffer();
}

void Renderer::Destroy()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDeviceWaitIdle(device->Value());

	for (uint32_t i = 0; i < GlobalConfig::FrameConfig::maxFramesInFlights; i++)
	{
		vkDestroyCommandPool(device->Value(), commandPools[i], nullptr);
	}
}

void Renderer::InitCommandPool()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto& queueFamilyIndices = Vk::VulkanContext::GetContext()->GetPhysicalDevice()->GetQueueFamilyIndices();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	for(uint32_t i = 0; i < GlobalConfig::FrameConfig::maxFramesInFlights; ++i)
		VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &commandPools[i]), "Failed to create command pool!");
}

void Renderer::InitCommandBuffer()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	for (uint32_t i = 0; i < GlobalConfig::FrameConfig::maxFramesInFlights; ++i)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPools[i];
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &commandBuffers[i]), "Failed to allocate command buffers!");
	}
}
void Renderer::RecreateSwapChain()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDeviceWaitIdle(device->Value());
	Vk::VulkanContext::GetContext()->GetSwapChain()->ReCreate();
}