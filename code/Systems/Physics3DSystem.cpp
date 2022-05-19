#include <Systems/Physics3DSystem.h>

namespace engine
{
	Physics3DSystem::Physics3DSystem(glm::vec3 gravity)
	{
		RequireComponent<TransformComponent>();
		RequireComponent<Rigidbody3DComponent>();

		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		SetGravity(gravity);

		spdlog::info("BULLET SDK world successfully created.");
	}

	Physics3DSystem::~Physics3DSystem()
	{
		//We should be deleting all rigidbodies and collision shapes on here.

		//deleting all allocated objects in reverse order of creation...
		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
	}

	void Physics3DSystem::SetGravity(glm::vec3 gravity)
	{
		dynamicsWorld->setGravity({ gravity.x, gravity.y, gravity.z });
	}

	void Physics3DSystem::Run(float dt)
	{
		//Has a internal fixed timestep of 1/60 (60Hz) applied to steps that are smaller.
		dynamicsWorld->stepSimulation(dt);
	}

	bool Physics3DSystem::Initialize()
	{
		for (Entity entity : GetSystemEntities())
		{
			Rigidbody3DComponent& rb3d = entity.GetComponent<Rigidbody3DComponent>();
			TransformComponent& transformComp = entity.GetComponent<TransformComponent>();

			//Emplace shapes
			if (entity.HasComponent<BoxCollider3DComponent>())
			{
				BoxCollider3DComponent & bc3d = entity.GetComponent<BoxCollider3DComponent>();
				btScalar& mass = bc3d.mass;
				btVector3 localInertia = btVector3{bc3d.localInertia.x, bc3d.localInertia.y, bc3d.localInertia.z};

				bc3d.shape->calculateLocalInertia(mass, localInertia);

				rb3d.SetTransformOrigin(transformComp.position);
				rb3d.state.reset(new btDefaultMotionState(rb3d.transform));
				btRigidBody::btRigidBodyConstructionInfo info(mass, rb3d.state.get(), bc3d.shape.get(), localInertia);
				btRigidBody* newRigidbody = new btRigidBody(info);

				newRigidbody->setRestitution(.7f);
				
				rb3d.rigidbody.reset(newRigidbody);
				dynamicsWorld->addRigidBody(newRigidbody);

				spdlog::info("Added box shape to physics-enabled entity with ID: " + std::to_string(entity.GetId()));
			}
			else
			{
				spdlog::error("Rigidbodies can't function without a physics shape! Attach a collider to all physics-affected entities.");
				return false;
			}

		}

		return true;
	}
}