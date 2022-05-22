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
#include <Components/SphereCollider3DComponent.h>
#include <Components/RaycastVehicle3DComponent.h>

#include <Systems/Movement3DSystem.h>
#include <Systems/ModelRender3DSystem.h>
#include <Systems/EntityStartup3DSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/PositionUpdater.h>

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
		eventBus->AddEventListener<OnCollisionEnter3DEvent>(this, &Game::OnCollisionEnter3D);

		/*
		*	Deserializes and spawns all static objects - not used for now...
		*/
		/*Scene3DDeserializer deserializer("../../../assets/scenes/test.scene", registry.get(), window);
		deserializer.Initialize();*/

		registry->AddSystem<Movement3DSystem>();
		registry->AddSystem<ModelRender3DSystem>(*window);
		registry->AddSystem<EntityStartup3DSystem>();
		registry->AddSystem<PhysicsSystem>(eventBus);
		registry->AddSystem<Physics3DSystem>(eventBus);
		registry->AddSystem<PositionUpdater>();

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

		test = registry->CreateEntity();
		std::shared_ptr<glt::Model> testModel(new glt::Model);
		testModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		test.AddComponent<TransformComponent>(glm::vec3(0.f, -50.f, 0.f), glm::vec3(0, -3.14159 / 2, 0), glm::vec3(6, 1, 6));
		test.AddComponent<BoxCollider3DComponent>(false, glm::vec3(6, 1, 3), 200);
		test.AddComponent<Rigidbody3DComponent>();
		test.AddComponent<Node3DComponent>("test", testModel);
		//finally we add the vehicle component, that needs wheels added to it once the registry is updated and knows of its existance...
		test.AddComponent<RaycastVehicle3DComponent>();

		key = registry->CreateEntity();
		std::shared_ptr<glt::Model> ballModel(new glt::Model_Obj("../../../assets/models/sphere.obj"));
		//ballModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		key.AddComponent<TransformComponent>(glm::vec3(-30.f, -54.f, -40.f), glm::vec3(0, 0, 0), glm::vec3(3, 3, 3));
		key.AddComponent<BoxCollider3DComponent>(true, glm::vec3(2), 0);
		key.AddComponent<Rigidbody3DComponent>();
		key.AddComponent<Node3DComponent>("ball", ballModel);

		Entity ground = registry->CreateEntity();
		std::shared_ptr<glt::Model> groundModel(new glt::Model);
		groundModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		ground.AddComponent<TransformComponent>(glm::vec3(0.f, -60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(50, 3, 100));
		ground.AddComponent<BoxCollider3DComponent>(false, glm::vec3(50, 3, 100));
		ground.AddComponent<Rigidbody3DComponent>();
		ground.AddComponent<Node3DComponent>("ground", groundModel);

		Entity ground2 = registry->CreateEntity();
		std::shared_ptr<glt::Model> ground2Model(new glt::Model);
		ground2Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		ground2.AddComponent<TransformComponent>(glm::vec3(-200, -60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(20, 3, 100));
		ground2.AddComponent<BoxCollider3DComponent>(false, glm::vec3(20, 3, 100));
		ground2.AddComponent<Rigidbody3DComponent>();
		ground2.AddComponent<Node3DComponent>("ground2", ground2Model);

		Entity ground3 = registry->CreateEntity();
		std::shared_ptr<glt::Model> ground3Model(new glt::Model);
		ground3Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		ground3.AddComponent<TransformComponent>(glm::vec3(-300, -60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(10, 3, 100));
		ground3.AddComponent<BoxCollider3DComponent>(false, glm::vec3(10, 3, 100));
		ground3.AddComponent<Rigidbody3DComponent>();
		ground3.AddComponent<Node3DComponent>("ground3", ground3Model);

		Entity platform = registry->CreateEntity();
		std::shared_ptr<glt::Model> platformModel(new glt::Model);
		platformModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		platform.AddComponent<TransformComponent>(glm::vec3(-61.f, -60.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(10, 3, 15));
		platform.AddComponent<BoxCollider3DComponent>(false, glm::vec3(10, 3, 15));
		platform.AddComponent<Rigidbody3DComponent>();
		platform.AddComponent<Node3DComponent>("platform", platformModel);

		Entity wallLeft = registry->CreateEntity();
		std::shared_ptr<glt::Model> wallLeftModel(new glt::Model);
		wallLeftModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wallLeft.AddComponent<TransformComponent>(glm::vec3(-190, -30.f, 70.f), glm::vec3(0, 0, 0), glm::vec3(5, 33, 45));
		wallLeft.AddComponent<BoxCollider3DComponent>(false, glm::vec3(5, 33, 45));
		wallLeft.AddComponent<Rigidbody3DComponent>();
		wallLeft.AddComponent<Node3DComponent>("wallLeft", wallLeftModel);

		Entity wallRight = registry->CreateEntity();
		std::shared_ptr<glt::Model> wallRightModel(new glt::Model);
		wallRightModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wallRight.AddComponent<TransformComponent>(glm::vec3(-190, -30.f, -70.f), glm::vec3(0, 0, 0), glm::vec3(5, 33, 45));
		wallRight.AddComponent<BoxCollider3DComponent>(false, glm::vec3(5, 33, 45));
		wallRight.AddComponent<Rigidbody3DComponent>();
		wallRight.AddComponent<Node3DComponent>("wallRight", wallRightModel);

		Entity wallUp = registry->CreateEntity();
		std::shared_ptr<glt::Model> wallUpModel(new glt::Model);
		wallUpModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wallUp.AddComponent<TransformComponent>(glm::vec3(-190, -8.f, 0.f), glm::vec3(0, 0, 0), glm::vec3(5, 11, 33));
		wallUp.AddComponent<BoxCollider3DComponent>(false, glm::vec3(5, 33, 45));
		wallUp.AddComponent<Rigidbody3DComponent>();
		wallUp.AddComponent<Node3DComponent>("wallUp", wallUpModel);

		
		wheel1 = registry->CreateEntity();
		std::shared_ptr<glt::Model> wheel1Model(new glt::Model);
		wheel1Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel1.AddComponent<TransformComponent>();
		wheel1.AddComponent<Node3DComponent>("wheel1", wheel1Model);

		wheel2 = registry->CreateEntity();
		wheel2.AddComponent<TransformComponent>();
		std::shared_ptr<glt::Model> wheel2Model(new glt::Model);
		wheel2Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel2.AddComponent<Node3DComponent>("wheel2", wheel2Model);

		wheel3 = registry->CreateEntity();
		wheel3.AddComponent<TransformComponent>();
		std::shared_ptr<glt::Model> wheel3Model(new glt::Model);
		wheel3Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel3.AddComponent<Node3DComponent>("wheel3", wheel3Model);

		wheel4 = registry->CreateEntity();
		wheel4.AddComponent<TransformComponent>();
		std::shared_ptr<glt::Model> wheel4Model(new glt::Model);
		wheel4Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
		wheel4.AddComponent<Node3DComponent>("wheel4", wheel4Model);


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
		kernel->AddRunningTask(registry->GetSystem<PositionUpdater>());
		kernel->AddRunningTask(registry->GetSystem<ModelRender3DSystem>());
	}

	bool Game::Initialize()
	{
		RaycastVehicle3DComponent& rv3d = test.GetComponent<RaycastVehicle3DComponent>();
		float wheelWidth = 1.f;
		float wheelRadius = 1.f;
		float wheelSuspensionHeight = -3.f;

		//Total of 4 wheels

		//Front wheels...
		glm::vec3 connectionPoint;

		connectionPoint = { glm::vec3(5, wheelSuspensionHeight, 3 )};
		rv3d.AddWheel(RaycastVehicleWheel(connectionPoint, wheelRadius, wheelWidth, true, wheel1));

		connectionPoint = { glm::vec3(-5, wheelSuspensionHeight, 3 )};
		rv3d.AddWheel(RaycastVehicleWheel(connectionPoint, wheelRadius, wheelWidth, true, wheel2));

		//Back wheels...
		connectionPoint = { glm::vec3(5, wheelSuspensionHeight, -3 )};
		rv3d.AddWheel(RaycastVehicleWheel(connectionPoint, wheelRadius, wheelWidth, false, wheel3));

		connectionPoint = { glm::vec3(-5, wheelSuspensionHeight, -3 )};
		rv3d.AddWheel(RaycastVehicleWheel(connectionPoint, wheelRadius, wheelWidth, false, wheel4));

		return true;
	}

	/*
	*	Runs every frame. Equivalent to similar functions in other engines like Update().
	*/
	void Game::Run(float deltaTime)
	{
		glm::vec3 refPos = test.GetComponent<TransformComponent>().position;
		glm::vec3 refRot = test.GetComponent<TransformComponent>().rotation;
		cam.GetComponent<TransformComponent>().SetTransformation({ refPos.x + 50, refPos.y + 10, refPos.z}, { 0, (3.14159 / 2), 0 }, {1,1,1});

		//Steering
		/*rv3d.vehicle->setSteeringValue(.14f, 0);
		rv3d.vehicle->setSteeringValue(.14f, 1);*/

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
		auto& rv3d = test.GetComponent< RaycastVehicle3DComponent >();

		switch (event.action)
		{
		case InputEvent::Action::QUIT:
			kernel->Stop();
			break;
		case InputEvent::Action::FORWARD:
			rv3d.vehicle->applyEngineForce(300 * event.value, 2);
			rv3d.vehicle->applyEngineForce(300 * event.value, 3);
			break;
		case InputEvent::Action::BACKWARDS:
			rv3d.vehicle->applyEngineForce(-300 * event.value, 2);
			rv3d.vehicle->applyEngineForce(-300 * event.value, 3);
			break;
		case InputEvent::Action::RIGHT:
			rv3d.vehicle->setSteeringValue(-.3f * event.value, 0);
			rv3d.vehicle->setSteeringValue(-.3f * event.value, 1);
			break;
		case InputEvent::Action::LEFT:
			rv3d.vehicle->setSteeringValue(.4f * event.value, 0);
			rv3d.vehicle->setSteeringValue(.4f * event.value, 1);
			break;
		case InputEvent::Action::JUMP:
			rv3d.vehicle->setBrake(6 * event.value, 0);
			rv3d.vehicle->setBrake(6 * event.value, 1);
			rv3d.vehicle->setBrake(6 * event.value, 2);
			rv3d.vehicle->setBrake(6 * event.value, 3);
			break;
		case InputEvent::Action::E:
			//Launch
			break;
		default:
			break;
		}
	}


	void Game::OnCollisionEnter3D(OnCollisionEnter3DEvent & event)
	{
		//Check here for the key

		std::string breakpoint_debug;

		if (event.collisionObj->getWorldArrayIndex() == key.GetComponent<Rigidbody3DComponent>().rigidbody->getWorldArrayIndex() ||
			event.otherObj->getWorldArrayIndex() == key.GetComponent<Rigidbody3DComponent>().rigidbody->getWorldArrayIndex())
		{
			playerHasKey = true;

			//std::string breakpoint_debug;
		}

	}

	void Game::OnTriggerEntry(OnTriggerEntryEvent& event)
	{
		
	}

}