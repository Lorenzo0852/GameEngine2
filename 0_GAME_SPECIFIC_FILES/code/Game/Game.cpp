/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <gltk/Mesh.hpp>
#include <gltk/Cube.hpp>
#include <gltk/Model.hpp>
#include <gltk/Model_Obj.hpp>
#include <gltk/Node.hpp>
#include <gltk/Light.hpp>
#include <gltk/Render_Node.hpp>
#include <gltk/Drawable.hpp>
#include <gltk/Material.hpp>
#include <gltk/Camera.hpp>

#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <sdl2/SDL_mixer.h>
#include <glm/glm.hpp>
#include "Game.h"

#include <Deserializer/Scene3DDeserializer.h>

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>
#include <Components/Node3DComponent.h>

#include <Systems/Movement3DSystem.h>
#include <Systems/ModelRender3DSystem.h>
#include <Systems/EntityStartup3DSystem.h>

using namespace engine;

namespace game
{
	Game::Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus)
	{
		registry = std::make_unique<Registry>();
		assetManager = std::make_unique<AssetManager>();
		this->eventBus = eventBus;

		this->kernel = &kernel;
		this->window = &window;

		window.SetWindowedFullscreen();
		window.SetVsync(true);

		spdlog::info("Game constructor called");
	}


	void Game::SetupScene()
	{
		/*
		*	We tell the eventBus (event manager for all entities) that we are listening for all events of the type InputEvent.
		*	eventBus will call the function we pass as parameter whenever the event is fired.
		*/
		eventBus->AddEventListener<InputEvent>(this, &Game::OnInputRegistered);
		/*
		*	Deserializes and spawns all static objects - In this demo, the four walls.
		*/
		Scene3DDeserializer deserializer("../../../assets/scenes/test.scene", registry.get(), window);
		deserializer.Initialize();

		/*
		*	We load up the .wav sounds we want to use in this demo.
		*/
		sound = Mix_LoadWAV("../../../assets/sounds/hit.wav");
		if (sound == NULL)
		{
			spdlog::error("Couldn't load .wav from the wanted path");
		}
		death = Mix_LoadWAV("../../../assets/sounds/death.wav");
		Mix_Volume(-1, 60);

		/*
		*	We start up and add all needed components to the dynamic (moving) entities.
		*/

		player = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cubeModel(new glt::Model);
		cubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		player.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		player.AddComponent<TransformComponent>(glm::vec3(0, 0, -20.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		player.AddComponent<Node3DComponent>("cube", cubeModel);

		Entity rightArm = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cube2Model(new glt::Model);
		cube2Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		rightArm.AddComponent<TransformComponent>(glm::vec3(1, 1, 0.f), glm::vec3(0, 0, 0), glm::vec3(0.2f, 1, 0.2f), &player);
		rightArm.AddComponent<Node3DComponent>("rightArm", cube2Model);

		Entity leftArm = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cube3Model(new glt::Model);
		cube3Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		leftArm.AddComponent<TransformComponent>(glm::vec3(-1, 1, 0.f), glm::vec3(0, 0, 0), glm::vec3(0.2f, 1, 0.2f), &player);
		leftArm.AddComponent<Node3DComponent>("leftArm", cube3Model);

		Entity head = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cube4Model(new glt::Model);
		cube4Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		head.AddComponent<TransformComponent>(glm::vec3(0, 0.5f, 1.f), glm::vec3(0, 0, 0), glm::vec3(0.7f, 0.7f, 0.7f), &player);
		head.AddComponent<Node3DComponent>("head", cube4Model);

		enemies[0] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > enemyTopRightModel(new glt::Model);
		enemyTopRightModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		enemies[0].AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		enemies[0].AddComponent<TransformComponent>(glm::vec3(36, 14, -20.f), glm::vec3(0, 0, 0), glm::vec3(.4f, .4f, .4f));
		enemies[0].AddComponent<Node3DComponent>("enemyTopRight", enemyTopRightModel);

		enemies[1] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > enemyTopLeftModel(new glt::Model);
		enemyTopLeftModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		enemies[1].AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		enemies[1].AddComponent<TransformComponent>(glm::vec3(-36, 14, -20.f), glm::vec3(0, 0, 0), glm::vec3(.4f, .4f, .4f));
		enemies[1].AddComponent<Node3DComponent>("enemyTopLeft", enemyTopLeftModel);

		enemies[2] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > enemyBotRightModel(new glt::Model);
		enemyBotRightModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		enemies[2].AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		enemies[2].AddComponent<TransformComponent>(glm::vec3(36, -14, -20.f), glm::vec3(0, 0, 0), glm::vec3(.4f, .4f, .4f));
		enemies[2].AddComponent<TransformComponent>(glm::vec3(36, -14, -20.f), glm::vec3(0, 0, 0), glm::vec3(.4f, .4f, .4f));
		enemies[2].AddComponent<Node3DComponent>("enemyBotRight", enemyBotRightModel);

		enemies[3] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > enemyBotLeftModel(new glt::Model);
		enemyBotLeftModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		enemies[3].AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		enemies[3].AddComponent<TransformComponent>(glm::vec3(-36, -14, -20.f), glm::vec3(0, 0, 0), glm::vec3(.4f, .4f, .4f));
		enemies[3].AddComponent<Node3DComponent>("enemyBotLeft", enemyBotLeftModel);

		Entity light = registry->CreateEntity();
		std::shared_ptr< glt::Light  > lightNode(new glt::Light);
		lightNode->set_color(glm::vec3(0.4f, 0.7f, 0.929f));
		light.AddComponent<TransformComponent>(glm::vec3(10.f, 10.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
		light.AddComponent<Node3DComponent>("light", lightNode);

		Entity cam = registry->CreateEntity();
		std::shared_ptr< glt::Camera > cameraNode(new glt::Camera(20.f, 1.f, 500.f, 1.f));

		cam.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		cam.AddComponent<Node3DComponent>("camera", cameraNode);


		/*
		*	Telling the kernel what tasks does it need to initialize and what tasks does it have to keep running in loop.
		*/
		//Update registry to process the entities that are waiting
		kernel->InitializeTask(*registry);
		kernel->InitializeTask(registry->GetSystem<ModelRender3DSystem>());
		kernel->InitializeTask(registry->GetSystem<EntityStartup3DSystem>());
		kernel->AddRunningTask(registry->GetSystem<Movement3DSystem>());
		kernel->AddRunningTask(registry->GetSystem<ModelRender3DSystem>());
	}

	/*
	*	Runs every frame. Equivalent to similar functions in other engines like Update().
	*/
	void Game::Run(float deltaTime)
	{
		/*
		*	Restraining player movement...
		*/
		Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();
		TransformComponent& playerTransform = player.GetComponent<TransformComponent>();
		if (playerTransform.position.y > 14)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.y = 13.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}

		if (playerTransform.position.y < -14)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.y = -13.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}

		if (playerTransform.position.x > 35)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.x = 34.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}

		if (playerTransform.position.x < -35)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.x = -34.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}

		/*
		*	Creating enemy movement...
		*/

		for (auto entity : enemies)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			glt::Node* enemyNode = entity.GetComponent<Node3DComponent>().node.get();
			glm::vec3 directionVector = movement3DSystem.MoveTowards(entity, playerTransform.position);
			enemyNode->translate(directionVector * 14.f * deltaTime);

			/*
			*	Detecting distances between player and enemies to use as a pseudo - collision system.
			*/
			if (movement3DSystem.Distance(playerTransform.position, transform.position) < 1.f)
			{
				Mix_PlayChannel(-1, death, 0);
				movement3DSystem.ResetTransform(player);
				for (auto e : enemies)
				{
					movement3DSystem.ResetTransform(e);
				}
			}
		}


	}

	void Game::OnInputRegistered(InputEvent& event)
	{
		RigidbodyComponent& cubeRigidbody = player.GetComponent<RigidbodyComponent>();
		switch (event.action)
		{
		case InputEvent::Action::QUIT:
			kernel->Stop();
			break;
		case InputEvent::Action::FORWARD:
			cubeRigidbody.velocity = glm::vec3(cubeRigidbody.velocity.x, 8 * event.value, cubeRigidbody.velocity.z);
			break;
		case InputEvent::Action::BACKWARDS:
			cubeRigidbody.velocity = glm::vec3(cubeRigidbody.velocity.x, -8 * event.value, cubeRigidbody.velocity.z);
			break;
		case InputEvent::Action::LEFT:
			cubeRigidbody.angularVelocity = glm::vec3(0, 0, 2 * event.value);
			break;
		case InputEvent::Action::RIGHT:
			cubeRigidbody.angularVelocity = glm::vec3(0, 0, -2 * event.value);
			break;
		}
	}
}