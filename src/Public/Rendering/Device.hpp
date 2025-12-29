#pragma once

#include "Window.hpp"

#include <vector>
#include <optional>

namespace crsp {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class Device {

	public:
		Device(Window& window);
		~Device();

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		VkInstance getInstance() { return instance; }
		VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
		VkDevice getDevice() { return device; }
		VkSurfaceKHR getSurface() { return surface; }
		VkCommandPool getCommandPool() { return commandPool; }
		VkQueue getGraphicsQueue() { return graphicsQueue; }
		VkQueue getPresentQueue() { return presentQueue; }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
		QueueFamilyIndices findQueueFamilies() { return findQueueFamilies(physicalDevice); }

		// Buffer functions
		void createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void copyBufferToImage(
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void createImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);
		void createImageView(VkImage image, VkFormat format, VkImageView& imageView);

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		bool hasStencilComponent(VkFormat format) {
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

	private:
		void createVulkanInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();

		bool isDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void createCommandPool();
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		bool checkValidationLayerSupport();
		bool setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkInstance instance;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkCommandPool commandPool;
		VkSurfaceKHR surface;

		VkDebugUtilsMessengerEXT debugMessenger;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
		};

#ifdef DEBUG
		const bool enableValidationLayers = true;
#else
		const bool enableValidationLayers = false;
#endif // DEBUG

	};
} // namespace