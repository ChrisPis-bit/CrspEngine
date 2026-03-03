#pragma once
#include "Texture.hpp"

#include <string>
#include <unordered_map>
#include <memory>

namespace crsp {
	struct GlyphInfo {
		float u0, v0, u1, v1;
		int width, height;
		int bearingX, bearingY;
		int advance;
	};

	class FontAtlas {
	public:
		FontAtlas(Device& device, uint32_t fontSize, uint32_t atlasSize, const std::string& fontPath, const char startChar = 32, const char endChar = 126);
		~FontAtlas();

		GlyphInfo getGlyphInfo(char c) { return glyphs[c]; }
		int getSize() { return size; }
		int getFontSize() { return fontSize; }
		float getYAdvance() { return yAdvance; }

		VkDescriptorImageInfo descriptorInfo() { return atlas->descriptorInfo(); }

	private:
		std::unique_ptr<Texture2D> atlas;
		std::unordered_map<char, GlyphInfo> glyphs;
		int size; // Atlas dimensions (width and height)
		int fontSize; // Atlas dimensions (width and height)

		float yAdvance;
	};
}