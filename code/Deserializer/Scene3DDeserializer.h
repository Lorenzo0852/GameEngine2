#pragma once
#include "../Task/Task.h"
#include "../ECS/ECS.h"
#include "Scene3DDeserializer.h"
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include "../Systems/EntityStartup3DSystem.h"
#include "../Systems/ModelRender3DSystem.h"
#include "../Systems/Movement3DSystem.h"

#include "../Components/Node3DComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/TransformComponent.h"

class Scene3DDeserializer : public Task
{
private:
	Registry* registry;
	Window* window;
	const char * path;

public:

	Scene3DDeserializer(const char* path) : path(path) {}
	Scene3DDeserializer(const char* path, Registry* registry, Window* window) : path(path), registry(registry), window(window) {}

	bool Initialize();
	void Run(float deltaTime);
};