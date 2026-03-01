#include "SceneLogic/TextRenderComponent.hpp"
#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/Scene.hpp"

namespace crsp {
	TextRenderComponent::TextRenderComponent(GameObject& owner) : UIRenderComponent(owner)
	{
		Mesh::Builder builder{};
		std::vector<Vertex2D> vertices =
		{
			// Bottom-left
			{ { -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },

			// Bottom-right
			{ {  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

			// Top-right
			{ {  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },

			// Top-left
			{ { -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
		};

		std::vector<uint16_t> indices =
		{
			0, 1, 2,   // First triangle
			2, 3, 0    // Second triangle
		};

		builder.indices = indices;
		builder.copyToVertexBuffer(vertices.data(), vertices.size(), sizeof(Vertex2D));

		mesh = owner.getScene().getResourceManager().createMesh(builder);
	}
}