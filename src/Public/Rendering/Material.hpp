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
		enum class RenderDomain {
			Surface3D,
			UI
		};

		Material(Device& device, VkRenderPass renderPass, VkDeviceSize uniformBufferSize, uint32_t textures, DescriptorPool& pool, VkDescriptorSetLayout globalSetLayout, RenderDomain renderDomain, const std::string& vertFilepath, const std::string& fragFilepath);
		~Material();

		void writeImage(uint32_t imageIndex, VkDescriptorImageInfo* imageInfo);
		void writeUniform(void* data);
		void build();

		VkDescriptorSet getDescriptorSet() { return descriptorSet; }
		VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
		void bindPipeline(VkCommandBuffer cmd) { pipeline->bind(cmd); }
		RenderDomain getRenderDomain() { return renderDomain; }
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass, const std::string& vertFilepath, const std::string& fragFilepath);

		VkDeviceSize uniformBufferSize;
		std::unique_ptr<Buffer> uniformBuffer;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;

		std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
		std::unique_ptr<DescriptorWriter> descriptorWriter;
		VkDescriptorSet descriptorSet;
		Device& device;
		RenderDomain renderDomain;
	};
}