#include "Surface.h"

Vk::Surface::Surface(const Vk::Instance* const instance, const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& creationFunction) :
	instance(instance)
{
	Init(creationFunction);
}

Vk::Surface::~Surface()
{
	Destroy();
}

VkSurfaceKHR Vk::Surface::Value() const
{
	return surface;
}

void Vk::Surface::Init(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& creationFunction)
{
	creationFunction(instance, &surface);
}

void Vk::Surface::Destroy()
{
	vkDestroySurfaceKHR(instance->Value(), surface, nullptr);
}
