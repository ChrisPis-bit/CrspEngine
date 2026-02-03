#pragma once

#include <glm/glm.hpp>
#include "PerlinNoise.hpp"

#include <Rendering/Mesh.hpp>

#include <vector>

namespace crsp {
	class TerrainGenerator {
		static constexpr float HEIGHT_MULT = 10;
		static constexpr float NOISE_SCALE = .01;
		static constexpr uint32_t OCTAVES = 4;

	public:
		TerrainGenerator(unsigned int seed = 123456u);
		~TerrainGenerator();

		void generateChunk(glm::vec2 offset, uint32_t resolution, float chunkSize, std::vector<Vertex> vertices, std::vector<uint16_t> indices);

	private:
		float getHeight(glm::vec2 pos);
		siv::PerlinNoise perlin;
	};
}