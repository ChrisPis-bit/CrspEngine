#include "Rendering/FontAtlas.hpp"

#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace crsp {
	FontAtlas::FontAtlas(Device& device, uint32_t fontSize, uint32_t atlasSize, const std::string& fontPath, Texture2D::Filter filter, const char startChar, const char endChar) : size(atlasSize), fontSize(fontSize) {
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			throw std::runtime_error("Could not init FreeType library");
		}

		FT_Face face;
		if (FT_New_Face(ft, (PROJECT_PATH + fontPath).c_str(), 0, &face)) {
			throw std::runtime_error("Failed to load font");
		}

		FT_Set_Pixel_Sizes(face, 0, fontSize);

		yAdvance = face->size->metrics.height / 64;

		std::vector<char> bitmap;
		bitmap.resize(atlasSize * atlasSize, 0);

		// copy glyphs into atlas
		int xOffset = 0;
		int yOffset = 0;
		int curHighestY = 0;

		for (char c = startChar; c <= endChar; ++c)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
			FT_Bitmap& bmp = face->glyph->bitmap;

			// Reset X offset once the atlas bounds are exceeded
			if (xOffset + bmp.width + 1 > atlasSize) {
				yOffset += curHighestY;
				xOffset = 0;
			}

			if (yOffset + bmp.rows + 1 >= atlasSize)
				break;

			if (curHighestY < bmp.rows + 1)
				curHighestY = bmp.rows + 1;

			// Write into bitmap
			for (int row = 0; row < bmp.rows; ++row) {
				for (int col = 0; col < bmp.width; ++col) {
					int atlasIndex = (row + yOffset) * atlasSize + xOffset + col;
					bitmap[atlasIndex] = bmp.buffer[row * bmp.pitch + col];
				}
			}

			// glyp metadata
			GlyphInfo info;
			info.u0 = static_cast<float>(xOffset) / atlasSize;
			info.v0 = static_cast<float>(yOffset) / atlasSize;
			info.u1 = static_cast<float>(xOffset + bmp.width) / atlasSize;
			info.v1 = static_cast<float>(yOffset + bmp.rows) / atlasSize;
			info.width = bmp.width;
			info.height = bmp.rows;
			info.bearingX = face->glyph->bitmap_left;
			info.bearingY = face->glyph->bitmap_top;
			info.advance = face->glyph->advance.x >> 6;

			glyphs[c] = info;

			xOffset += bmp.width + 1;
		}
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		Texture2D::Builder atlasTextureBuilder{};
		atlasTextureBuilder.width = atlasSize;
		atlasTextureBuilder.height = atlasSize;
		atlasTextureBuilder.channels = 1;
		atlasTextureBuilder.format = VK_FORMAT_R8_UNORM;
		atlasTextureBuilder.filter = filter;
		atlasTextureBuilder.data = bitmap.data();
		atlas = std::make_unique<Texture2D>(device, atlasTextureBuilder);
	}

	FontAtlas::~FontAtlas()
	{
	}
}