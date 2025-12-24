#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

#include "Rendering/Window.hpp"
#include "Rendering/Device.hpp"
#include "Rendering/SwapChain.hpp"
#include "Rendering/Pipeline.hpp"

namespace crsp {


	class App {
	public:
		App();
		~App();

		void run();

		const uint32_t width = 800;
		const uint32_t height = 600;

	private:
		void initVulkan();
		void mainLoop();
		void cleanup();
		void createCommandBuffers();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void drawFrame();

		void recreateSwapChain();







		Window window;
		Device device;
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<Pipeline> pipeline;

		std::vector<VkCommandBuffer> commandBuffers;



		uint32_t currentImageIndex;
		int currentFrame = 0;





	};
}