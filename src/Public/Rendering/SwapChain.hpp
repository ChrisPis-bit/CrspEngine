#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace crsp{

	class SwapChain {
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device& device, VkExtent2D windowExtent);
		SwapChain(Device& device, VkExtent2D windowExtent, std::shared_ptr<SwapChain> oldSwapChain);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		VkRenderPass getRenderPass() { return renderPass; }
		VkFramebuffer getFrameBuffer(uint32_t index) { return swapChainFramebuffers[index]; }
		VkImageView getImageView(uint32_t index) { return swapChainImageViews[index]; }

		VkExtent2D getSwapChainExtent() { return swapChainExtent; }
		float extentAspectRatio() {
			return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
		}

		VkResult getNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool compareSwapFormats(const SwapChain& swapChain) const {
			return swapChain.swapChainImageFormat == swapChainImageFormat;
		}

		size_t getImageCount() { return swapChainImages.size(); }

	private:
		void init();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void createSwapChain();
		void createImageViews();
		void createDepthResources();

		void createFramebuffers();

		void createRenderPass();

		void createSyncObjects();

		void cleanupSwapChain();
		void cleanupDepthResources();

		VkFormat findDepthFormat();

		Device& device;

		VkRenderPass renderPass;

		VkSwapchainKHR swapChain;
		std::shared_ptr<SwapChain> oldSwapChain;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImage> depthImages;
		VkFormat swapChainImageFormat;
		VkFormat depthFormat;

		VkExtent2D swapChainExtent;
		VkExtent2D windowExtent;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkImageView> depthImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
		std::vector<VkDeviceMemory> depthImageMemory;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		uint32_t currentFrame = 0;
	};
} // namespace