#include "ECS/Systems/TextRenderSystem.hpp"
#include "Rendering/Mesh.hpp"

#include <array>

namespace crsp {
	void TextRenderSystem::lateUpdate(float deltaTime, float currentTime)
	{
		renderObjects.clear();
		const float baseAspect = 16.0f / 9.0f;
		const float curAspect = window.getAspect();
		const float aspectCorrection = curAspect / baseAspect;

		for (auto const& entity : entities)
		{
			Transform2D* transform = entityManager.getComponent<Transform2D>(entity);
			TextRender* textRender = entityManager.getComponent<TextRender>(entity);
			float aspect = transform->getAspect();

			if (textRender->textUpdated && textRender->text.size() > 0) {
				// Calculate font scale
				const float fontSize = (float)textRender->font->getFontSize() / textRender->fontScale;

				size_t characterCount = textRender->text.size();
				if (characterCount > textRender->maxCharacters)characterCount = textRender->maxCharacters; // Don't exceed max character range

				std::vector<Vertex2D> vertices;
				vertices.resize(textRender->maxCharacters * 4); // allocate for all needed characters

				// Max span for text
				const float maxSpan = (fontSize) * (transform->width * baseAspect);

				// Pre-calculate line spans
				std::vector<int> spans{ 0 };
				size_t currentSpan = 0;
				int currentWordSpan = 0;
				for (size_t i = 0; i < characterCount; i++)
				{
					char character = textRender->text[i];
					GlyphInfo glyph = textRender->font->getGlyphInfo(character);
					int glyphSpan = glyph.bearingX + glyph.width;

					if (character == ' ') {
						// Word end
						spans[currentSpan] += currentWordSpan;
						currentWordSpan = 0;

						// Check if we can add spacing
						// Ignore this character if it doesn't fit
						if (spans[currentSpan] + glyph.advance <= maxSpan) {
							spans[currentSpan] += glyph.advance;
						}
						else {
							currentSpan++;
							spans.push_back(0);
						}
					}
					else if (character == '\n') {
						// Line break
						spans[currentSpan] += currentWordSpan;
						currentWordSpan = 0;

						currentSpan++;
						spans.push_back(0);
					}
					else {
						// Normal letter
						currentWordSpan += glyph.advance;

						// If word exceeds max span, push it to the next line.
						if (spans[currentSpan] + currentWordSpan > maxSpan && spans[currentSpan] != 0) {
							currentSpan++;
							spans.push_back(0);
						}			
					}
				}

				spans.back() += currentWordSpan;

				// Align vertices to glyphs
				// Change pen start depedning on alignment mode
				glm::vec2 pen(textRender->alignment == TextRender::Alignment::CENTER ? (maxSpan - spans[0]) / 2.0f : 0, textRender->font->getYAdvance());

				currentSpan = 0;
				int currentLineSpan = 0; // Current span, seperate from alignment mode
				for (size_t i = 0; i < characterCount; i++)
				{
					char character = textRender->text[i];

					// Skip linebreaks
					if (character == '\n')
						continue;

					GlyphInfo glyph = textRender->font->getGlyphInfo(character);

					// Check if we need to advance to the next line
					bool advancedToNextLine = false;
					if (currentLineSpan + glyph.advance > spans[currentSpan]) {
						advancedToNextLine = true;
						currentSpan++;
						currentLineSpan = 0;

						// Change pen start depedning on alignment mode
						pen.x = textRender->alignment == TextRender::Alignment::CENTER ? (maxSpan - spans[currentSpan]) / 2.0f : 0;
						pen.y += textRender->font->getYAdvance();
					}

					// Top-left position
					glm::vec2 pos = glm::vec2(
						pen.x + glyph.bearingX,
						pen.y - (glyph.height)
					);

					// Place vertices
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

					if (!advancedToNextLine || character != ' ') {
						currentLineSpan += glyph.advance;
						pen.x += glyph.advance;
					}
				}

				// Correct to -1 -> 1 aka screen-space
				for (auto& vertex : vertices)
				{
					vertex.position /= fontSize; // Scale 
					vertex.position.x /= baseAspect * transform->width; // Squash based on the added space from the containers aspect
					vertex.position *= 2; // Correct to screen space
					vertex.position -= 1.0f;
				}

				// Update mesh
				float vertexSize = sizeof(Vertex2D);
				std::vector<uint8_t> rawDataVector;
				rawDataVector.resize(vertices.size() * vertexSize);
				std::memcpy(rawDataVector.data(), vertices.data(), rawDataVector.size());

				textRender->mesh->writeVertices(rawDataVector, vertexSize);

				// Finished updating
				textRender->textUpdated = false;
			}

			// Applies the window aspect ratio to the text.
			glm::vec2 realScale = transform->scale;
			glm::vec2 realPosition = transform->position;
			if (curAspect > baseAspect) {
				realPosition.y += (1 - transform->height);
				realScale.x *= transform->width / aspectCorrection;
			}
			else {
				realScale.x *= transform->width;
				realScale.y *= aspectCorrection;
				realPosition.y += aspectCorrection - (transform->height * aspectCorrection);
			}

			// Render
			UIRenderObject renderObject;
			renderObject.material = textRender->material.get();
			renderObject.mesh = textRender->mesh.get();
			renderObject.transform.position = realPosition;
			renderObject.transform.scale = realScale;
			renderObject.transform.rotation = transform->rotation;
			renderObjects.push_back(renderObject);
		}
	}
}