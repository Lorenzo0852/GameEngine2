/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <gltk/Cube.hpp>
#include <gltk/Model.hpp>
//#include <gltk/Model_Obj.hpp>
#include <gltk/Light.hpp>
#include <gltk/Render_Node.hpp>
//#include <gltk/Camera.hpp>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/Node3DComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/ModelRender3DSystem.h"
#include "../Systems/Movement3DSystem.h"
#include "../Systems/EntityStartup3DSystem.h"


Game::Game(Window & window, Kernel & kernel, std::shared_ptr<EventBus> eventBus)
{
	registry			=		std::make_unique<Registry>();
	assetManager		=		std::make_unique<AssetManager>();
	this->eventBus		=		eventBus;

	this->kernel = &kernel;
	this->window = &window;

	window.SetWindowedFullscreen();
	window.SetVsync(true);

	spdlog::info("Game constructor called");
}


void Game::SetupScene()
{
	/*rapidxml::file<> xmlFile("../../../assets/scenes/test.scene");
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	rapidxml::xml_node<>* registryNode = doc.first_node()->first_node("registry");
	while (registryNode != 0)
	{
		rapidxml::xml_node<>* childNode = registryNode->first_node();
		while (childNode != 0)
		{
			spdlog::info("" + std::string(childNode->value()));
			childNode = childNode->next_sibling();
		}
		registryNode = registryNode->next_sibling("registry");
	}*/
	
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<ModelRender3DSystem>(*window);
	registry->AddSystem<Movement3DSystem>();
	registry->AddSystem<EntityStartup3DSystem>();

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


	cam = registry->CreateEntity();
	std::shared_ptr< glt::Camera > cameraNode(new glt::Camera(20.f, 1.f, 500.f, 1.f));

	cam.AddComponent<RigidbodyComponent>();
	cam.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	cam.AddComponent<Node3DComponent>("camera", cameraNode);

	enemy1 = registry->CreateEntity();
	std::shared_ptr< glt::Model  > enemy1Model(new glt::Model);
	enemy1Model->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
	enemy1.AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0.f, 0.f, 0.f));
	enemy1.AddComponent<TransformComponent>(glm::vec3(36, 14, -20.f), glm::vec3(0, 0, 0), glm::vec3(.4f, .4f, .4f));
	
	enemy1.AddComponent<Node3DComponent>("enemy1", enemy1Model);

	/*********************************************
	*	WALLS
	*********************************************/

	Entity upperWall = registry->CreateEntity();
	std::shared_ptr< glt::Model  > upperWallModel(new glt::Model);
	upperWallModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());

	upperWall.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
	upperWall.AddComponent<TransformComponent>(glm::vec3(0, 15, -20.f), glm::vec3(0, 0, 0), glm::vec3(40, 1, 1));
	upperWall.AddComponent<Node3DComponent>("upperWall", upperWallModel);

	Entity bottomWall = registry->CreateEntity();
	std::shared_ptr< glt::Model  > bottomWallModel(new glt::Model);
	bottomWallModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());

	bottomWall.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
	bottomWall.AddComponent<TransformComponent>(glm::vec3(0, -15, -20.f), glm::vec3(0, 0, 0), glm::vec3(40, 1, 1));
	bottomWall.AddComponent<Node3DComponent>("bottomWall", bottomWallModel);

	Entity leftWall = registry->CreateEntity();
	std::shared_ptr< glt::Model  > leftWallModel(new glt::Model);
	leftWallModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
	leftWall.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
	leftWall.AddComponent<TransformComponent>(glm::vec3(-37, 0, -20.f), glm::vec3(0, 0, 0), glm::vec3(1, 40, 1));
	leftWall.AddComponent<Node3DComponent>("leftWall", leftWallModel);

	Entity rightWall = registry->CreateEntity();
	std::shared_ptr< glt::Model  > rightWallModel(new glt::Model);
	rightWallModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());
	rightWall.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
	rightWall.AddComponent<TransformComponent>(glm::vec3(37, 0, -20.f), glm::vec3(0, 0, 0), glm::vec3(1, 40, 1));
	rightWall.AddComponent<Node3DComponent>("rightWall", rightWallModel);

	//teapot = registry->CreateEntity();
	//std::shared_ptr< glt::Model  > teapotModel(new glt::Model_Obj("../../../assets/models/utah-teapot.obj"));
	//teapot.AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0, 0.2f, 0));
	//teapot.AddComponent<TransformComponent>(glm::vec3(15, 0, -75), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	//teapot.AddComponent<Node3DComponent>("teapot", teapotModel);

	light = registry->CreateEntity();
	std::shared_ptr< glt::Light  > lightNode(new glt::Light);
	lightNode->set_color(glm::vec3(0.4f,0.7f,0.929f));
	light.AddComponent<TransformComponent>(glt::Vector3(10.f, 10.f, 10.f), glt::Vector3(0.f, 0.f, 0.f));
	light.AddComponent<Node3DComponent>("light", lightNode);

	//Update registry to process the entities that are waiting
	kernel->InitializeTask(*registry);
	kernel->InitializeTask(registry->GetSystem<ModelRender3DSystem>());
	kernel->InitializeTask(registry->GetSystem<EntityStartup3DSystem>());
	kernel->AddRunningTask(registry->GetSystem<Movement3DSystem>());
	kernel->AddRunningTask(registry->GetSystem<ModelRender3DSystem>());

	/*registry->AddSystem<RenderSystem>();*/

	//assetManager->AddTexture(renderer, "tank-image", "../../../assets/images/tank-panther-right.png");
	//assetManager->AddTexture(renderer, "truck-image", "../../../assets/images/truck-ford-right.png");

	//Entity tank = registry->CreateEntity();
	//gameLogger-> info("Entity ID: " + std::to_string(tank.GetId()));

	//tank.AddComponent<TransformComponent>(glm::vec3(2000, 500, 0), glm::vec3(5.0, 5.0, 5.0), 0);
	//tank.AddComponent<RigidbodyComponent>(glm::vec3(50.0, 0.0, 0.0));
	//tank.AddComponent<SpriteComponent>("tank-image", 32, 32);

	//Entity truck = registry->CreateEntity();
	//gameLogger->info("Entity ID: " + std::to_string(truck.GetId()));

	//truck.AddComponent<TransformComponent>(glm::vec3(1800, 500, 0), glm::vec3(5.0, 5.0, 5.0), 0);
	//truck.AddComponent<RigidbodyComponent>(glm::vec3(50.0, 0.0, 0.0));
	//truck.AddComponent<SpriteComponent>("truck-image", 32, 32);
}

void Game::Run(float deltaTime)
{
	eventBus->Reset();
	eventBus->AddEventListener<InputEvent>(this, &Game::OnInputRegistered);
	Movement3DSystem& movement3DSystem = registry->GetSystem<Movement3DSystem>();
	TransformComponent& playerTransform = player.GetComponent<TransformComponent>();
	if (playerTransform.position.y > 14)
	{
		auto& transform = player.GetComponent<TransformComponent>();
		transform.position.y = 13.95f;
		registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
	}

	if (playerTransform.position.y < -14)
	{
		auto& transform = player.GetComponent<TransformComponent>();
		transform.position.y = -13.95f;
		registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
	}

	if (playerTransform.position.x > 35)
	{
		auto& transform = player.GetComponent<TransformComponent>();
		transform.position.x = 34.95f;
		registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
	}

	if (playerTransform.position.x < -35)
	{
		auto& transform = player.GetComponent<TransformComponent>();
		transform.position.x = -34.95f;
		registry->GetSystem<Movement3DSystem>().MoveToPosition(player, transform.position);
	}
	
	TransformComponent& enemy1TransformComponent = enemy1.GetComponent<TransformComponent>();
	glt::Node * enemyNode = enemy1.GetComponent<Node3DComponent>().node.get();

	glm::vec3 directionVector = movement3DSystem.MoveTowards(enemy1, playerTransform.position);
	
	enemyNode->translate(directionVector * 4.f * deltaTime);
}

void Game::OnInputRegistered(InputEvent& event)
{
	RigidbodyComponent & cubeRigidbody = player.GetComponent<RigidbodyComponent>();
	switch (event.action)
	{
	case InputEvent::Action::QUIT :
		kernel->Stop();
		break;
	case InputEvent::Action::FORWARD:
		cubeRigidbody.velocity = glm::vec3(cubeRigidbody.velocity.x	,	 8 * event.value		,	 cubeRigidbody.velocity.z);
		break;
	case InputEvent::Action::BACKWARDS:
		cubeRigidbody.velocity = glm::vec3(cubeRigidbody.velocity.x	,	-8 * event.value		,	 cubeRigidbody.velocity.z);
		break;
	case InputEvent::Action::LEFT:
		cubeRigidbody.angularVelocity = glm::vec3(0, 0, 2 * event.value);
		break;
	case InputEvent::Action::RIGHT:
		cubeRigidbody.angularVelocity = glm::vec3(0, 0, -2 * event.value);
		break;
	}
}