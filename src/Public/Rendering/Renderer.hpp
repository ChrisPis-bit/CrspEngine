#pragma once

#include "Window.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"

#include <memory>

namespace crsp {
	class Renderer {
	public:
		Renderer(Window& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		uint32_t getFrameIndex() { return currentFrame; }
		float getAspectRatio() { return swapChain->extentAspectRatio(); }

		VkRenderPass getSwapChainRenderPass() { return swapChain->getRenderPass(); }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();

		void recreateSwapChain();

		Window& window;
		Device& device;

		std::unique_ptr<SwapChain> swapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrame = 0;
	};
}