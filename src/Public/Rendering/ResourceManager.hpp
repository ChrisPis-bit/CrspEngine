#pragma once
#include "Rendering/Device.hpp"
#include "Rendering/GlobalDescriptorsManager.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"

#include <memory.h>
#include <unordered_map>
#include <vector>

namespace crsp {
	struct GlyphInfo {
		float u0, v0, u1, v1;
		int width, height;
		int bearingX, bearingY;
		int advance;
	};

	class FontAtlas {
	public:
		std::unordered_map<char, GlyphInfo> glyphs;
		std::vector<unsigned char> bitmap;
		int width;
		int height;
	};

	/// <summary>
	/// Handles loading, creating and storing shared resources.
	/// </summary>
	class ResourceManager {
	public:
		ResourceManager(Device& device, Renderer& renderer, GlobalDescriptorsManager& descriptorsManager);
		~ResourceManager() = default;

		std::shared_ptr<Texture2D> loadTexture(std::string filePath, std::string identifier);
		std::shared_ptr<Texture2D> getTexture(std::string identifier);

		std::shared_ptr<Mesh> createMesh(Mesh::Builder& builder);
		std::shared_ptr<Mesh> loadMesh(std::string filePath, std::string identifier);
		std::shared_ptr<Mesh> getMesh(std::string identifier);

		std::shared_ptr<Material> createMaterial(uint32_t uniformBufferSize, uint32_t textureAmount, Material::RenderDomain renderDomain, const std::string& vertFilepath, const std::string& fragFilepath, std::string identifier);
		std::shared_ptr<Material> getMaterial(std::string identifier);

		std::shared_ptr<Texture2D> atlasTexture;
	private:
		Device& device;
		GlobalDescriptorsManager& descriptorsManager;
		Renderer& renderer;

		std::unordered_map<std::string, std::shared_ptr<Texture2D>> loadedTextures;
		std::unordered_map<std::string, std::shared_ptr<Mesh>> loadedMeshes;
		std::unordered_map<std::string, std::shared_ptr<Material>> loadedMaterials;
	};
}