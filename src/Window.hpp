#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace crsp {
	class Window {
	public:
		Window(int width, int height, std::string name);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		GLFWwindow* getGLFWwindow() { return window; }
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		bool wasResized() { return framebufferResized; }
		void resetResizedFlag() { framebufferResized = false; }

		void createSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		void init();

		GLFWwindow* window;
		int width, height;
		std::string name;
		bool framebufferResized = false;

	};

} // namespace