#pragma once

#include "Device.hpp"

namespace crsp {
	class Texture2D {
	public:
		Texture2D(Device& device, const std::string& filePath);
		~Texture2D();

		VkDescriptorImageInfo descriptorInfo();

	private:
		void createTextureImage(const std::string& filePath);
		void createTextureImageView();
		void createTextureSampler();

		Device& device;

		VkImageView textureImageView;
		VkSampler textureSampler;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;

		uint32_t mipLevels;
	};
}