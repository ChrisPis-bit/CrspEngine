#pragma once

#include "Device.hpp"
#include "GameObject.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"

#include <memory>

namespace crsp {

	struct ObjectPushConstantData {
		glm::mat4 normalMatrix{ 1.f };
		glm::mat4 modelMatrix{ 1.f };
	};

	class MeshRenderSystem {
	public:
		MeshRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~MeshRenderSystem();

		MeshRenderSystem(const MeshRenderSystem&) = delete;
		MeshRenderSystem operator=(const MeshRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		Device& device;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace