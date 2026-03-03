#include "Rendering/ResourceManager.hpp"

#include <algorithm>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace crsp {
	ResourceManager::ResourceManager(Device& device, Renderer& renderer, GlobalDescriptorsManager& descriptorsManager) : device{ device }, renderer{ renderer }, descriptorsManager{ descriptorsManager }
	{
	}
	std::shared_ptr<FontAtlas> ResourceManager::loadFont(std::string filePath, std::string identifier)
	{
		if (auto pair = loadedFonts.find(identifier); pair != loadedFonts.end()) {
			return pair->second;
		}
		else {
			std::shared_ptr<FontAtlas> fontAtlas = std::make_shared<FontAtlas>(device, 16, 128, filePath, Texture2D::Filter::NEAREST);
			loadedFonts.insert({ identifier, fontAtlas });
			return fontAtlas;
		}
	}
	std::shared_ptr<FontAtlas> ResourceManager::getFont(std::string identifier)
	{
		if (auto pair = loadedFonts.find(identifier); pair != loadedFonts.end())
			return pair->second;
		else
			return nullptr;
	}
	std::shared_ptr<Texture2D> ResourceManager::loadTexture(std::string filePath, std::string identifier, Texture2D::Filter filter)
	{
		if (auto pair = loadedTextures.find(identifier); pair != loadedTextures.end()) {
			return pair->second;
		}
		else {
			std::shared_ptr<Texture2D> texture = Texture2D::createTextureFromFile(device, filePath, filter);
			loadedTextures.insert({ identifier, texture });
			return texture;
		}
	}
	std::shared_ptr<Texture2D> ResourceManager::getTexture(std::string identifier)
	{
		if (auto pair = loadedTextures.find(identifier); pair != loadedTextures.end())
			return pair->second;
		else
			return nullptr;
	}
	std::shared_ptr<Mesh> ResourceManager::createMesh(Mesh::Builder& builder, Mesh::Type type)
	{
		return std::make_shared<Mesh>(device, builder);
	}
	std::shared_ptr<Mesh> ResourceManager::loadMesh(std::string filePath, std::string identifier)
	{
		if (auto pair = loadedMeshes.find(identifier); pair != loadedMeshes.end()) {
			return pair->second;
		}
		else {
			std::shared_ptr<Mesh> mesh = Mesh::createMeshFromFile(device, PROJECT_PATH + filePath);
			loadedMeshes.insert({ identifier, mesh });
			return mesh;
		}
	}
	std::shared_ptr<Mesh> ResourceManager::getMesh(std::string identifier)
	{
		if (auto pair = loadedMeshes.find(identifier); pair != loadedMeshes.end())
			return pair->second;
		else
			return nullptr;
	}

	std::shared_ptr<Material> ResourceManager::createMaterial(uint32_t uniformBufferSize, uint32_t textureAmount, Material::RenderDomain renderDomain, const std::string& vertFilepath, const std::string& fragFilepath, std::string identifier)
	{
		if (auto pair = loadedMaterials.find(identifier); pair != loadedMaterials.end()) {
			return pair->second;
		}
		else {
			std::shared_ptr<Material> material = std::make_unique<Material>(device, 
				renderer.getSwapChainRenderPass(), 
				uniformBufferSize, textureAmount,
				*descriptorsManager.globalPool, descriptorsManager.getGlobalDescriptorSetLayout(),
				renderDomain,
				vertFilepath,
				fragFilepath);

			loadedMaterials.insert({ identifier, material });
			return material;
		}
	}
	std::shared_ptr<Material> ResourceManager::getMaterial(std::string identifier)
	{
		if (auto pair = loadedMaterials.find(identifier); pair != loadedMaterials.end())
			return pair->second;
		else
			return nullptr;
	}
}