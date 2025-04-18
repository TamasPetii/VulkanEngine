#include "ViewportWindow.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/CameraComponent.h"
#include "Engine/Systems/CameraSystem.h"

void ViewportWindow::Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, std::set<VkDescriptorSet>& textureSet, uint32_t frameIndex)
{
	static Window::Config windowConfig{
		.name = "Viewport-Window",
		.flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar
	};

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	Window::RenderWindow(windowConfig, [&]() -> void
		{
			if (ImGui::BeginMenuBar())
			{
				GizmoConfigMenu();
				ViewportImageMenu();
				ImGui::EndMenuBar();
			}

			auto viewPortSize = ImGui::GetContentRegionAvail();

			uint32_t nextFrameIndex = (frameIndex + 1) % Settings::FRAMES_IN_FLIGHT;

			auto frameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex);
			auto nextFrameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", nextFrameIndex);

			if (static_cast<uint32_t>(viewPortSize.x) != nextFrameBuffer->GetSize().width || static_cast<uint32_t>(viewPortSize.y) != nextFrameBuffer->GetSize().height)
				resourceManager->GetVulkanManager()->MarkFrameBufferToResize("Main", nextFrameIndex, static_cast<uint32_t>(viewPortSize.x), static_cast<uint32_t>(viewPortSize.y));

			auto sampler = resourceManager->GetVulkanManager()->GetSampler("Nearest")->Value();
			auto imageView = frameBuffer->GetImage(GetViewportImageName())->GetImageView();

			VkDescriptorSet image = ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			textureSet.insert(image);

			ImGui::Image((ImTextureID)image, viewPortSize);

			RenderGizmo(registry);
			GetClickedActiveEntity(registry, frameBuffer);		
		}
	);

	ImGui::PopStyleVar();
}

std::string ViewportWindow::GetViewportImageName()
{
	switch (viewportImage)
	{
	case 0: return "Color"; break;
	case 1: return "Normal"; break;
	default: return "Main";
	}
}

void ViewportWindow::GizmoConfigMenu()
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
}

void ViewportWindow::ViewportImageMenu()
{
	if (ImGui::BeginMenu("Image"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 0.0f));
		if (ImGui::BeginChild("##ViewportImage", ImVec2(255, 210), ImGuiChildFlags_AlwaysUseWindowPadding))
		{
			ImGui::RadioButton("Color", &viewportImage, 0);
			ImGui::RadioButton("Normal", &viewportImage, 1);
			ImGui::RadioButton("Main", &viewportImage, 2);

			ImGui::EndChild();
		}
		ImGui::PopStyleVar();

		ImGui::EndMenu();
	}
}

void ViewportWindow::RenderGizmo(std::shared_ptr<Registry> registry)
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

void ViewportWindow::GetClickedActiveEntity(std::shared_ptr<Registry> registry, std::shared_ptr<Vk::FrameBuffer> frameBuffer)
{
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
		}
	}
}
