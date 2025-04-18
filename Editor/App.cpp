#include "App.h"

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <dwmapi.h>

#include "Engine/Managers/InputManager.h"

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->GetEngine()->WindowResizeEvent();
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	if (action == GLFW_PRESS)
	{
		InputManager::Instance()->SetKeyboardKey(key, true);
	}
	else if (action == GLFW_RELEASE)
	{
		InputManager::Instance()->SetKeyboardKey(key, false);
	}
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (action == GLFW_PRESS)
	{
		InputManager::Instance()->SetMouseButton(button, true);
	}
	else if (action == GLFW_RELEASE)
	{
		InputManager::Instance()->SetMouseButton(button, false);
	}
}

void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

	InputManager::Instance()->SetMousePosition(xpos, ypos);
}

App::App()
{
	Init();
}

App::~App()
{
	Clear();
}

void App::Run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		engine->SimulateFrame();
	}
}

void App::Init()
{
	InitWindow();
	InitEngine();
	InitGui();

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
}

void App::Clear()
{
	gui.reset();
	engine.reset();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void App::InitWindow()
{
	CHECK_ERROR_NULL_MESSAGE(glfwInit(), "Couldn't initialize glfw!");

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	const int32_t width = static_cast<int32_t>(videoMode->width * 0.9f);
	const int32_t height = static_cast<int32_t>(videoMode->height * 0.9f);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "Vulkan Engine", nullptr, nullptr);
	CHECK_ERROR_NULL_MESSAGE(window, "Couldn't create glfw window!");

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

	HWND hwnd = glfwGetWin32Window(window);
	COLORREF color = RGB(0, 0, 0);
	const DWORD DWMWA_CAPTION_COLOR = 35;
	HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

	const int32_t windowLeft = videoMode->width / 2 - width / 2;
	const int32_t windowTop = videoMode->height / 2 - height / 2;
	glfwSetWindowPos(window, windowLeft, windowTop);

	{
		int width, height, channels;
		unsigned char* pixels = stbi_load("../Assets/Engine/Window/Logo.png", &width, &height, &channels, 0);

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
}

void App::InitEngine()
{
	engine = std::make_shared<Engine>();

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> glfwExtensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

	engine->SetRequiredWindowExtensions(glfwExtensionNames);
	
	engine->SetSurfaceCreationFunction(
		[&](const Vk::Instance* const instance, VkSurfaceKHR* surface) -> void {
			VK_CHECK_MESSAGE(glfwCreateWindowSurface(instance->Value(), window, nullptr, surface), "Failed to create window surface");
		}
	);

	engine->SetWindowExtentFunction(
		[&]() -> std::pair<int, int> {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			return std::make_pair(width, height);
		}
	);

	engine->Initialize();
}

void App::InitGui()
{
	gui = std::make_shared<Gui>(window);

	engine->SetGuiRenderFunction(
		[&](VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex) -> void {
			gui->Render(commandBuffer, registry, resourceManager, frameIndex);
		}
	);
}
