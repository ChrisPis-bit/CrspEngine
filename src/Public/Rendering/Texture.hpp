#pragma once

#include "Device.hpp"
#include <memory>

namespace crsp {
	class Texture2D {
	public:
		enum class Filter {
			LINEAR,
			NEAREST,
		};

		struct Builder {
			int width;
			int height;
			int channels;
			void* data;
			VkFormat format;
			Filter filter;

			void loadTexture(const std::string& filepath);
		};

		Texture2D(Device& device, const Texture2D::Builder& builder);
		~Texture2D();

		VkDescriptorImageInfo descriptorInfo();

		static std::unique_ptr<Texture2D> createTextureFromFile(Device& device, const std::string& filepath, Filter filter = Filter::LINEAR);

	private:
		void createTextureImage(const Texture2D::Builder& builder);
		void createTextureImageView();
		void createTextureSampler();

		Device& device;

		VkImageView textureImageView;
		VkSampler textureSampler;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkFormat format;
		VkFilter filter;

		uint32_t mipLevels;
	};
}