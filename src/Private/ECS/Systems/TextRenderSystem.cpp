#include "ECS/Systems/TextRenderSystem.hpp"
#include "Rendering/Mesh.hpp"

#include <array>

namespace crsp {
	void TextRenderSystem::update(float deltaTime, float currentTime)
	{
		renderObjects.clear();
		//const float baseAspect = 1.0f / 1.0f;
		const float baseAspect = 16.0f / 9.0f;
		const float curAspect = window.getAspect();
		const float aspectCorrection = curAspect / baseAspect;

		for (auto const& entity : entities)
		{
			Transform2D* transform = entityManager.getComponent<Transform2D>(entity);
			TextRender* textRender = entityManager.getComponent<TextRender>(entity);
			float aspect = transform->getAspect();

			if ((textRender->textUpdated || window.wasResized()) && textRender->text.size() > 0) {
				// Calculate aspect changes.
				const float fontSize = (float)textRender->font->getFontSize() / textRender->fontScale;

				size_t characterCount = textRender->text.size();
				if (characterCount > textRender->maxCharacters)characterCount = textRender->maxCharacters; // Don't exceed max character range

				std::vector<Vertex2D> vertices;
				vertices.resize(textRender->maxCharacters * 4); // allocate for all needed characters

				// Max span for text
				const float maxSpan = (fontSize) * (transform->width * baseAspect); // TODO

				// Calculate line spans
				std::vector<int> spans{ 0 };
				size_t currentSpan = 0;
				for (size_t i = 0; i < characterCount; i++)
				{
					char character = textRender->text[i];
					GlyphInfo glyph = textRender->font->getGlyphInfo(character);
					int glyphSpan = glyph.bearingX + glyph.width;

					if (spans[currentSpan] == 0 || spans[currentSpan] + glyphSpan <= maxSpan) {
						spans[currentSpan] += glyph.advance;
					}
					else {
						currentSpan++;
						spans.push_back(glyphSpan);
					}
				}

				// Align vertices to glyphs
				// Change pen start depedning on alignment mode
				glm::vec2 pen(textRender->alignment == TextRender::Alignment::CENTER ? (maxSpan - spans[0]) / 2.0f : 0, textRender->font->getYAdvance());

				currentSpan = 0;
				float currentLineSpan = 0;
				for (size_t i = 0; i < characterCount; i++)
				{
					char character = textRender->text[i];
					GlyphInfo glyph = textRender->font->getGlyphInfo(character);

					// Check next line
					if ((currentLineSpan + glyph.bearingX + glyph.width) > spans[currentSpan]) {
						currentSpan++;

						// Change pen start depedning on alignment mode
						pen.x = textRender->alignment == TextRender::Alignment::CENTER ? (maxSpan - spans[currentSpan]) / 2.0f : 0;

						currentLineSpan = 0;
						pen.y += textRender->font->getYAdvance();
					}

					// Top-left position
					glm::vec2 pos = glm::vec2(
						pen.x + glyph.bearingX,
						pen.y - (glyph.height)
					);

					int base = i * 4;
					Vertex2D& vertBL = vertices[base];// Bottom-left
					Vertex2D& vertBR = vertices[base + 1]; // Bottom-right
					Vertex2D& vertTR = vertices[base + 2]; // Top-right
					Vertex2D& vertTL = vertices[base + 3]; // Top-left

					vertBL.position = glm::vec2(pos.x, pos.y);
					vertBL.color = glm::vec3(1);
					vertBL.texCoord = glm::vec2(glyph.u0, glyph.v0);

					vertBR.position = glm::vec2(pos.x + glyph.width, pos.y);
					vertBR.color = glm::vec3(1);
					vertBR.texCoord = glm::vec2(glyph.u1, glyph.v0);

					vertTR.position = glm::vec2(pos.x + glyph.width, pos.y + glyph.height);
					vertTR.color = glm::vec3(1);
					vertTR.texCoord = glm::vec2(glyph.u1, glyph.v1);

					vertTL.position = glm::vec2(pos.x, pos.y + glyph.height);
					vertTL.color = glm::vec3(1);
					vertTL.texCoord = glm::vec2(glyph.u0, glyph.v1);

					currentLineSpan += glyph.advance;
					pen.x += glyph.advance;
				}

				// Correct to -1 -> 1 space
				for (auto& vertex : vertices)
				{
					vertex.position /= fontSize;
					vertex.position.x /= baseAspect * transform->width;
					vertex.position *= 2;
					vertex.position -= 1.0f;
				}

				// Update mesh
				float vertexSize = sizeof(Vertex2D);
				std::vector<uint8_t> rawDataVector;
				rawDataVector.resize(vertices.size() * vertexSize);
				std::memcpy(rawDataVector.data(), vertices.data(), rawDataVector.size());

				textRender->mesh->writeVertices(rawDataVector, vertexSize);

				textRender->textUpdated = false;
			}

			// Render
			// Applies the aspect ratio to the text.
			glm::vec2 realScale = transform->scale;
			glm::vec2 realPosition = transform->position;
			if (curAspect > baseAspect) {
				//realScale.y *= transform->height;
				realPosition.y += (1 - transform->height);
				realScale.x *= transform->width / aspectCorrection;
			}
			else {
				realScale.x *= transform->width;
				realScale.y *= aspectCorrection;
				realPosition.y += aspectCorrection - (transform->height * aspectCorrection);
			}

			UIRenderObject renderObject;
			renderObject.material = textRender->material;
			renderObject.mesh = textRender->mesh;
			renderObject.transform.position = realPosition;
			renderObject.transform.scale = realScale;
			//renderObject.transform.scale = glm::vec2(1.0f, 1.0f);
			renderObject.transform.rotation = transform->rotation;
			renderObjects.push_back(renderObject);
		}
	}
}