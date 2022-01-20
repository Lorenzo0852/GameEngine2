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

	cube = registry->CreateEntity();
	std::shared_ptr< glt::Model  > cubeModel(new glt::Model);
	cubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());

	cube.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, 0), glm::vec3(0.f, 0.f, 0.f));
	cube.AddComponent<TransformComponent>(glm::vec3(0, 0, -20.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	cube.AddComponent<Node3DComponent>("cube", cubeModel);

	cam = registry->CreateEntity();
	std::shared_ptr< glt::Camera > cameraNode(new glt::Camera(20.f, 1.f, 500.f, 1.f));

	cam.AddComponent<RigidbodyComponent>();
	cam.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	cam.AddComponent<Node3DComponent>("camera", cameraNode);

	teapot = registry->CreateEntity();
	std::shared_ptr< glt::Model  > teapotModel(new glt::Model_Obj("../../../assets/models/utah-teapot.obj"));
	teapot.AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0, 0.2f, 0));
	teapot.AddComponent<TransformComponent>(glm::vec3(15, 0, -75), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	teapot.AddComponent<Node3DComponent>("teapot", teapotModel);

	light = registry->CreateEntity();
	std::shared_ptr< glt::Light  > lightNode(new glt::Light);
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
}

void Game::OnInputRegistered(InputEvent& event)
{
	RigidbodyComponent & cubeRigidbody = cube.GetComponent<RigidbodyComponent>();
	switch (event.action)
	{
	case InputEvent::Action::QUIT :
		kernel->Stop();
		break;
	case InputEvent::Action::FORWARD:
		cubeRigidbody.velocity = glm::vec3(cubeRigidbody.velocity.x	,	 10 * event.value		,	 cubeRigidbody.velocity.z);
		break;
	case InputEvent::Action::BACKWARDS:
		cubeRigidbody.velocity = glm::vec3(cubeRigidbody.velocity.x	,	-10 * event.value		,	 cubeRigidbody.velocity.z);
		break;
	case InputEvent::Action::LEFT:
		cubeRigidbody.velocity = glm::vec3(		-10 * event.value	, cubeRigidbody.velocity.y	,	 cubeRigidbody.velocity.z);
		break;
	case InputEvent::Action::RIGHT:
		cubeRigidbody.velocity = glm::vec3(		10 * event.value	, cubeRigidbody.velocity.y	,	 cubeRigidbody.velocity.z);
		break;
	}
}