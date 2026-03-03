#include "ECS/Systems/TextRenderSystem.hpp"
#include "Rendering/Mesh.hpp"

#include <array>

namespace crsp {
	void TextRenderSystem::update(float deltaTime, float currentTime)
	{
		renderObjects.clear();
		//const float baseAspect = 1.0f / 1.0f;
		const float baseAspect = 16.0f / 9.0f;

		for (auto const& entity : entities)
		{
			Transform2D* transform = entityManager.getComponent<Transform2D>(entity);
			TextRender* textRender = entityManager.getComponent<TextRender>(entity);
			float aspect = transform->getAspect();

			if (textRender->textUpdated && textRender->text.size() > 0) {
				// Calculate aspect changes.
				const float fontSize = (float)textRender->font->getFontSize();

				size_t characterCount = textRender->text.size();
				std::vector<Vertex2D> vertices;
				std::vector<uint16_t> indices;
				vertices.resize(characterCount * 4);
				indices.resize(characterCount * 6);

				// Initiate index buffer
				for (size_t i = 0; i < characterCount; i++)
				{
					uint16_t indexBase = i * 6;
					uint16_t vertBase = i * 4;
					indices[indexBase] = vertBase;
					indices[indexBase + 1] = vertBase + 1;
					indices[indexBase + 2] = vertBase + 2;

					indices[indexBase + 3] = vertBase + 2;
					indices[indexBase + 4] = vertBase + 3;
					indices[indexBase + 5] = vertBase;
				}

				// Align vertices to glyphs
				glm::vec2 pen(0.0f, textRender->font->getYAdvance());
				const float maxSpan = fontSize / textRender->fontScale * aspect * baseAspect; // TODO
				for (size_t i = 0; i < characterCount; i++)
				{
					char character = textRender->text[i];
					GlyphInfo glyph = textRender->font->getGlyphInfo(character);

					// Check next line
					if ((pen.x + glyph.bearingX + glyph.width) > maxSpan) {
						pen.x = 0;
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

					pen.x += glyph.advance;
				}

				for (auto& vertex : vertices)
				{
					vertex.position -= maxSpan / 2;
					vertex.position /= maxSpan;
					vertex.position *= 2;
					//
				}




				std::vector<Vertex2D> testvertices =
				{
					// Bottom-left
					{ { -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },

					// Bottom-right
					{ {  1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

					// Top-right
					{ {  1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },

					// Top-left
					{ { -1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
				};

				std::vector<uint16_t> testindices =
				{
					0, 1, 2,   // First triangle
					2, 3, 0    // Second triangle
				};


				// Update mesh
				//float vertexSize = sizeof(Vertex2D);
				//std::vector<uint8_t> rawDataVector;
				//rawDataVector.resize(testvertices.size() * vertexSize);
				//std::memcpy(rawDataVector.data(), testvertices.data(), rawDataVector.size());

				//textRender->mesh->writeVertices(rawDataVector, vertexSize);
				//textRender->mesh->writeIndices(testindices);
				float vertexSize = sizeof(Vertex2D);
				std::vector<uint8_t> rawDataVector;
				rawDataVector.resize(vertices.size() * vertexSize);
				std::memcpy(rawDataVector.data(), vertices.data(), rawDataVector.size());

				textRender->mesh->writeVertices(rawDataVector, vertexSize);
				textRender->mesh->writeIndices(indices);

				textRender->textUpdated = false;
			}

			// Render
			// Applies the aspect ratio to the text.
			glm::vec2 realScale = transform->scale;
			const float curAspect = window.getAspect();
			const float aspectCorrection = curAspect / baseAspect;
			if (curAspect > baseAspect) {
				realScale.y *= transform->height;
				realScale.x *= transform->width / aspectCorrection;
			}
			else {
				realScale.x *= transform->width;
				realScale.y *= transform->height * aspectCorrection;
			}

			UIRenderObject renderObject;
			renderObject.material = textRender->material;
			renderObject.mesh = textRender->mesh;
			renderObject.transform.position = transform->position;
			renderObject.transform.scale = realScale;
			//renderObject.transform.scale = glm::vec2(1.0f, 1.0f);
			renderObject.transform.rotation = transform->rotation;
			renderObjects.push_back(renderObject);
		}
	}
}