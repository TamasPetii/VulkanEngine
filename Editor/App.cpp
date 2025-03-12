#include "App.h"

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <dwmapi.h>

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	//app-> = true;
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

		engine->Update();
		engine->Render();
	}
}

void App::Init()
{
	InitWindow();
	InitEngine();
}

void App::Clear()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void App::InitWindow()
{
	CHECK_ERROR_NULL(glfwInit());

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	const int32_t width = static_cast<int32_t>(videoMode->width * 0.9f);
	const int32_t height = static_cast<int32_t>(videoMode->height * 0.9f);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "Vulkan Engine", nullptr, nullptr);
	CHECK_ERROR_NULL(window);

	glfwSetWindowUserPointer(window, this);
	//glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

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

	for (uint32_t i = 0; i < glfwExtensionCount; i++)
		engine->SetRequiredExtension(glfwExtensionNames[i]);
}
