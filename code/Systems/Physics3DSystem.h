#pragma once

#include <ECS/ECS.h>
#include <EventBus/EventBus.h>
#include <Events/OnCollisionEnter3DEvent.h>

#include <spdlog/spdlog.h>
#include <EventBus/EventBus.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Components/TransformComponent.h>
#include <Components/Rigidbody3DComponent.h>
#include <Components/BoxCollider3DComponent.h>
#include <Components/SphereCollider3DComponent.h>
#include <Components/RaycastVehicle3DComponent.h>





namespace engine
{
	/// <summary>
	/// 3D related physics use BULLET SDK
	/// </summary>
	class Physics3DSystem : public System
	{
	private:
		//Allocated on the heap according to Bullet's user manual
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btCollisionWorld* collisionWorld;

		std::shared_ptr<EventBus> eventBus;

		//We don't need to keep track of states, rigidbodies and shapes as we have access to all the Rigidbody3DComponents affected by the system,
		//where we already have all that data.

	public:
		Physics3DSystem(std::shared_ptr<EventBus> eventBus, glm::vec3 gravity = { 0, -9.8f, 0 });
		~Physics3DSystem();

		void SetGravity(glm::vec3 gravity);

		bool Initialize() override;
		void Run(float deltaTime) override; //pure virtual

		static void MyNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo);
	};
}