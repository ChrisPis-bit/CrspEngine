#pragma once

#include "Device.hpp"
#include "GameObject.hpp"
#include "Pipeline.hpp"

#include <memory>

namespace crsp {
	class MeshRenderSystem {
	public:
		MeshRenderSystem(Device& device, VkRenderPass renderPass);
		~MeshRenderSystem();

		MeshRenderSystem(const MeshRenderSystem&) = delete;
		MeshRenderSystem operator=(const MeshRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		Device& device;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace