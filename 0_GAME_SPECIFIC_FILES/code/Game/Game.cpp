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
#include <Systems/PhysicsSystem.h>

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <b2_revolute_joint.h>
#include <b2_circle_shape.h>


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

		//window.SetWindowedFullscreen();
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
		eventBus->AddEventListener<OnTriggerEntryEvent>(this, &Game::OnTriggerEntry);

		/*
		*	Deserializes and spawns all static objects - not used for now...
		*/
		/*Scene3DDeserializer deserializer("../../../assets/scenes/test.scene", registry.get(), window);
		deserializer.Initialize();*/

		registry->AddSystem<Movement3DSystem>();
		registry->AddSystem<ModelRender3DSystem>(*window);
		registry->AddSystem<EntityStartup3DSystem>();
		registry->AddSystem<PhysicsSystem>(eventBus);
		registry->AddSystem<Physics3DSystem>();

		/*
		*	We load up the .wav sounds we want to use in this demo.
		*/
		/*sound = Mix_LoadWAV("../../../assets/sounds/hit.wav");
		if (sound == NULL)
		{
			spdlog::error("Couldn't load .wav from the wanted path");
		}
		death = Mix_LoadWAV("../../../assets/sounds/death.wav");
		Mix_Volume(-1, 60);*/
		
		// ... do stuff ...

		//To destroy joints:
		//NULLIFY to avoid errors...

		/*world->DestroyJoint(joint);
		joint = nullptr;*/

		/*
		*	We start up and add all needed components to the dynamic (moving) entities.
		*/

		m_speed = 30.0f;

		car_base = registry->CreateEntity();
		std::shared_ptr< glt::Model  > carBaseCubeModel(new glt::Model);
		carBaseCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		car_base.AddComponent<TransformComponent>(glm::vec3(-90.f, 60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(7.f, 0.5f, 0.5f));
		car_base.AddComponent<RigidbodyComponent>();
		car_base.AddComponent<Node3DComponent>("car_base", carBaseCubeModel);
		car_base.AddComponent<BoxColliderComponent>(false, 4.f, 0.5f, 1.0f, 0.9f);

		Entity test = registry->CreateEntity();
		std::shared_ptr<glt::Model> testModel(new glt::Model);
		testModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		test.AddComponent<TransformComponent>(glm::vec3(-90.f, 60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(3, 3, 3));
		test.AddComponent<BoxCollider3DComponent>(glm::vec3(3,3,3), 10);
		test.AddComponent<Rigidbody3DComponent>();
		test.AddComponent<Node3DComponent>("test", testModel);
		

		/* Lights, cam, FX, etc */

		Entity light = registry->CreateEntity();
		std::shared_ptr< glt::Light  > lightNode(new glt::Light);
		lightNode->set_color(glm::vec3(0.4f, 0.7f, 0.929f));
		light.AddComponent<TransformComponent>(glm::vec3(10.f, 10.f, -30.f), glm::vec3(0.f, 0.f, 0.f));
		light.AddComponent<Node3DComponent>("light", lightNode);


		cam = registry->CreateEntity();
		std::shared_ptr< glt::Camera > cameraNode(new glt::Camera(20.f, 1.f, 500.f, 1.f));

		cam.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 100.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		cam.AddComponent<Node3DComponent>("camera", cameraNode);


		/*
		*	Telling the kernel what tasks does it need to initialize and what tasks does it have to keep running in loop.
		*/

		//Update registry to process the entities that are waiting
		kernel->InitializeTask(*registry);
		kernel->InitializeTask(registry->GetSystem<ModelRender3DSystem>());
		kernel->InitializeTask(registry->GetSystem<EntityStartup3DSystem>());
		kernel->InitializeTask(registry->GetSystem<PhysicsSystem>());
		kernel->InitializeTask(registry->GetSystem<Physics3DSystem>());

		//Physics need to be calculated before any rendering is done...
		kernel->AddPriorizedRunningTask(registry->GetSystem<Physics3DSystem>());
		kernel->AddPriorizedRunningTask(registry->GetSystem<PhysicsSystem>());

		kernel->AddRunningTask(registry->GetSystem<Movement3DSystem>());
		kernel->AddRunningTask(registry->GetSystem<ModelRender3DSystem>());
	}

	bool Game::Initialize()
	{
		return true;
	}

	/*
	*	Runs every frame. Equivalent to similar functions in other engines like Update().
	*/
	void Game::Run(float deltaTime)
	{

		//Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();


		//Cam need Rigidbody to move... Enable if wanted.
		//cam.GetComponent<TransformComponent>().SetTransformation(glm::vec3(m_car->GetPosition().x, 0.f, 100.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		
		/*if (playerTransform.position.y > 14)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.y = 13.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}*/

	}

	void Game::OnInputRegistered(InputEvent& event)
	{
		switch (event.action)
		{
		case InputEvent::Action::QUIT:
			kernel->Stop();
			break;
		}
	}

	void Game::OnTriggerEntry(OnTriggerEntryEvent& event)
	{
		
	}
}