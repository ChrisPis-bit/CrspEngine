#include "Rendering/Material.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include "Rendering/UIRenderer.hpp"

#include <cassert>
#include <stdexcept>

namespace crsp {

	Material::Material(Device& device, VkRenderPass renderPass, VkDeviceSize uniformBufferSize, uint32_t textures, DescriptorPool& pool, VkDescriptorSetLayout globalSetLayout, RenderDomain renderDomain, const std::string& vertFilepath, const std::string& fragFilepath)
		: device{ device }, renderDomain{ renderDomain }, uniformBufferSize{ uniformBufferSize }
	{
		// Uniform buffer
		if (uniformBufferSize > 0) {
			uniformBuffer = std::make_unique<Buffer>(device, uniformBufferSize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			uniformBuffer->map();
		}

		// Descriptor set layout
		auto materialSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		for (size_t i = 0; i < textures; i++)
		{
			materialSetLayout.addBinding(i + 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		}
		descriptorSetLayout = materialSetLayout.build();

		// Writer
		descriptorWriter = std::make_unique<DescriptorWriter>(*descriptorSetLayout, pool);

		// Pipeline
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass, vertFilepath, fragFilepath);
	}

	Material::~Material()
	{
		vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
	}

	void Material::writeImage(uint32_t imageIndex, VkDescriptorImageInfo* imageInfo)
	{
		descriptorWriter->writeImage(imageIndex + 1, imageInfo);
	}
	void Material::writeUniform(void* data)
	{
		uniformBuffer->writeToBuffer(data);
	}

	void Material::build()
	{
		if (uniformBufferSize > 0) {
			VkDescriptorBufferInfo uniformBufferDescriptor = uniformBuffer->descriptorInfo();
			descriptorWriter->writeBuffer(0, &uniformBufferDescriptor);
		}
		descriptorWriter->build(descriptorSet);
	}

	void Material::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		// TODO: better separate surface and UI render pipelines.
		uint32_t pushConstantSize;
		switch (renderDomain) {
		case RenderDomain::Surface3D:
			pushConstantSize = sizeof(ObjectPushConstantData);
			break;
		case RenderDomain::UI:
			pushConstantSize = sizeof(UIPushConstantData);
			break;
		default:
			pushConstantSize = sizeof(ObjectPushConstantData);
		}

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = pushConstantSize;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout, descriptorSetLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void Material::createPipeline(VkRenderPass renderPass, const std::string& vertFilepath, const std::string& fragFilepath)
	{
		assert(pipelineLayout != nullptr && "cannot create pipeline before creating the pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.multisampleInfo.rasterizationSamples = device.getMSAAsamples();
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;

		// TODO: better separate surface and UI render pipelines.
		if (renderDomain == RenderDomain::UI) {
			pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
			pipelineConfig.depthStencilInfo.depthTestEnable = VK_FALSE;
			pipelineConfig.depthStencilInfo.depthWriteEnable = VK_FALSE;
			pipelineConfig.vertexInputAttributeDescriptions = Vertex2D::getAttributeDescriptions();
			pipelineConfig.vertexInputBindingDescription = Vertex2D::getBindingDescriptions();
		}

		pipeline = std::make_unique<Pipeline>(device,
			vertFilepath,
			fragFilepath,
			pipelineConfig);
	}
}