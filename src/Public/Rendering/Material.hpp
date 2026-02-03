#pragma once
#include "Descriptors.hpp"
#include "Pipeline.hpp"

namespace crsp {
	struct MaterialLayout {
		DescriptorSetLayout setLayout;
		PipelineConfigInfo pipelineConfig;
	};
	struct MaterialDescriptorPool {
		DescriptorPool pool;
		uint32_t maxMaterials;
	};
}