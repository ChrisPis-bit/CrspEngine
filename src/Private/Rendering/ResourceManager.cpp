#include "Rendering/ResourceManager.hpp"

namespace crsp {
	ResourceManager::ResourceManager(Device& device, Renderer& renderer, GlobalDescriptorsManager& descriptorsManager) : device{ device }, renderer{ renderer }, descriptorsManager{ descriptorsManager }
	{
	}
	std::shared_ptr<Texture2D> ResourceManager::loadTexture(std::string filePath, std::string identifier)
	{
		if (auto pair = loadedTextures.find(identifier); pair != loadedTextures.end()) {
			return pair->second;
		}
		else {
			auto texture = std::make_shared<Texture2D>(device, filePath);
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

	std::shared_ptr<Material> ResourceManager::createMaterial(uint32_t uniformBufferSize, uint32_t textureAmount, const std::string& vertFilepath, const std::string& fragFilepath, std::string identifier)
	{
		if (auto pair = loadedMaterials.find(identifier); pair != loadedMaterials.end()) {
			return pair->second;
		}
		else {
			std::shared_ptr<Material> material = std::make_unique<Material>(device, 
				renderer.getSwapChainRenderPass(), 
				uniformBufferSize, textureAmount,
				*descriptorsManager.globalPool, descriptorsManager.getGlobalDescriptorSetLayout(),
				"shaders/simple_shader.vert.spv",
				"shaders/simple_shader.frag.spv");

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