#include "Rendering/ResourceManager.hpp"

#include <algorithm>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace crsp {
	ResourceManager::ResourceManager(Device& device, Renderer& renderer, GlobalDescriptorsManager& descriptorsManager) : device{ device }, renderer{ renderer }, descriptorsManager{ descriptorsManager }
	{
		// FONT STUFF
		const uint32_t fontSize = 16;
		const uint32_t atlasWidth = 128;
		const uint32_t atlasHeight = 128;

		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			throw std::runtime_error("Could not init FreeType library");
		}

		FT_Face face;
		if (FT_New_Face(ft, (std::string(PROJECT_PATH) + "fonts/Minecraft.ttf").c_str(), 0, &face)) {
			throw std::runtime_error("Failed to load font");
		}

		FT_Set_Pixel_Sizes(face, 0, fontSize); // 0 = width auto, height = fontSize

		const char startChar = 32;  // space
		const char endChar = 126;   // ~
		//const char endChar = 126;   // ~

		//// First pass: measure atlas size
		//int atlasWidth = 0;
		//int atlasHeight = 0;
		//for (char c = startChar; c <= endChar; ++c) {
		//	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
		//	atlasWidth += face->glyph->bitmap.width + 1;
		//	atlasHeight = std::max(atlasHeight, (int)face->glyph->bitmap.rows);
		//}

		FontAtlas atlas{};
		atlas.width = atlasWidth;
		atlas.height = atlasHeight;
		atlas.bitmap.resize(atlasWidth * atlasHeight, 0);

		// copy glyphs into atlas
		int xOffset = 0;
		int yOffset = 0;
		int curHighestY = 0;
		for (char c = startChar; c <= endChar; ++c)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
			FT_Bitmap& bmp = face->glyph->bitmap;

			// Reset X offset once the atlas bounds are exceeded
			if (xOffset + bmp.width + 1 > atlasWidth) {
				yOffset += curHighestY;
				xOffset = 0;
			}

			if (yOffset + bmp.rows + 1 >= atlasHeight)
				break; // Exceeded Y bounds!

			if (curHighestY < bmp.rows + 1)
				curHighestY = bmp.rows + 1;

			// Write into bitmap
			for (int row = 0; row < bmp.rows; ++row) {
				for (int col = 0; col < bmp.width; ++col) {
					int atlasIndex = (row + yOffset) * atlasWidth + xOffset + col;
					atlas.bitmap[atlasIndex] = bmp.buffer[row * bmp.pitch + col];
				}
			}

			// glyp metadata
			GlyphInfo info;
			info.u0 = static_cast<float>(xOffset) / atlasWidth;
			info.v0 = static_cast<float>(yOffset) / atlasHeight;
			info.u1 = static_cast<float>(xOffset + bmp.width) / atlasWidth;
			info.v1 = static_cast<float>(yOffset + bmp.rows) / atlasHeight;
			info.width = bmp.width;
			info.height = bmp.rows;
			info.bearingX = face->glyph->bitmap_left;
			info.bearingY = face->glyph->bitmap_top;
			info.advance = face->glyph->advance.x >> 6;

			atlas.glyphs[c] = info;

			xOffset += bmp.width + 1;
		}
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		Texture2D::Builder atlasTextureBuilder{};
		atlasTextureBuilder.width = atlas.width;
		atlasTextureBuilder.height = atlas.height;
		atlasTextureBuilder.channels = 1;
		atlasTextureBuilder.format = VK_FORMAT_R8_UNORM;
		atlasTextureBuilder.data = atlas.bitmap.data();
		atlasTexture = std::make_shared<Texture2D>( device, atlasTextureBuilder );
	}
	std::shared_ptr<Texture2D> ResourceManager::loadTexture(std::string filePath, std::string identifier)
	{
		if (auto pair = loadedTextures.find(identifier); pair != loadedTextures.end()) {
			return pair->second;
		}
		else {
			std::shared_ptr<Texture2D> texture = Texture2D::createTextureFromFile(device, filePath);
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
	std::shared_ptr<Mesh> ResourceManager::createMesh(Mesh::Builder& builder)
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