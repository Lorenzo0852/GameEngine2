#include <Systems/Physics3DSystem.h>

namespace engine
{
	Physics3DSystem::Physics3DSystem(std::shared_ptr<EventBus> eventBus, glm::vec3 gravity)
	{
		RequireComponent<TransformComponent>();
		RequireComponent<Rigidbody3DComponent>();

		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		collisionWorld = dynamicsWorld->getCollisionWorld();
		SetGravity(gravity);

		dispatcher->setNearCallback(MyNearCallback);

		this->eventBus = eventBus;

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

	void Physics3DSystem::MyNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo)
	{
		dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
	}

	void Physics3DSystem::Run(float dt)
	{
		//Has a internal fixed timestep of 1/60 (60Hz) applied to steps that are smaller.
		dynamicsWorld->stepSimulation(dt);

		int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = contactManifold->getBody0();
			const btCollisionObject* obB = contactManifold->getBody1();

			eventBus->FireEvent<OnCollisionEnter3DEvent>(obA, obB);

		}

		for (Entity& entity : GetSystemEntities())
		{
			//Constraint stuff
			if(entity.HasComponent< RaycastVehicle3DComponent >())
			{
				auto& rv3d = entity.GetComponent< RaycastVehicle3DComponent >();

				for (int i = 0; i < rv3d.vehicle->getNumWheels(); i++)
				{
					rv3d.vehicle->updateWheelTransform(i, true);
				}
			}
		}

	}

	bool Physics3DSystem::Initialize()
	{
		for (Entity & entity : GetSystemEntities())
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

				rb3d.SetTransformOrigin(transformComp.position, transformComp.rotation);
				rb3d.state.reset(new btDefaultMotionState(rb3d.transform));
				btRigidBody::btRigidBodyConstructionInfo info(mass, rb3d.state.get(), bc3d.shape.get(), localInertia);
				btRigidBody* newRigidbody = new btRigidBody(info);
				newRigidbody->setRestitution(.7f);

				if (bc3d.isTrigger)
				{
					//Disable contact between bodies...
					newRigidbody->setCollisionFlags(newRigidbody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
				}
				
				rb3d.rigidbody.reset(newRigidbody);
				dynamicsWorld->addRigidBody(newRigidbody);

				spdlog::info("Added BOX shape to physics-enabled entity with ID: " + std::to_string(entity.GetId()));
			}
			else if (entity.HasComponent<SphereCollider3DComponent>())
			{
				SphereCollider3DComponent& sc3d = entity.GetComponent<SphereCollider3DComponent>();
				btScalar& mass = sc3d.mass;
				btVector3 localInertia = btVector3{ sc3d.localInertia.x, sc3d.localInertia.y, sc3d.localInertia.z };

				sc3d.shape->calculateLocalInertia(mass, localInertia);

				rb3d.SetTransformOrigin(transformComp.position, transformComp.rotation);
				rb3d.state.reset(new btDefaultMotionState(rb3d.transform));
				btRigidBody::btRigidBodyConstructionInfo info(mass, rb3d.state.get(), sc3d.shape.get(), localInertia);
				btRigidBody* newRigidbody = new btRigidBody(info);

				newRigidbody->setRestitution(.7f);

				if (sc3d.isTrigger)
				{
					//Disable contact between bodies...
					newRigidbody->setCollisionFlags(newRigidbody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
				}

				rb3d.rigidbody.reset(newRigidbody);
				dynamicsWorld->addRigidBody(newRigidbody);

				spdlog::info("Added SPHERE shape to physics-enabled entity with ID: " + std::to_string(entity.GetId()));
			}
			else
			{
				spdlog::error("Rigidbodies can't function without a physics shape! Attach a collider to all physics-affected entities.");
				return false;
			}

			//Constraint-related stuff...

			if (entity.HasComponent< RaycastVehicle3DComponent >())
			{
				auto & rv3d = entity.GetComponent< RaycastVehicle3DComponent >();
				rv3d.vehicleRaycaster.reset(new btDefaultVehicleRaycaster(dynamicsWorld));
				//We consider the chassis to be the entity with this component.
				rv3d.vehicle.reset(new btRaycastVehicle(rv3d.tuning, rb3d.rigidbody.get(), rv3d.vehicleRaycaster.get()));

				rb3d.rigidbody->setActivationState(DISABLE_DEACTIVATION);
				dynamicsWorld->addVehicle(rv3d.vehicle.get());

				int rightIndex = 0;
				int upIndex = 1;
				int forwardIndex = 2;
				rv3d.vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

				int i = 0;
				for (RaycastVehicleWheel wheel : rv3d.wheels)
				{
					rv3d.vehicle->addWheel(
						btVector3{ wheel.connectionPoint.x, wheel.connectionPoint.y, wheel.connectionPoint.z },
						btVector3{ wheel.wheelDirection.x, wheel.wheelDirection.y, wheel.wheelDirection.z },
						btVector3{ wheel.wheelAxle.x, wheel.wheelAxle.y, wheel.wheelAxle.z },
						wheel.suspensionRestLength,
						wheel.wheelRadius,
						rv3d.tuning,
						wheel.isFrontWheel);

					btWheelInfo& wheelInfo = rv3d.vehicle->getWheelInfo(i++);

					wheelInfo.m_suspensionStiffness = wheel.suspensionStiffness;
					wheelInfo.m_wheelsDampingRelaxation = wheel.suspensionDamping;
					wheelInfo.m_wheelsDampingCompression = wheel.suspensionCompression;
					wheelInfo.m_frictionSlip = wheel.wheelFriction;
					wheelInfo.m_rollInfluence = wheel.rollInfluence;
				}

			}

		}

		return true;
	}
}