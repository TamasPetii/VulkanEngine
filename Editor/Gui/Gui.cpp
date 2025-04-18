#include "Gui.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Editor/Gui/Windows/ComponentWindow.h"
#include "Editor/Gui/Windows/EntityWindow.h"
#include "Editor/Gui/Windows/GlobalSettingsWindow.h"
#include "Engine/Systems/CameraSystem.h"

Gui::Gui(GLFWwindow* window)
{
	Initialize(window);
}

Gui::~Gui()
{
	Cleanup();
}

void Gui::Initialize(GLFWwindow* window)
{
	auto instance = Vk::VulkanContext::GetContext()->GetInstance();
	auto physicalDevice = Vk::VulkanContext::GetContext()->GetPhysicalDevice();
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto swapChain = Vk::VulkanContext::GetContext()->GetSwapChain();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

	VK_CHECK_MESSAGE(vkCreateDescriptorPool(device->Value(), &poolInfo, nullptr, &imguiPool), "Couldn't create imgui descriptor pool");

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance->Value();
	init_info.PhysicalDevice = physicalDevice->Value();
	init_info.Device = device->Value();
	init_info.QueueFamily = physicalDevice->GetQueueFamilyIndices().graphicsFamily.value();
	init_info.Queue = device->GetQueue(Vk::QueueType::GRAPHICS);
	init_info.DescriptorPool = imguiPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = static_cast<uint32_t>(swapChain->GetImages().size());
	init_info.ImageCount = static_cast<uint32_t>(swapChain->GetImages().size());
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.UseDynamicRendering = true;
	init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapChain->GetImageFormat();

	ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplVulkan_CreateFontsTexture();

	SetStyle();
}

void Gui::Cleanup()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDeviceWaitIdle(device->Value());

	ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(device->Value(), imguiPool, nullptr);
}

void Gui::Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	for (VkDescriptorSet descriptorSet : imguiDescriptorSets[frameIndex])
		ImGui_ImplVulkan_RemoveTexture(descriptorSet);
	imguiDescriptorSets[frameIndex].clear();

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

	ImGui::ShowDemoWindow();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Gizmo")) 
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 0.0f));
				if (ImGui::BeginChild("##GizmoWindow", ImVec2(255, 210), ImGuiChildFlags_AlwaysUseWindowPadding))
				{
					ImGui::SeparatorText("Operation");

					int mode = static_cast<int>(gizmoConfig.mode);
					ImGui::RadioButton("Local", &mode, static_cast<int>(ImGuizmo::LOCAL)); ImGui::SameLine();
					ImGui::RadioButton("World", &mode, static_cast<int>(ImGuizmo::WORLD));
					gizmoConfig.mode = static_cast<ImGuizmo::MODE>(mode);

					int operation = static_cast<int>(gizmoConfig.operation);
					ImGui::RadioButton("Trasnlate", &operation, static_cast<int>(ImGuizmo::TRANSLATE)); ImGui::SameLine();
					ImGui::RadioButton("Rotate", &operation, static_cast<int>(ImGuizmo::ROTATE)); ImGui::SameLine();
					ImGui::RadioButton("Scale", &operation, static_cast<int>(ImGuizmo::SCALE));
					gizmoConfig.operation = static_cast<ImGuizmo::OPERATION>(operation);

					ImGui::SeparatorText("Snapping");

					ImGui::Text("Enable"); ImGui::SameLine(80);
					ImGui::Checkbox("##EnableSnap", &gizmoConfig.useSnap);

					ImGui::Text("Translate"); ImGui::SameLine(80);
					ImGui::DragFloat3("##TranslateSnap", gizmoConfig.snapTranslate);

					ImGui::Text("Rotate"); ImGui::SameLine(80);
					ImGui::DragFloat("##RotateSnap", &gizmoConfig.snapAngle);

					ImGui::Text("Scale"); ImGui::SameLine(80);
					ImGui::DragFloat("##ScaleSnap", &gizmoConfig.snapScale);

					ImGui::EndChild();
				}
				ImGui::PopStyleVar();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}


		auto viewPortSize = ImGui::GetContentRegionAvail();
		
		uint32_t nextFrameIndex = (frameIndex + 1) % Settings::FRAMES_IN_FLIGHT;

		auto frameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex);
		auto nextFrameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", nextFrameIndex);

		if (static_cast<uint32_t>(viewPortSize.x) != nextFrameBuffer->GetSize().width || static_cast<uint32_t>(viewPortSize.y) != nextFrameBuffer->GetSize().height)
			resourceManager->GetVulkanManager()->MarkFrameBufferToResize("Main", nextFrameIndex, static_cast<uint32_t>(viewPortSize.x), static_cast<uint32_t>(viewPortSize.y));

		auto sampler = resourceManager->GetVulkanManager()->GetSampler("Nearest")->Value();
		auto imageView = frameBuffer->GetImage("Main")->GetImageView();
		
		VkDescriptorSet image = ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		imguiDescriptorSets[frameIndex].insert(image);

		ImGui::Image((ImTextureID)image, viewPortSize);

		RenderGizmo(registry);

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGuizmo::IsUsing())
		{
			int mouseX = ImGui::GetMousePos().x - ImGui::GetWindowContentRegionMin().x - ImGui::GetWindowPos().x;
			int mouseY = ImGui::GetMousePos().y - ImGui::GetWindowContentRegionMin().y - ImGui::GetWindowPos().y;
			int contentRegionX = ImGui::GetContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
			int contentRegionY = ImGui::GetContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
			mouseY = contentRegionY - mouseY;

			if (mouseX >= 0 && mouseX < contentRegionX &&
				mouseY >= 0 && mouseY < contentRegionY)
			{
				VkDeviceSize bufferSize = sizeof(uint32_t);

				Vk::BufferConfig stagingConfig;
				stagingConfig.size = bufferSize;
				stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT;
				stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				Vk::Buffer stagingBuffer{ stagingConfig };

				Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
					[&](VkCommandBuffer commandBuffer) -> void 
					{
						Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("Entity")->Value(),
							VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT);

						Vk::Buffer::CopyImageToBuffer(commandBuffer, frameBuffer->GetImage("Entity")->Value(), stagingBuffer.Value(), 1, 1, mouseX, frameBuffer->GetSize().height - mouseY);
					}
				);

				uint32_t entityID;
				void* mappedBuffer = stagingBuffer.MapMemory();
				memcpy(&entityID, mappedBuffer, (size_t)bufferSize);
				stagingBuffer.UnmapMemory();

				registry->SetActiveEntity(entityID);
				std::cout << std::format("Clicked on position ({},{}) found entity {}", mouseX, mouseY, entityID) << "\n";
			}
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

	EntityWindow::Render(registry, resourceManager, frameIndex);
	ComponentWindow::Render(registry, resourceManager, frameIndex);
	GlobalSettingsWindow::Render(registry, resourceManager, frameIndex);

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void Gui::SetStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 0.95f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 0.59f, 1.00f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.57f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.57f, 1.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.00f, 0.00f, 0.00f, 0.12f);
	colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.47f, 0.83f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.47f, 0.83f, 0.90f);
	colors[ImGuiCol_NavCursor] = ImVec4(0.00f, 0.47f, 0.83f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	// Sizes
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.FramePadding = ImVec2(6.0f, 4.0f);
	style.ItemSpacing = ImVec2(8.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
	style.IndentSpacing = 12.0f;
	style.ScrollbarSize = 12.0f;
	style.GrabMinSize = 12.0f;

	// Borders
	style.WindowBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;

	// Rounding
	style.WindowRounding = 4.0f;
	style.ChildRounding = 4.0f;
	style.FrameRounding = 4.0f;
	style.PopupRounding = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.GrabRounding = 2.0f;

	// Tabs
	style.TabBorderSize = 1.0f;
	style.TabBarBorderSize = 2.0f;
	style.TabCloseButtonMinWidthSelected = -1.0f; // Always
	style.TabCloseButtonMinWidthUnselected = -1.0f; // Always
	style.TabRounding = 4.0f;

	// Tables
	style.CellPadding = ImVec2(4.0f, 4.0f);
	style.TableAngledHeadersAngle = 25.0f * (3.1415926535f / 180.0f); // Convert degrees to radians
	style.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);

	// Windows
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;

	// Widgets
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	style.SeparatorTextBorderSize = 2.0f;
	style.SeparatorTextAlign = ImVec2(0.00f, 0.5f);
	style.SeparatorTextPadding = ImVec2(20.f, 2.0f);
	style.LogSliderDeadzone = 4.0f;

	// Docking
	style.DockingSeparatorSize = 2.0f;

	// Tooltips
	style.DisplayWindowPadding = ImVec2(15.0f, 15.0f);
	style.DisplaySafeAreaPadding = ImVec2(3.0f, 3.0f);
}

void Gui::RenderGizmo(std::shared_ptr<Registry> registry)
{
	auto [cameraPool, transformPool] = registry->GetPools<CameraComponent, TransformComponent>();

	if (!cameraPool || !transformPool)
		return;

	auto cameraEntity = CameraSystem::GetMainCameraEntity(registry);
	auto cameraComponent = cameraPool->GetData(cameraEntity);
	auto activeEntity = registry->GetActiveEntity();

	if (activeEntity != NULL_ENTITY && transformPool->HasComponent(activeEntity))
	{
		float* snapValue = nullptr;

		switch (gizmoConfig.operation) {
		case ImGuizmo::TRANSLATE:
			snapValue = gizmoConfig.snapTranslate;
			break;
		case ImGuizmo::ROTATE:
			snapValue = &gizmoConfig.snapAngle;
			break;
		case ImGuizmo::SCALE:
			snapValue = &gizmoConfig.snapScale;
			break;
		}

		glm::mat4 viewMatrix = cameraComponent->view;
		glm::mat4 projectionMatrix = cameraComponent->proj;
		auto transformComponent = registry->GetComponent<TransformComponent>(activeEntity);
		auto transform = transformComponent->transform;

		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ImGuizmo::SetDrawlist();
		ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), gizmoConfig.operation, gizmoConfig.mode, glm::value_ptr(transform), NULL, gizmoConfig.useSnap ? snapValue : NULL, NULL, NULL);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, scale, rotation;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

			glm::vec3 lastTranslation, lastScale, lastRotation;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformComponent->transform), glm::value_ptr(lastTranslation), glm::value_ptr(lastRotation), glm::value_ptr(lastScale));

			switch (gizmoConfig.operation)
			{
			case ImGuizmo::TRANSLATE:
				transformComponent->translation += translation - lastTranslation;
				transformPool->SetBit<UPDATE_BIT>(activeEntity);
				break;
			case ImGuizmo::ROTATE:
				transformComponent->rotation += rotation - lastRotation;
				transformPool->SetBit<UPDATE_BIT>(activeEntity);
				break;
			case ImGuizmo::SCALE:
				transformComponent->scale += scale - lastScale;
				transformPool->SetBit<UPDATE_BIT>(activeEntity);
				break;
			}
		}

		if (ImGui::IsKeyPressed(ImGuiKey_1))
		{
			gizmoConfig.operation = ImGuizmo::TRANSLATE;
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_2))
		{
			gizmoConfig.operation = ImGuizmo::ROTATE;
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_3))
		{
			gizmoConfig.operation = ImGuizmo::SCALE;
		}
	}
}
