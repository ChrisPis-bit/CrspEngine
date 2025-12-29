#pragma once

#include "Device.hpp"

namespace crsp {
	class Texture2D {
	public:
		Texture2D(Device& device);
		~Texture2D();

		VkDescriptorImageInfo descriptorInfo();

	private:
		void createTextureImage();
		void createTextureImageView();
		void createTextureSampler();

		Device& device;

		VkImageView textureImageView;
		VkSampler textureSampler;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
	};
}