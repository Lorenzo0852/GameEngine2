#pragma once
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <memory>

namespace engine
{
	struct Rigidbody3DComponent
	{
		//btTransform should be == to the entity's transform on instantiation so they both are in sync.
		btTransform transform;
		std::shared_ptr<btDefaultMotionState>		state;
		//Rigidbody gets injected by the Physics3DSystem...
		std::shared_ptr<btRigidBody>				rigidbody;

		Rigidbody3DComponent() = default;

		void SetTransformOrigin(glm::vec3 origin, glm::vec3 originRotation)
		{
			transform.setIdentity();
			transform.setOrigin(btVector3{origin.x, origin.y, origin.z});
			btQuaternion rot;
			rot.setEulerZYX(originRotation.z, originRotation.y, originRotation.x);
			transform.setRotation(rot);
		}
	};
}