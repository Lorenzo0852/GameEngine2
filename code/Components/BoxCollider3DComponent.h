#pragma once

#include <ECS/ECS.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>


namespace engine
{
	struct BoxCollider3DComponent
	{
		std::shared_ptr< btCollisionShape > shape;
		float mass = 0;
		glm::vec3 localInertia = glm::vec3(0);

		glm::vec3 size = glm::vec3(1);

		bool isTrigger = false;

		BoxCollider3DComponent(bool isTrigger = false, glm::vec3 halfExtents = { 1,1,1 }, float mass = 0, glm::vec3 localInertia = glm::vec3(0))
			:
			isTrigger(isTrigger),
			size(halfExtents),
			mass(mass),
			localInertia(localInertia)
		{
			shape.reset(new btBoxShape({ halfExtents.x, halfExtents.y, halfExtents.z }));
		}
	};
}