#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/Node3DComponent.h"
#include "../Window/Window.h"
#include <spdlog/spdlog.h>

class ModelRender3DSystem : public System
{
public:
	ModelRender3DSystem()
	{
		// We specify the components that our system is interested in.
		RequireComponent<TransformComponent>();
		RequireComponent<Node3DComponent>();
	}

	void LoadEntities(glt::Render_Node& glRenderer, Window& window)
	{
		spdlog::info("Adding entities to OpenGL renderer...");
		for (auto entity : GetSystemEntities())
		{
			Node3DComponent openGlComp = entity.GetComponent<Node3DComponent>();
			glRenderer.add(openGlComp.modelId, openGlComp.node);
			spdlog::info("Added \"" + openGlComp.modelId + "\" to renderer system");
		}

		GLsizei width = GLsizei(window.GetWidth());
		GLsizei height = GLsizei(window.GetHeight());
		glRenderer.get_active_camera()->set_aspect_ratio(float(width) / height);
		glViewport(0, 0, width, height);
	}

	void Render(glt::Render_Node & glRenderer, Window & window)
	{
		glClearColor(0.2, 0.2f, 0.2f, 1);
		window.Clear();
		glRenderer.render();
		window.SwapBuffers();
	}
};