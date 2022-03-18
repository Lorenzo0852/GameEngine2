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

#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <b2_revolute_joint.h>
#include <b2_circle_shape.h>


using namespace engine;

namespace game
{
	Game::Game(Window& window, Kernel& kernel, std::shared_ptr<EventBus> eventBus)
	{
		b2Vec2 gravity(0.0f, -10.0f);
		world = new b2World(gravity);

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

		//Ground:

		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0.f, -23.0f);
		b2Body* groundBody = world->CreateBody(&groundBodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(50.0f, 10.0f);
		groundBody->CreateFixture(&groundBox, 0.0f);

		//Dynamic:

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0.0f, 4.0f);
		mainBody = world->CreateBody(&bodyDef);

		//We attach a polygon shape to the body...
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(1.0f, 1.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		
		mainBody->CreateFixture(&fixtureDef);

		//Body 2:
		b2BodyDef wheelBodyDef;
		wheelBodyDef.type = b2_dynamicBody;
		b2Vec2 mainBodyPosition = mainBody->GetPosition();
		wheelBodyDef.position.Set(mainBodyPosition.x, mainBodyPosition.y - 1.f);
		wheelBody = world->CreateBody(&wheelBodyDef);

		b2CircleShape wheelShape;
		wheelShape.m_p.Set(0, 0);
		wheelShape.m_radius = 0.5f;

		b2FixtureDef wheelFixtureDef;
		wheelFixtureDef.shape = &wheelShape;
		wheelFixtureDef.density = 1.0f;
		wheelFixtureDef.friction = 0.3f;

		wheelBody->CreateFixture(&wheelFixtureDef);


		//JOINTS

		b2RevoluteJointDef jointDef;
		jointDef.bodyA = mainBody;
		jointDef.bodyB = wheelBody;
		jointDef.localAnchorA = mainBody->GetWorldCenter();
		jointDef.localAnchorB = mainBody->GetWorldCenter();

		b2RevoluteJoint* joint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);

		// ... do stuff ...

		//To destroy joints:
		//NULLIFY to avoid errors...

		/*world->DestroyJoint(joint);
		joint = nullptr;*/

		/*
		*	We start up and add all needed components to the dynamic (moving) entities.
		*/

		player = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cubeModel(new glt::Model);
		cubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		//player.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		player.AddComponent<TransformComponent>(glm::vec3(0, 0, -20.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		player.AddComponent<Node3DComponent>("cube", cubeModel);

		wheel = registry->CreateEntity();
		std::shared_ptr< glt::Model  > wheelCubeModel(new glt::Model);
		wheelCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		//player.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
		wheel.AddComponent<TransformComponent>(glm::vec3(0, 0, -20.f), glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f));
		wheel.AddComponent<Node3DComponent>("wheel_1", wheelCubeModel);

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
		world->Step(1.f/60.f, 6, 2);
		

		Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();

		b2Vec2 position = mainBody->GetPosition();
		float angle = mainBody->GetAngle();
		TransformComponent& playerTransform = player.GetComponent<TransformComponent>();
		playerTransform.position = {position.x, position.y, playerTransform.position.z};
		movement3DSystem.MoveToPosition(player, playerTransform.position);

		b2Vec2 wheelPosition = wheelBody->GetPosition();
		float wheelAngle = wheelBody->GetAngle();

		TransformComponent& wheelTransform = wheel.GetComponent<TransformComponent>();
		wheelTransform.position = { wheelPosition.x, wheelPosition.y, wheelTransform.position.z };
		wheelTransform.rotation = { wheelTransform.rotation.x, wheelTransform.rotation.y, wheelAngle };
		movement3DSystem.MoveToPosition(wheel, wheelTransform.position);

		/*if (playerTransform.position.y > 14)
		{
			auto& transform = player.GetComponent<TransformComponent>();
			transform.position.y = 13.95f;
			registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
			Mix_PlayChannel(-1, sound, 0);
		}*/

		///*
		//*	Restraining player movement...
		//*/
		//Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();
		//TransformComponent& playerTransform = player.GetComponent<TransformComponent>();
		//if (playerTransform.position.y > 14)
		//{
		//	auto& transform = player.GetComponent<TransformComponent>();
		//	transform.position.y = 13.95f;
		//	registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
		//	Mix_PlayChannel(-1, sound, 0);
		//}

		//if (playerTransform.position.y < -14)
		//{
		//	auto& transform = player.GetComponent<TransformComponent>();
		//	transform.position.y = -13.95f;
		//	registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
		//	Mix_PlayChannel(-1, sound, 0);
		//}

		//if (playerTransform.position.x > 35)
		//{
		//	auto& transform = player.GetComponent<TransformComponent>();
		//	transform.position.x = 34.95f;
		//	registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
		//	Mix_PlayChannel(-1, sound, 0);
		//}

		//if (playerTransform.position.x < -35)
		//{
		//	auto& transform = player.GetComponent<TransformComponent>();
		//	transform.position.x = -34.95f;
		//	registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
		//	Mix_PlayChannel(-1, sound, 0);
		//}
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
			cubeRigidbody.velocity = glm::vec3(-8 * event.value, cubeRigidbody.velocity.y, cubeRigidbody.velocity.z);
			//cubeRigidbody.angularVelocity = glm::vec3(0, 0, 2 * event.value);
			break;
		case InputEvent::Action::RIGHT:
			cubeRigidbody.velocity = glm::vec3(8 * event.value, cubeRigidbody.velocity.y, cubeRigidbody.velocity.z);
			//cubeRigidbody.angularVelocity = glm::vec3(0, 0, -2 * event.value);
			break;
		}

		mainBody->SetLinearVelocity({ cubeRigidbody.velocity.x, cubeRigidbody.velocity.y });
	}
}