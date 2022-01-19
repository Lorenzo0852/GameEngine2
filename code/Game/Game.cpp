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

#include "../Logger/Logger.h"
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


Game::Game()
{
	isRunning = false;
	registry = std::make_unique<Registry>();
	assetManager = std::make_unique<AssetManager>();
	spdlog::info("Game constructor called");
}

void Game::Initialize(Window & window)
{
	window.SetWindowedFullscreen();
	window.SetVsync(true);

	this->window = &window;
	std::shared_ptr< glt::Model  > cube(new glt::Model);
	//-1 index means "get the default monitor/screen for the renderer".
	//renderer = SDL_CreateRenderer(window.sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC);
	glRenderer.reset(new glt::Render_Node);
	//std::shared_ptr< glt::Model  > cube(new glt::Model);
	//std::shared_ptr< glt::Camera > camera(new glt::Camera(20.f, 1.f, 500.f, 1.f));
	std::shared_ptr< glt::Light  > light(new glt::Light);

	//cube->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());

	//glRenderer->add("camera", camera);
	glRenderer->add("light", light);
	//glRenderer->add("cube", cube);

	//glRenderer->get("camera")->translate(glt::Vector3(0.f, 0.f, 5.f));
	glRenderer->get("light")->translate(glt::Vector3(10.f, 10.f, 10.f));

	//if (!renderer)
	//{
	//	gameLogger->error("Error creating SDL renderer.");
	//	return;
	//}

	//if (!glContext)
	//{
	//	gameLogger->error("Error creating OpenGL context.");
	//	return;
	//}

	isRunning = true;
}

void Game::Run()
{
	while (isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}


void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
			//Event that the system triggers when the user closes the window
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				{
					isRunning = false;
				}
				break;
		}
	}
}

void Game::SetupScene()
{
	/*rapidxml::file<> xmlFile("../../assets/scenes/test.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	spdlog::info("XML: " + std::string(doc.first_node()->name()));*/
	
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<ModelRender3DSystem>();
	registry->AddSystem<Movement3DSystem>();
	registry->AddSystem<EntityStartup3DSystem>();

	Entity cube = registry->CreateEntity();
	std::shared_ptr< glt::Model  > cubeModel(new glt::Model);
	cubeModel->add(std::shared_ptr<glt::Drawable>(new glt::Cube), glt::Material::default_material());

	cube.AddComponent<RigidbodyComponent>(glm::vec3(0.f, 0, -1.f), glm::vec3(0.f, 0.2f, -0.2f));
	cube.AddComponent<TransformComponent>(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	cube.AddComponent<Node3DComponent>("cube", cubeModel);

	Entity cam = registry->CreateEntity();
	std::shared_ptr< glt::Camera > cameraNode(new glt::Camera(20.f, 1.f, 500.f, 1.f));

	cam.AddComponent<RigidbodyComponent>();
	cam.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	cam.AddComponent<Node3DComponent>("camera", cameraNode);

	Entity fence = registry->CreateEntity();
	std::shared_ptr< glt::Model  > fenceModel(new glt::Model_Obj("../../../assets/models/utah-teapot.obj"));
	fence.AddComponent<RigidbodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0, 0.2f, 0));
	fence.AddComponent<TransformComponent>(glm::vec3(5, 0, -75), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	fence.AddComponent<Node3DComponent>("fence", fenceModel);

	//Update registry to process the entities that are waiting
	registry->Update();
	registry->GetSystem<ModelRender3DSystem>().LoadEntities(*glRenderer, *window);
	registry->GetSystem<EntityStartup3DSystem>().SetStartingTransform();

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

void Game::Update()
{
	double deltaTime = (static_cast<double>(SDL_GetTicks64()) - millisecondsPreviousFrame) * 0.001;
	millisecondsPreviousFrame = SDL_GetTicks64();

	//Ask systems to update
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<Movement3DSystem>().Update(deltaTime);
	//Update registry to process the entities that are waiting
	registry->Update();
}

/// <summary>
/// Renders all gameobjects
/// </summary>
void Game::Render()
{
	registry->GetSystem<ModelRender3DSystem>().Render(*glRenderer, *window);

	//GLsizei width = GLsizei(window->GetWidth());
	//GLsizei height = GLsizei(window->GetHeight());

	//glRenderer->get_active_camera()->set_aspect_ratio(float(width) / height);

	//glViewport(0, 0, width, height);

	//glClearColor(0.5, 0.5f, 0.5f, 1);
	//window->Clear();
	//glRenderer->render();
	//window->SwapBuffers();

	//// Se rota el objeto:

	//auto cube = glRenderer->get("cube");

	//cube->rotate_around_x(0.01f);
	//cube->rotate_around_y(0.02f);
	//cube->rotate_around_z(0.03f);

	////SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	////SDL_RenderClear(renderer);

	//Ask systems to render
	//registry->GetSystem<RenderSystem>().Render(renderer, assetManager);
	// Render game objects...

	//Present the renderer / swaps render buffers.
	//SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Game::ClearOpenGLContext() const
{
	if (glContext)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}