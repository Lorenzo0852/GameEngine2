#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ECS/ECS.h>

namespace engine
{
	/// <summary>
/// Components are data-driven structures. Nothing more.
/// All functionality needed in a component is managed in the "System" part of Entity-Component-System
/// </summary>
	struct TransformComponent {

		Entity* parent = NULL;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::vec3 initialPosition;
		glm::vec3 initialRotation;
		glm::vec3 initialScale;
		glm::mat4 transformation;

		TransformComponent(
			glm::vec3 position = glm::vec3(0, 0, 0),
			glm::vec3 rotation = glm::vec3(0, 0, 0),
			glm::vec3 scale = glm::vec3(0, 0, 0),
			Entity* parent = nullptr) {

			this->position = position;
			this->scale = scale;
			this->rotation = rotation;
			this->parent = parent;
			transformation = glm::mat4(1);
			
			glm::mat4 identity(1);
			glm::mat4 translation = glm::translate(identity, position);
			glm::mat4 rotationMat = glm::rotate(identity, rotation.x, glm::vec3(1, 0, 0));
			glm::mat4 scaling = glm::scale(identity, scale);

			transformation = scaling;

			if (parent != nullptr)
			{
				glm::mat4 parentTransformation = parent->GetComponent<TransformComponent>().transformation;
				transformation *= parentTransformation;
			}
		}

		glm::mat4 SetTransformation(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)
		{
			position = _position;
			rotation = _rotation;
			scale = _scale;

			transformation = glm::mat4(1);
			transformation = glm::translate(transformation, _position);
			transformation = glm::rotate(transformation, _rotation.x, glm::vec3(1, 0, 0));
			transformation = glm::rotate(transformation, _rotation.y, glm::vec3(0, 1, 0));
			transformation = glm::rotate(transformation, _rotation.z, glm::vec3(0, 0, 1));
			transformation = glm::scale(transformation, _scale);

			if (parent != nullptr)
			{
				glm::mat4 parentTransformation = parent->GetComponent<TransformComponent>().transformation;
				transformation *= parentTransformation;
			}

			return transformation;
		}

		glm::mat4 SetTransformation(glm::mat4 _transformation)
		{
			transformation = _transformation;

			if (parent != nullptr)
			{
				glm::mat4 parentTransformation = parent->GetComponent<TransformComponent>().transformation;
				transformation *= parentTransformation;
			}

			return transformation;
		}

		glm::mat4 GetTransformation()
		{
			return transformation;
		}

	};
}