#include <Terrain/TerrainGenerator.hpp>

namespace crsp {
	TerrainGenerator::TerrainGenerator(unsigned int seed)
	{
		const siv::PerlinNoise::seed_type perlinSeed = seed;

		perlin = siv::PerlinNoise(perlinSeed);
	}

	void TerrainGenerator::generateChunk(glm::vec2 offset, uint32_t resolution, float chunkSize, std::vector<Vertex> vertices, std::vector<uint16_t> indices)
	{
		vertices.clear();
		indices.clear();

		float texelSize = chunkSize / resolution;

		for (size_t x = 0; x < resolution; x++)
		{
			for (size_t y = 0; y < resolution; y++)
			{
				// Get position
				glm::vec2 gridPosition = glm::vec2(x * texelSize, y * texelSize);
				float height = getHeight(gridPosition);
				glm::vec3 position = glm::vec3(gridPosition.x, gridPosition.y, height);

				// Get Normal
				//vec3 offset

				// Create vertex
				Vertex vertex{};
				vertex.position = position;
				vertex.texCoord = glm::vec2(x, y) / chunkSize;

				vertices.push_back(vertex);
			}
		}
	}
	float TerrainGenerator::getHeight(glm::vec2 pos)
	{
		return perlin.octave2D_01(pos.x * NOISE_SCALE, pos.y * NOISE_SCALE, OCTAVES) * HEIGHT_MULT;
	}
} // namespace