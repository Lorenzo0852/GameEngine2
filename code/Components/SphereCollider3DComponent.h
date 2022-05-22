#pragma once

#include <ECS/ECS.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>


namespace engine
{
	struct SphereCollider3DComponent
	{
		std::shared_ptr< btCollisionShape > shape;
		float mass = 0;
		glm::vec3 localInertia = glm::vec3(0);
		float radius = 1;

		bool isTrigger = false;

		SphereCollider3DComponent(bool isTrigger = false, float radius = 1, float mass = 0, glm::vec3 localInertia = glm::vec3(0))
			:
			isTrigger(isTrigger),
			radius(radius),
			mass(mass),
			localInertia(localInertia)
		{
			shape.reset(new btSphereShape(btScalar(1.)));
		}
	};
}