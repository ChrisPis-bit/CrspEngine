#include "Boids/BoidsScene.hpp"
#include "Boids/Systems/BoidSystem.hpp"
#include "Utils.hpp"

namespace crsp {
	void BoidsScene::loadResources()
	{
		// Load resources
		resourceManager->loadMesh("models/apple.obj", "apple");
		std::shared_ptr<FontAtlas> font = resourceManager->loadFont("fonts/Minecraft.ttf", "pixelated");

		// Create materials

		std::shared_ptr<Material> environmentMaterial = resourceManager->createMaterial(sizeof(BaseMaterial), 0,
			Material::RenderDomain::Surface3D,
			"shaders/lit_color.vert.spv",
			"shaders/lit_color.frag.spv",
			"boid_material");
		BaseMaterial boidMat{};
		boidMat.color = glm::vec4(0.5, 0.5, 0.5, 1.0);
		environmentMaterial->writeUniform(&boidMat);
		environmentMaterial->build();

		std::shared_ptr<Material> fontMaterial = resourceManager->createMaterial(0, 1,
			Material::RenderDomain::UI,
			"shaders/text.vert.spv",
			"shaders/text.frag.spv",
			"pixelated_font");
		fontMaterial->writeImage(0, font->getTexture());
		fontMaterial->build();
	}

	void BoidsScene::registerSystems()
	{
		meshRenderSystem = entityManager.registerSystem<MeshRenderSystem>();
		textRenderSystem = entityManager.registerSystem<TextRenderSystem>(getWindow());

		entityManager.registerSystem<BoidSystem>(boidSettings);
	}

	void BoidsScene::spawnEntities()
	{
		for (size_t i = 0; i < 1000; i++)
		{
			Entity newBoid = entityManager.createEntity();

			MeshRender* meshRender = entityManager.addComponent<MeshRender>(newBoid);
			Transform* transform = entityManager.addComponent<Transform>(newBoid);
			Boid* boid = entityManager.addComponent<Boid>(newBoid);

			meshRender->mesh = resourceManager->getMesh("apple");
			meshRender->material = resourceManager->getMaterial("boid_material");

			transform->position = glm::vec3(
				randomFloatRange(boidSettings.boundsMin, boidSettings.boundsMax),
				randomFloatRange(boidSettings.boundsMin, boidSettings.boundsMax),
				randomFloatRange(boidSettings.boundsMin, boidSettings.boundsMax)
				);

			boid->velocity = glm::normalize(glm::vec3(
				randomFloatRange(-1.0f, 1.0f),
				randomFloatRange(-1.0f, 1.0f),
				randomFloatRange(-1.0f, 1.0f)
			)) * boidSettings.maxSpeed;
		}
	}

	void BoidsScene::start()
	{
		mainLight.renderShadows = false;
	}

	void BoidsScene::update(float deltaTime, float totalTime)
	{
		std::cout << deltaTime << '\n';
		camera.move(getWindow().getGLFWwindow(), deltaTime);
	}

	void BoidsScene::render()
	{
		renderSurfaces(meshRenderSystem->renderObjects);
		renderUI(textRenderSystem->renderObjects);
	}
}