#include "Window.hpp"

#include <stdexcept>

namespace crsp {
	Window::Window(int width, int height, std::string name): width(width), height(height), name(name)
	{
		init();
	}

	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void Window::init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		appWindow->framebufferResized = true;
		appWindow->width = width;
		appWindow->height = height;
	}
}