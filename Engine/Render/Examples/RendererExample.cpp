/*

Renderer::Renderer()
{
	Init();
}

void Renderer::Render()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto swapChain = Vk::VulkanContext::GetContext()->GetSwapChain();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto presentQueue = device->GetQueue(Vk::QueueType::PRESENTATION);

	vkWaitForFences(device->Value(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device->Value(), swapChain->Value(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetFences(device->Value(), 1, &inFlightFence);
	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass->Value();
	renderPassInfo.framebuffer = frameBuffer->Value();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = frameBuffer->GetSize();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {1.0f, 1.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->Value());

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

	vkCmdEndRenderPass(commandBuffer);

	Vk::Image::TransitionImageLayout(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

	Vk::Image::CopyImageToImage(commandBuffer, frameBuffer->GetImage("colorImage")->Value(), frameBuffer->GetSize(), swapChain->GetImages()[imageIndex], swapChain->GetExtent());

	Vk::Image::TransitionImageLayout(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_NONE);

	VK_CHECK_MESSAGE(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer!");

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VK_CHECK_MESSAGE(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence), "Failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapChain->Value() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Renderer::Init()
{
	auto swapChainExtent = Vk::VulkanContext::GetContext()->GetSwapChain()->GetExtent();

	shaderModuls["BasicVert"] = std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaderModuls["BasicFrag"] = std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

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

	Vk::RenderPassBuilder renderPassBuilder;
	renderPassBuilder.RegisterSubpass("mainSubpass", 0);
	renderPassBuilder.AttachImageDescription("colorImage", 0, colorImageSpec.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	renderPassBuilder.AttachImageReferenceToSubpass("mainSubpass", "colorImage", VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachDepthDescription(1, depthImageSpec.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachDepthReferenceToSubpass("mainSubpass", VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachSubpassDependency("mainSubpass", Vk::DependencyStage::SRC, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_NONE);
	renderPassBuilder.AttachSubpassDependency("mainSubpass", Vk::DependencyStage::DST, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
	renderPass = renderPassBuilder.BuildRenderPass();

	Vk::FrameBufferBuilder frameBufferBuilder;
	frameBufferBuilder.SetInitialSize(swapChainExtent.width, swapChainExtent.height);
	frameBufferBuilder.AttachImageSpec("colorImage", 0, colorImageSpec);
	frameBufferBuilder.AttachDepthSpec(1, depthImageSpec);
	frameBuffer = frameBufferBuilder.BuildFrameBuffer(renderPass);

	std::vector<VkVertexInputBindingDescription> vertexBindingDescirption = { Vertex::GetBindingDescription() };
	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription = Vertex::GetAttributeDescriptions();

	Vk::GraphicsPipelineBuilder pipelineBuilder;
	pipelineBuilder.SetDefaultInfos();
	pipelineBuilder.SetShaderStage(shaderModuls["BasicVert"]);
	pipelineBuilder.SetShaderStage(shaderModuls["BasicFrag"]);
	pipelineBuilder.SetInputAssemblyTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.DefaultDynamicStates();
	pipelineBuilder.DefaultRasterizationInfo();
	pipelineBuilder.DefaultColorBlendOptions();
	pipelineBuilder.SetDepthStencilOptions(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS);
	//pipelineBuilder.SetVertexInputInfo();
	//pipelineBuilder.SetPushConstantRange();
	//pipelineBuilder.SetDescriptorSetLayout();
	graphicsPipeline = pipelineBuilder.BuildPipeline(renderPass, 0);

	InitCommandPool();
	InitCommandBuffer();
	InitSyncronization();
}

void Renderer::InitCommandPool()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto& queueFamilyIndices = Vk::VulkanContext::GetContext()->GetPhysicalDevice()->GetQueueFamilyIndices();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &commandPool), "Failed to create command pool!");
}

void Renderer::InitCommandBuffer()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &commandBuffer), "Failed to allocate command buffers!");
}

void Renderer::InitSyncronization()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK_MESSAGE(vkCreateSemaphore(device->Value(), &semaphoreInfo, nullptr, &imageAvailableSemaphore), "Failed to create image available semaphore!");
	VK_CHECK_MESSAGE(vkCreateSemaphore(device->Value(), &semaphoreInfo, nullptr, &renderFinishedSemaphore), "Failed to create render finished semaphore!");
	VK_CHECK_MESSAGE(vkCreateFence(device->Value(), &fenceInfo, nullptr, &inFlightFence), "Failed to create in-flight fence!");
}

*/