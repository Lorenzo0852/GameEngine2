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

		TransformComponent & carbaseTransform = car_base.GetComponent<TransformComponent>();
		wheel_1 = registry->CreateEntity();
		std::shared_ptr< glt::Model  > wheelCubeModel(new glt::Model);
		wheelCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel_1.AddComponent<TransformComponent>(
			glm::vec3(carbaseTransform.position.x -5,
			carbaseTransform.position.y -1, 0.f),
			glm::vec3(0, 0, 0), glm::vec3(1,1,1));
		wheel_1.AddComponent<RigidbodyComponent>();
		wheel_1.AddComponent<Node3DComponent>("wheel_1", wheelCubeModel);
		wheel_1.AddComponent<CircleColliderComponent>(false);

		wheel_2 = registry->CreateEntity();
		std::shared_ptr< glt::Model  > cubeModel(new glt::Model);
		cubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel_2.AddComponent<TransformComponent>(
			glm::vec3(carbaseTransform.position.x + 5,
				carbaseTransform.position.y + -1, 0.f),
			glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		wheel_2.AddComponent<RigidbodyComponent>();
		wheel_2.AddComponent<Node3DComponent>("wheel_2", cubeModel);
		wheel_2.AddComponent<CircleColliderComponent>(false);

		//car container
		car_container[0] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > carContainerBottomCubeModel(new glt::Model);
		carContainerBottomCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		car_container[0].AddComponent<TransformComponent>(glm::vec3(-90.f, 61.5f, 0.f), glm::vec3(0, 0, 0), glm::vec3(8.f, 0.5f, 0.5f));
		car_container[0].AddComponent<RigidbodyComponent>();
		car_container[0].AddComponent<Node3DComponent>("car_container_bot", carContainerBottomCubeModel);
		car_container[0].AddComponent<BoxColliderComponent>(false, 8.f, 0.5f, 0.2f, 0.9f);

		car_container[1] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > carContainerLeftCubeModel(new glt::Model);
		carContainerLeftCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		car_container[1].AddComponent<TransformComponent>(glm::vec3(-97.f, 63.5f, 0.f), glm::vec3(0, 0, 0), glm::vec3(.5f, 2.f, 1.f));
		car_container[1].AddComponent<RigidbodyComponent>();
		car_container[1].AddComponent<Node3DComponent>("car_container_left", carContainerLeftCubeModel);
		car_container[1].AddComponent<BoxColliderComponent>(false, .5f, 2.f, 0.2f, 0.9f);

		car_container[2] = registry->CreateEntity();
		std::shared_ptr< glt::Model  > carContainerRightCubeModel(new glt::Model);
		carContainerRightCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		car_container[2].AddComponent<TransformComponent>(glm::vec3(-83.f, 63.5f, 0.f), glm::vec3(0, 0, 0), glm::vec3(.5f, 2.f, 1.f));
		car_container[2].AddComponent<RigidbodyComponent>();
		car_container[2].AddComponent<Node3DComponent>("car_container_right", carContainerRightCubeModel);
		car_container[2].AddComponent<BoxColliderComponent>(false, .5f, 2.f, 0.2f, 0.9f);


		Entity ground = registry->CreateEntity();
		std::shared_ptr< glt::Model  > groundCubeModel(new glt::Model);
		groundCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		ground.AddComponent<TransformComponent>(glm::vec3(-100.f, 50.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(20.f, 1, 1));
		ground.AddComponent<RigidbodyComponent>(staticBody);
		ground.AddComponent<Node3DComponent>("ground", groundCubeModel);
		ground.AddComponent<BoxColliderComponent>(false, 20.f, 1);

		Entity down_ramp = registry->CreateEntity();
		std::shared_ptr< glt::Model  > downRampCubeModel(new glt::Model);
		downRampCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		down_ramp.AddComponent<TransformComponent>(glm::vec3(-52.f, 21.f, 0.f), glm::vec3(0, 0, -0.8f), glm::vec3(40.f, 1, 1));
		down_ramp.AddComponent<RigidbodyComponent>(staticBody);
		down_ramp.AddComponent<Node3DComponent>("down_ramp", downRampCubeModel);
		down_ramp.AddComponent<BoxColliderComponent>(false, 40.f, 1);

		Entity down_ground = registry->CreateEntity();
		std::shared_ptr< glt::Model  > downgroundCubeModel(new glt::Model);
		downgroundCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		down_ground.AddComponent<TransformComponent>(glm::vec3(-20.f, -8.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(5.f, 1, 1));
		down_ground.AddComponent<RigidbodyComponent>(staticBody);
		down_ground.AddComponent<Node3DComponent>("down_ground", downgroundCubeModel);
		down_ground.AddComponent<BoxColliderComponent>(false, 5.f, 1);

		Entity up_ramp = registry->CreateEntity();
		std::shared_ptr< glt::Model  > upRampCubeModel(new glt::Model);
		upRampCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		up_ramp.AddComponent<TransformComponent>(glm::vec3(-11.f, -6.3f, 0.f), glm::vec3(0, 0, 0.3f), glm::vec3(5.f, 1, 1));
		up_ramp.AddComponent<RigidbodyComponent>(staticBody);
		up_ramp.AddComponent<Node3DComponent>("up_ramp", upRampCubeModel);
		up_ramp.AddComponent<BoxColliderComponent>(false, 5.f, 1);

		Entity right_ground = registry->CreateEntity();
		std::shared_ptr< glt::Model  > rightGroundCubeModel(new glt::Model);
		rightGroundCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		right_ground.AddComponent<TransformComponent>(glm::vec3(60.f, -10.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(30.f, 1, 1));
		right_ground.AddComponent<RigidbodyComponent>(staticBody);
		right_ground.AddComponent<Node3DComponent>("right_ground", rightGroundCubeModel);
		right_ground.AddComponent<BoxColliderComponent>(false, 30.f, 1, 1, 0.1f);

		Entity finish_wall = registry->CreateEntity();
		std::shared_ptr< glt::Model  > finishWallCubeModel(new glt::Model);
		finishWallCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		finish_wall.AddComponent<TransformComponent>(glm::vec3(90.f, -10.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(1.f, -10.f, 1));
		finish_wall.AddComponent<RigidbodyComponent>(staticBody);
		finish_wall.AddComponent<Node3DComponent>("finish_wall", finishWallCubeModel);
		finish_wall.AddComponent<BoxColliderComponent>(false, 0.f, 10.f);

		right_trigger = registry->CreateEntity();
		right_trigger.AddComponent<TransformComponent>(glm::vec3(80.f, -5.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(8.f, 4.f, 1.f));
		right_trigger.AddComponent<RigidbodyComponent>(staticBody);
		right_trigger.AddComponent<BoxColliderComponent>(true, 8.f, 4.f);

		platform = registry->CreateEntity();
		std::shared_ptr< glt::Model  > platformCubeModel(new glt::Model);
		platformCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		platform.AddComponent<TransformComponent>(glm::vec3(12.f, -75.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(15.f, 1, 1));
		platform.AddComponent<RigidbodyComponent>();
		platform.AddComponent<Node3DComponent>("platform", platformCubeModel);
		platform.AddComponent<BoxColliderComponent>(false, 15.f, 1);

		platform_trigger = registry->CreateEntity();
		std::shared_ptr< glt::Model  > platformTriggerModel(new glt::Model);
		platformTriggerModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		platform_trigger.AddComponent<TransformComponent>(glm::vec3(12.f, -70.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(12.f, 4, 1));
		platform_trigger.AddComponent<RigidbodyComponent>();
		platform_trigger.AddComponent<BoxColliderComponent>(true, 12.f, 4.f, 0.01f);

		prismatic = registry->CreateEntity();
		std::shared_ptr< glt::Model  > prismaticCubeModel(new glt::Model);
		prismaticCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		prismatic.AddComponent<TransformComponent>(glm::vec3(12.f, -90.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(1.f, 15.f, 1));
		prismatic.AddComponent<RigidbodyComponent>();
		prismatic.AddComponent<Node3DComponent>("prismatic", prismaticCubeModel);
		prismatic.AddComponent<BoxColliderComponent>(false, 12.f, 1);
		
		container_left = registry->CreateEntity();
		std::shared_ptr< glt::Model  > containerLeftCubeModel(new glt::Model);
		containerLeftCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		container_left.AddComponent<TransformComponent>(glm::vec3(70.f, 10.f, 0.f), glm::vec3(0, 0, 3.14 / 4), glm::vec3(6.f, 1, 1));
		container_left.AddComponent<RigidbodyComponent>();
		container_left.AddComponent<Node3DComponent>("container_left", containerLeftCubeModel);
		container_left.AddComponent<BoxColliderComponent>(false, 6.f, 1);

		container_right = registry->CreateEntity();
		std::shared_ptr< glt::Model  > containerRightCubeModel(new glt::Model);
		containerRightCubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		container_right.AddComponent<TransformComponent>(glm::vec3(63.f, 10.f, 0.f), glm::vec3(0, 0, - 3.14 / 4), glm::vec3(6.f, 1, 1));
		container_right.AddComponent<RigidbodyComponent>();
		container_right.AddComponent<Node3DComponent>("container_right", containerRightCubeModel);
		container_right.AddComponent<BoxColliderComponent>(false, 6.f, 1);

		container_base = registry->CreateEntity();
		container_base.AddComponent<TransformComponent>(glm::vec3(90.f, 11.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(1.f, 1.f, 1));
		container_base.AddComponent<RigidbodyComponent>(staticBody);
		container_base.AddComponent<BoxColliderComponent>(true, 1, 1.f);

		for (int i = 0; i < 10; i++)
		{
			Entity entity = registry->CreateEntity();
			std::shared_ptr< glt::Model  > model(new glt::Model);
			model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
			entity.AddComponent<TransformComponent>
				(glm::vec3(90.f, 14.f, 0.f),
					glm::vec3(0, 0, -3.14 / 4),
					glm::vec3(.7f, .7f, 1));
			entity.AddComponent<RigidbodyComponent>();
			entity.AddComponent<Node3DComponent>("cargo" + std::to_string(i), model);
			entity.AddComponent<CircleColliderComponent>(false, .7f);
		}

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

		//Physics need to be calculated before any rendering is done...
		kernel->AddPriorizedRunningTask(registry->GetSystem<PhysicsSystem>());

		kernel->AddRunningTask(registry->GetSystem<Movement3DSystem>());
		kernel->AddRunningTask(registry->GetSystem<ModelRender3DSystem>());
	}

	bool Game::Initialize()
	{
		PhysicsSystem& physicsSystem = registry->GetSystem<PhysicsSystem>();

		back_wheel_motor  = physicsSystem.MotorizeAsWheel(
				&wheel_1,
				&car_base,
				1500.f,
				{0, -1}
		);

		front_wheel_motor = physicsSystem.MotorizeAsWheel(
				&wheel_2,
				&car_base,
				1500.f,
				{0, -1}
		);
		
		prismaticJoint = physicsSystem.MotorizedPrismaticJoint(
				&prismatic,
				&right_trigger,
				1500.f,
				-10.f,
				+65.f,
				{ 0, 1 }
			);

		physicsSystem.Weld(
				&platform,
				&prismatic
			);

		physicsSystem.Weld(
				&platform_trigger,
				&platform
			);

		car_container_motor = physicsSystem.Motorize(
				&car_container[0],
				&car_base,
				10000,
				{ -8.f, 1.f }
		);

		physicsSystem.Weld(
			&container_left,
			&container_right
		);

		container_motor = physicsSystem.Motorize(
		
			&container_left,
			&container_base,
			10000,
			{0,0},
				0,
				3.1415
		);

		//welding the sides of the vehicle container
		for (int i = 1; i < 3; i++)
		{
			physicsSystem.Weld(
					&car_container[i],
					&car_container[0]
				);
		}

		return true;
	}

	/*
	*	Runs every frame. Equivalent to similar functions in other engines like Update().
	*/
	void Game::Run(float deltaTime)
	{

		Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();

		b2Vec2 position = wheel_2.GetComponent<RigidbodyComponent>().body->GetPosition();
		float angle = wheel_2.GetComponent<RigidbodyComponent>().body->GetAngle();
		TransformComponent& playerTransform = wheel_2.GetComponent<TransformComponent>();

		b2Vec2 wheelPosition = wheel_1.GetComponent<RigidbodyComponent>().body->GetPosition();
		float wheelAngle = wheel_1.GetComponent<RigidbodyComponent>().body->GetAngle();
		TransformComponent& wheelTransform = wheel_1.GetComponent<TransformComponent>();

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
		RigidbodyComponent& cubeRigidbody = wheel_2.GetComponent<RigidbodyComponent>();
		switch (event.action)
		{
		case InputEvent::Action::QUIT:
			kernel->Stop();
			break;
		case InputEvent::Action::FORWARD:
			/*m_car_container_joint_motor->SetMotorSpeed(20.0f);*/
			break;
		case InputEvent::Action::BACKWARDS:
			if (event.value == 1)
			{
				back_wheel_motor.EnableMotor(true);
				front_wheel_motor.EnableMotor(true);
				back_wheel_motor.SetMotorSpeed(-1 * back_wheel_motor.GetMotorSpeed());
				front_wheel_motor.SetMotorSpeed(-1 * front_wheel_motor.GetMotorSpeed());
				if (back_wheel_motor.GetMotorSpeed() < 1.f)
				{
					back_wheel_motor.SetMotorSpeed(0);
					front_wheel_motor.SetMotorSpeed(0);
				}
			}
			else
			{
				back_wheel_motor.EnableMotor(false);
				front_wheel_motor.EnableMotor(false);
			}
			break;
		case InputEvent::Action::LEFT:
			if (event.value == 1)
			{
				back_wheel_motor.EnableMotor(true);
				front_wheel_motor.EnableMotor(true);
				back_wheel_motor.SetMotorSpeed(m_speed);
				front_wheel_motor.SetMotorSpeed(m_speed);
			}
			else
			{
				back_wheel_motor.EnableMotor(false);
				front_wheel_motor.EnableMotor(false);
			}
			break;
		case InputEvent::Action::RIGHT:
			if (event.value == 1)
			{
				back_wheel_motor.EnableMotor(true);
				front_wheel_motor.EnableMotor(true);
				back_wheel_motor.SetMotorSpeed(-m_speed);
				front_wheel_motor.SetMotorSpeed(-m_speed);
			}
			else
			{
				back_wheel_motor.EnableMotor(false);
				front_wheel_motor.EnableMotor(false);
			}
			break;
		case InputEvent::Action::Q:
			if (event.value == 1)
			{
				car_container_motor.SetMotorSpeed(0.2f);
			}
			else
			{
				car_container_motor.SetMotorSpeed(0);
			}
			break;
		case InputEvent::Action::E:
			if (event.value == 1)
			{
				car_container_motor.SetMotorSpeed(-0.2f);
			}
			else
			{
				car_container_motor.SetMotorSpeed(0);
			}
			break;
		}
	}

	void Game::OnTriggerEntry(OnTriggerEntryEvent& event)
	{
		if (event.triggerFixture->GetBody() == right_trigger.GetComponent<RigidbodyComponent>().body
			&&
			event.otherFixture->GetBody() == wheel_2.GetComponent<RigidbodyComponent>().body)
		{
			prismaticJoint.SetMotorSpeed(15.f);
			container_motor.SetMotorSpeed(15.f);
		}

		if (event.triggerFixture->GetBody() == platform_trigger.GetComponent<RigidbodyComponent>().body)
		{
			prismaticJoint.SetLimits(0.f, 120.f);
			prismaticJoint.SetMotorSpeed(5.f);
		}
	}
}