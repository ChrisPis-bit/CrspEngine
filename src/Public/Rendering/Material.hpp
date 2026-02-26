#pragma once
#include "Descriptors.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace crsp {
	struct BaseMaterial {
		glm::vec4 color;
	};

	class Material {
	public:

		Material(Device& device, VkRenderPass renderPass, VkDeviceSize uniformBufferSize, uint32_t textures, DescriptorPool& pool, VkDescriptorSetLayout globalSetLayout, const std::string& vertFilepath, const std::string& fragFilepath);
		~Material();

		void writeImage(uint32_t imageIndex, VkDescriptorImageInfo* imageInfo);
		void writeUniform(void* data);
		void build();

		VkDescriptorSet getDescriptorSet() { return descriptorSet; }
		VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
		void bindPipeline(VkCommandBuffer cmd) { pipeline->bind(cmd); }
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass, const std::string& vertFilepath, const std::string& fragFilepath);

		std::unique_ptr<Buffer> uniformBuffer;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;

		std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
		std::unique_ptr<DescriptorWriter> descriptorWriter;
		VkDescriptorSet descriptorSet;
		Device& device;
	};
}