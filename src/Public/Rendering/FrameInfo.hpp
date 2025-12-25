#pragma once

#include "Camera.hpp"

#include <vulkan/vulkan.h>

namespace crsp {
	struct FrameInfo {
		int frameIndex; // Index of the current frame being rendered on the swapchain
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
} // namespace