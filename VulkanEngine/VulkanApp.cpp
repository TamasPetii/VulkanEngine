#include "VulkanApp.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#ifdef NDEBUG
bool useValidationLayers = false;
#else
bool useValidationLayers = true;
#endif

#include <dwmapi.h>

void VulkanApp::Run()
{
	Init();
	MainLoop();
	Cleanup();
}

void VulkanApp::Init()
{
	InitWindow();
	InitVulkan();
	InitSwapChain();
	InitCommands();
	InitSyncStructures();
	InitDescriptors();
	InitPipelines();
	InitImgui();
	InitDefaultData();
}

void VulkanApp::MainLoop()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	float prevTime = 0.0f;
	
	float fpsTimer = 0.0f;
	int frameCount = 0;

	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		deltaTime = time - prevTime;
		prevTime = time;

		fpsTimer += deltaTime;
		frameCount++;

		if (fpsTimer > 1.f)
		{
			float fps = frameCount / fpsTimer;
			frameCount = 0;
			fpsTimer = 0;

			std::string title = "Vulkan Engine | " + std::to_string(static_cast<int>(fps)) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		glfwPollEvents();
		Draw();
	}
}

void VulkanApp::Cleanup()
{
	vkDeviceWaitIdle(logicalDevice);

	DestroyBuffer(rectangle.vertexBuffer);
	DestroyBuffer(rectangle.indexBuffer);

	vkDestroyPipelineLayout(logicalDevice, meshPipelineLayout, nullptr);
	vkDestroyPipeline(logicalDevice, meshPipeline, nullptr);

	vkDestroyPipelineLayout(logicalDevice, trianglePipelineLayout, nullptr);
	vkDestroyPipeline(logicalDevice, trianglePipeline, nullptr);

	ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(logicalDevice, imguiPool, nullptr);

	vkDestroyFence(logicalDevice, immediateFance, nullptr);
	vkDestroyCommandPool(logicalDevice, immediateCommandPool, nullptr);

	vkDestroyPipelineLayout(logicalDevice, gradientPipelineLayout, nullptr);
	vkDestroyPipeline(logicalDevice, gradientPipeline, nullptr);

	globalDescriptorAllocator.DestroyPool(logicalDevice);
	vkDestroyDescriptorSetLayout(logicalDevice, drawImageDescriptorLayout, nullptr);

	vkDestroyImageView(logicalDevice, drawImage.imageView, nullptr);
	vmaDestroyImage(vmaAllocator, drawImage.image, drawImage.allocation);
	vmaDestroyAllocator(vmaAllocator);

	for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		vkDestroyCommandPool(logicalDevice, frameData[i].commandPool, nullptr);
		vkDestroyFence(logicalDevice, frameData[i].renderFence, nullptr);
		vkDestroySemaphore(logicalDevice, frameData[i].renderSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, frameData[i].swapchainSemaphore, nullptr);
	}

	DestroySwapchain();
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);

	if(useValidationLayers)
		vkb::destroy_debug_utils_messenger(instance, debugMessenger);

	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
}

void VulkanApp::Draw()
{
	VK_CHECK(vkWaitForFences(logicalDevice, 1, &GetCurrentFrameData().renderFence, true, UINT64_MAX), "Wait for fences error");
	VK_CHECK(vkResetFences(logicalDevice, 1, &GetCurrentFrameData().renderFence), "Reset fences error");

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, GetCurrentFrameData().swapchainSemaphore, nullptr, &swapchainImageIndex), "Couldn't acquire swapchain image!");

	VkCommandBuffer commandBuffer = GetCurrentFrameData().commandBuffer;
	VK_CHECK(vkResetCommandBuffer(commandBuffer, 0), "Couldn't reset command buffer");

	VkCommandBufferBeginInfo commandBufferBeginInfo = Vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "Couldn't begin command buffer!");

	//Clear color values in image, and transferimage layout properly to being able to copy its data
	Vkimages::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	
	DrawBackground(commandBuffer);

	Vkimages::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	DrawGeometry(commandBuffer);

	Vkimages::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	//Copy image to swapchain image, and transfer image layout properly
	Vkimages::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Vkimages::CopyImageToImage(commandBuffer, drawImage.image, swapchainImages[swapchainImageIndex], drawExtent, swapchainExtent);
	Vkimages::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	RenderGui();
	DrawImGui(commandBuffer, swapchainImageViews[swapchainImageIndex]);
	Vkimages::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	VK_CHECK(vkEndCommandBuffer(commandBuffer), "Error occured while ending command buffer!");

	VkCommandBufferSubmitInfo commandSubmitInfo = Vkinit::CommandBufferSubmitInfo(commandBuffer);
	VkSemaphoreSubmitInfo waitSubmitInfo = Vkinit::SemaphoreSubmintInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrameData().swapchainSemaphore);
	VkSemaphoreSubmitInfo signalSubmitInfo = Vkinit::SemaphoreSubmintInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrameData().renderSemaphore);
	VkSubmitInfo2 submitInfo = Vkinit::SubmitInfo(&commandSubmitInfo, &signalSubmitInfo, &waitSubmitInfo);

	VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, GetCurrentFrameData().renderFence), "Couldn't submit command to graphics queue!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &swapchainImageIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &GetCurrentFrameData().renderSemaphore;
	presentInfo.pNext = nullptr;

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo), "Couldn't start presentation operations!");

	frame++;
}

void VulkanApp::DrawBackground(VkCommandBuffer commandBuffer)
{
	drawExtent.width = drawImage.imageExtent.width;
	drawExtent.height = drawImage.imageExtent.height;

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipelineLayout, 0, 1, &drawImageDescriptor, 0, nullptr);

	ComputePushConstants pushConstants;
	pushConstants.data1 = glm::vec4(1, 0, 0, 1);
	pushConstants.data2 = glm::vec4(0, 0, 1, 1);

	vkCmdPushConstants(commandBuffer, gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &pushConstants);
	vkCmdDispatch(commandBuffer, std::ceil(drawExtent.width / 16.0), std::ceil(drawExtent.height / 16.0), 1);
}

void VulkanApp::DrawGeometry(VkCommandBuffer commandBuffer)
{
	//begin a render pass  connected to our draw image
	VkRenderingAttachmentInfo colorAttachment = Vkinit::AttachmentInfo(drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = Vkinit::RenderingInfo(drawExtent, &colorAttachment, nullptr);

	vkCmdBeginRendering(commandBuffer, &renderInfo);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);

	//set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = drawExtent.width;
	viewport.height = drawExtent.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = drawExtent.width;
	scissor.extent.height = drawExtent.height;

	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	//-----------------------------

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

	GPUDrawPushConstants push_constants;
	push_constants.worldMatrix = glm::mat4{ 1.f };
	push_constants.vertexBuffer = rectangle.vertexBufferAddress;

	vkCmdPushConstants(commandBuffer, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(commandBuffer, rectangle.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

	vkCmdEndRendering(commandBuffer);
}

void VulkanApp::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);

	HWND hwnd = glfwGetWin32Window(window);
	COLORREF color = RGB(0, 0, 0);
	const DWORD DWMWA_CAPTION_COLOR = 35;
	HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

	int width, height, channels;
	unsigned char* pixels = stbi_load("DendriteLogo.png", &width, &height, &channels, 0);

	if (pixels)
	{
		GLFWimage images[1];
		images[0].width = width;
		images[0].height = height;
		images[0].pixels = pixels;
		glfwSetWindowIcon(window, 1, images);
	}

	stbi_image_free(pixels);
}

void VulkanApp::InitVulkan()
{
	vkb::InstanceBuilder builder;

	auto buildedResource = builder.set_app_name("Vulkan Engine")
								  .request_validation_layers(useValidationLayers)
								  .use_default_debug_messenger()
								  .require_api_version(1, 3, 0)
								  .build();

	vkb::Instance vkbInstance = buildedResource.value();
	instance = vkbInstance;

	if(useValidationLayers)
		debugMessenger = vkbInstance.debug_messenger;

	VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface), "Failed to create window surface");

	VkPhysicalDeviceVulkan13Features features13{};
	features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features13.dynamicRendering = true;
	features13.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{};
	features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

	vkb::PhysicalDeviceSelector selector{ vkbInstance };

	vkb::PhysicalDevice vkbPhysicalDevice = selector.set_minimum_version(1, 3)
													.set_required_features_12(features12)
													.set_required_features_13(features13)
													.set_surface(surface)
													.select()
													.value();

	vkb::DeviceBuilder deviceBuilder{ vkbPhysicalDevice };
	vkb::Device vkbLogicalDevice = deviceBuilder.build().value();

	physicalDevice = vkbPhysicalDevice.physical_device;
	logicalDevice = vkbLogicalDevice.device;

	graphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueIndex = vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();

	VmaAllocatorCreateInfo vamAllocatorInfo{};
	vamAllocatorInfo.device = logicalDevice;
	vamAllocatorInfo.instance = instance;
	vamAllocatorInfo.physicalDevice = physicalDevice;
	vamAllocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&vamAllocatorInfo, &vmaAllocator);
}

void VulkanApp::InitSwapChain()
{
	CreateSwapchain();

	VkExtent3D drawImageExtent;
	drawImageExtent.width = WIDTH;
	drawImageExtent.height = HEIGHT;
	drawImageExtent.depth = 1;

	drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	drawImage.imageExtent = drawImageExtent;
	
	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo imageCreateInfo = Vkinit::ImageCreateInfo(drawImage.imageFormat, drawImageUsages, drawImage.imageExtent);

	VmaAllocationCreateInfo imageAllocationInfo{};
	imageAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	imageAllocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_CHECK(vmaCreateImage(vmaAllocator, &imageCreateInfo, &imageAllocationInfo, &drawImage.image, &drawImage.allocation, nullptr), "Couldn't allocate image");

	VkImageViewCreateInfo imageViewCreateInfo = Vkinit::ImageViewCreateInfo(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &drawImage.imageView), "Couldn't create image view");


}

void VulkanApp::InitCommands()
{
	
	VkCommandPoolCreateInfo commandPoolInfo = Vkinit::CommandPoolCreateInfo(graphicsQueueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		VK_CHECK(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &frameData[i].commandPool), "Couldn't create command pool!");

		VkCommandBufferAllocateInfo commandBufferInfo = Vkinit::CommandBufferAllocateInfo(frameData[i].commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(logicalDevice, &commandBufferInfo, &frameData[i].commandBuffer), "Couldn't allocate command buffer");
	}

	VK_CHECK(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &immediateCommandPool), "Couldn't create command pool!");

	VkCommandBufferAllocateInfo commandBufferInfo = Vkinit::CommandBufferAllocateInfo(immediateCommandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(logicalDevice, &commandBufferInfo, &immediateCommandBuffer), "Couldn't allocate command buffer");
}

void VulkanApp::InitSyncStructures()
{
	VkFenceCreateInfo fenceInfo = Vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreInfo = Vkinit::SemaphoreCreateInfo();

	for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		VK_CHECK(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &frameData[i].renderFence), "Couldn't create render fence");
		VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &frameData[i].renderSemaphore), "Couldn't create render semaphore");
		VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &frameData[i].swapchainSemaphore), "Couldn't create swapchain semaphore");
	}

	VK_CHECK(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &immediateFance), "Couldn't create immediate fence");
}

void VulkanApp::InitDescriptors()
{
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
	};

	globalDescriptorAllocator.InitPool(logicalDevice, 10, sizes);

	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		drawImageDescriptorLayout = builder.Build(logicalDevice, VK_SHADER_STAGE_COMPUTE_BIT);
	}

	drawImageDescriptor = globalDescriptorAllocator.Allocate(logicalDevice, drawImageDescriptorLayout);

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageInfo.imageView = drawImage.imageView;

	VkWriteDescriptorSet writeSet{};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.pNext = nullptr;
	writeSet.dstBinding = 0;
	writeSet.descriptorCount = 1;
	writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeSet.dstSet = drawImageDescriptor;
	writeSet.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(logicalDevice, 1, &writeSet, 0, nullptr);

}

void VulkanApp::InitPipelines()
{
	InitTrianglePipeline();
	InitBackgroundPipelines();
	InitMeshPipeline();
}

void VulkanApp::InitBackgroundPipelines()
{
	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	layoutCreateInfo.pSetLayouts = &drawImageDescriptorLayout;
	layoutCreateInfo.setLayoutCount = 1;

	VkPushConstantRange pushConstant{};
	pushConstant.offset = 0;
	pushConstant.size = sizeof(ComputePushConstants);
	pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	
	layoutCreateInfo.pushConstantRangeCount = 1;
	layoutCreateInfo.pPushConstantRanges = &pushConstant;

	VK_CHECK(vkCreatePipelineLayout(logicalDevice, &layoutCreateInfo, nullptr, &gradientPipelineLayout), "Couldn't create pipeline layout!");

	VkShaderModule computeDrawShader;
	VK_CHECK(!Vkpipelines::LoadShaderModule("Shaders/GradientPush-cp.spv", logicalDevice, &computeDrawShader), "Couldn't load shader modul!");

	VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext = nullptr;
	stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = computeDrawShader;
	stageinfo.pName = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = gradientPipelineLayout;
	computePipelineCreateInfo.stage = stageinfo;
	
	VK_CHECK(vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradientPipeline), "Couldn't create pipeline!");

	vkDestroyShaderModule(logicalDevice, computeDrawShader, nullptr);
}

void VulkanApp::InitTrianglePipeline()
{
	VkShaderModule triangleVertShader;
	VK_CHECK(!Vkpipelines::LoadShaderModule("Shaders/Triangle-vs.spv", logicalDevice, &triangleVertShader), "Couldn't load shader modul!");

	VkShaderModule triangleFragShader;
	VK_CHECK(!Vkpipelines::LoadShaderModule("Shaders/Triangle-fs.spv", logicalDevice, &triangleFragShader), "Couldn't load shader modul!");

	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	VK_CHECK(vkCreatePipelineLayout(logicalDevice, &layoutCreateInfo, nullptr, &trianglePipelineLayout), "Couldn't create pipeline layout!");

	PipelineBuilder pipelineBuilder;
	pipelineBuilder.pipelineLayout = trianglePipelineLayout;
	pipelineBuilder.SetShaders(triangleVertShader, triangleFragShader);
	pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
	pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	pipelineBuilder.SetMultisamplingNone();
	pipelineBuilder.DisableBlending();
	pipelineBuilder.DisableDepthTest();
	pipelineBuilder.SetColorAttachmentFormat(drawImage.imageFormat);
	pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

	trianglePipeline = pipelineBuilder.BuildPipeline(logicalDevice);

	vkDestroyShaderModule(logicalDevice, triangleFragShader, nullptr);
	vkDestroyShaderModule(logicalDevice, triangleVertShader, nullptr);
}

void VulkanApp::InitMeshPipeline()
{
	VkShaderModule meshVertShader;
	VK_CHECK(!Vkpipelines::LoadShaderModule("Shaders/Mesh-vs.spv", logicalDevice, &meshVertShader), "Couldn't load shader modul!");

	VkShaderModule meshFragShader;
	VK_CHECK(!Vkpipelines::LoadShaderModule("Shaders/Mesh-fs.spv", logicalDevice, &meshFragShader), "Couldn't load shader modul!");

	VkPushConstantRange bufferRange{};
	bufferRange.offset = 0;
	bufferRange.size = sizeof(GPUDrawPushConstants);
	bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	layoutCreateInfo.pushConstantRangeCount = 1;
	layoutCreateInfo.pPushConstantRanges = &bufferRange;

	VK_CHECK(vkCreatePipelineLayout(logicalDevice, &layoutCreateInfo, nullptr, &meshPipelineLayout), "Couldn't create pipeline layout!");

	PipelineBuilder pipelineBuilder;
	pipelineBuilder.pipelineLayout = meshPipelineLayout;
	pipelineBuilder.SetShaders(meshVertShader, meshFragShader);
	pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
	pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	pipelineBuilder.SetMultisamplingNone();
	pipelineBuilder.DisableBlending();
	pipelineBuilder.DisableDepthTest();
	pipelineBuilder.SetColorAttachmentFormat(drawImage.imageFormat);
	pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

	meshPipeline = pipelineBuilder.BuildPipeline(logicalDevice);

	vkDestroyShaderModule(logicalDevice, meshVertShader, nullptr);
	vkDestroyShaderModule(logicalDevice, meshFragShader, nullptr);
}

void VulkanApp::CreateSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{physicalDevice, logicalDevice, surface};
	swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	VkSurfaceFormatKHR surfaceFormat;
	surfaceFormat.format = swapchainImageFormat;
	surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	vkb::Swapchain vkbSwapchain = swapchainBuilder.set_desired_format(surfaceFormat)
												  .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
												  .set_desired_extent(WIDTH, HEIGHT)
												  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
												  .build()
												  .value();
	swapchainExtent = vkbSwapchain.extent;
	swapchain = vkbSwapchain.swapchain;
	swapchainImages = vkbSwapchain.get_images().value();
	swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanApp::DestroySwapchain()
{
	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);

	for (int i = 0; i < swapchainImageViews.size(); i++) {

		vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
	}
}

void VulkanApp::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VK_CHECK(vkResetFences(logicalDevice, 1, &immediateFance), "Couldn't create immediate fence!");
	VK_CHECK(vkResetCommandBuffer(immediateCommandBuffer, 0), "Couldn't reset immediate command buffer");

	VkCommandBuffer commandBuffer = immediateCommandBuffer;

	VkCommandBufferBeginInfo commandBufferBeginInfo = Vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "Couldn't begin immediate command buffer");

	function(commandBuffer);

	VK_CHECK(vkEndCommandBuffer(commandBuffer), "Couldn't end immediate command buffer");

	VkCommandBufferSubmitInfo commandBufferSubmitInfo = Vkinit::CommandBufferSubmitInfo(commandBuffer);
	VkSubmitInfo2 submitInfo = Vkinit::SubmitInfo(&commandBufferSubmitInfo, nullptr, nullptr);

	VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, immediateFance), "Couldn't submit immediate buffer into queue");
	VK_CHECK(vkWaitForFences(logicalDevice, 1, &immediateFance, true, UINT64_MAX), "Error occured while waiting for immediateFance to be signaled!");
}

void VulkanApp::InitImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable multi-viewport support

	ImGui_ImplGlfw_InitForVulkan(window, true);

	VkDescriptorPoolSize poolSizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
	poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
	poolInfo.pPoolSizes = poolSizes;

	VK_CHECK(vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &imguiPool), "Couldn't create imgui descriptor pool");

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = logicalDevice;
	init_info.QueueFamily = graphicsQueueIndex;
	init_info.Queue = graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = static_cast<uint32_t>(3);
	init_info.ImageCount = static_cast<uint32_t>(3);
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.UseDynamicRendering = true;
	init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;

	ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplVulkan_CreateFontsTexture();
}

void VulkanApp::DrawImGui(VkCommandBuffer commandBuffer, VkImageView imageView)
{
	VkRenderingAttachmentInfo attachmentInfo = Vkinit::AttachmentInfo(imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderingInfo = Vkinit::RenderingInfo(swapchainExtent, &attachmentInfo, nullptr);

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	vkCmdEndRendering(commandBuffer);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void VulkanApp::RenderGui()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//some imgui UI to test
	ImGui::ShowDemoWindow();

	//make imgui calculate internal draw structures
	ImGui::Render();
}

void VulkanApp::InitDefaultData()
{
	std::array<Vertex, 4> rect_vertices;

	rect_vertices[0].position = { 0.5,-0.5, 0 };
	rect_vertices[1].position = { 0.5,0.5, 0 };
	rect_vertices[2].position = { -0.5,-0.5, 0 };
	rect_vertices[3].position = { -0.5,0.5, 0 };

	rect_vertices[0].color = { 0,0, 0,1 };
	rect_vertices[1].color = { 0.5,0.5,0.5 ,1 };
	rect_vertices[2].color = { 1,0, 0,1 };
	rect_vertices[3].color = { 0,1, 0,1 };

	std::array<uint32_t, 6> rect_indices;

	rect_indices[0] = 0;
	rect_indices[1] = 1;
	rect_indices[2] = 2;

	rect_indices[3] = 2;
	rect_indices[4] = 1;
	rect_indices[5] = 3;

	rectangle = UploadMesh(rect_indices, rect_vertices);
}

AllocatedBuffer VulkanApp::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.usage = usage;
	bufferInfo.size = allocSize;
	bufferInfo.pNext = nullptr;

	VmaAllocationCreateInfo vmaAllocInfo{};
	vmaAllocInfo.usage = memoryUsage;
	vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	
	AllocatedBuffer newBuffer;
	VK_CHECK(vmaCreateBuffer(vmaAllocator, &bufferInfo, &vmaAllocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.info), "Couldn't create buffer");

	return newBuffer;
}

void VulkanApp::DestroyBuffer(const AllocatedBuffer& buffer)
{
	vmaDestroyBuffer(vmaAllocator, buffer.buffer, buffer.allocation);
}

GPUMeshBuffers VulkanApp::UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices)
{
	const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	GPUMeshBuffers newSurface;

	newSurface.vertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	newSurface.indexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo deviceAdressInfo{};
	deviceAdressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	deviceAdressInfo.buffer = newSurface.vertexBuffer.buffer;

	newSurface.vertexBufferAddress = vkGetBufferDeviceAddress(logicalDevice, &deviceAdressInfo);

	AllocatedBuffer stagingBuffer = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	void* data = stagingBuffer.allocation->GetMappedData();
	memcpy(data, vertices.data(), vertexBufferSize);
	memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

	ImmediateSubmit([&](VkCommandBuffer commandBuffer)
		{
		VkBufferCopy vertexCopy{ 0 };
		vertexCopy.dstOffset = 0;
		vertexCopy.srcOffset = 0;
		vertexCopy.size = vertexBufferSize;

		vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, newSurface.vertexBuffer.buffer, 1, &vertexCopy);

		VkBufferCopy indexCopy{ 0 };
		indexCopy.dstOffset = 0;
		indexCopy.srcOffset = vertexBufferSize;
		indexCopy.size = indexBufferSize;

		vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);

		}
	);

	DestroyBuffer(stagingBuffer);

	return newSurface;
}
