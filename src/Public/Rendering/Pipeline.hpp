#pragma once

#include "Device.hpp"


namespace crsp {
	class Pipeline {
	public:
		Pipeline(Device& device, VkRenderPass renderPass);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline operator=(const Pipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

	private:
		void createGraphicsPipeline(VkRenderPass renderPass);

		VkShaderModule createShaderModule(const std::vector<char>& code);

		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		Device& device;
	};
} // namespace