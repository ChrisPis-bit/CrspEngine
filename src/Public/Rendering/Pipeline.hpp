#pragma once

#include "Device.hpp"


namespace crsp {
	class Pipeline {
	public:
		Pipeline(Device& device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline operator=(const Pipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

	private:
		void createGraphicsPipeline(VkRenderPass renderPass, VkPipelineLayout pipelineLayout);

		VkShaderModule createShaderModule(const std::vector<char>& code);

		VkPipeline graphicsPipeline;

		Device& device;
	};
} // namespace