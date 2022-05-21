#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <ECS/ECS.h>
#include <Components/TransformComponent.h>
#include <Components/RigidbodyComponent.h>
#include <Components/Rigidbody3DComponent.h>
#include <Components/RaycastVehicle3DComponent.h>

#include <Components/Node3DComponent.h>
#include <gltk/Node.hpp>

namespace engine
{
	/// <summary>
	/// Physics-based transform synchronization
	/// </summary>
	class Movement3DSystem : public System
	{
	public:
		Movement3DSystem()
		{
			// We specify the components that our system is interested in.
			RequireComponent<TransformComponent>();
			//RequireComponent<RigidbodyComponent>();
			RequireComponent<Node3DComponent>();
		}

		void Run(float deltaTime)
		{
			for (auto& entity : GetSystemEntities())
			{
				//2D
				if (entity.HasComponent<RigidbodyComponent>())
				{
					std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
					auto& transform = entity.GetComponent<TransformComponent>();
					const auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

					b2Vec2 position = rigidbody.body->GetPosition();
					float angle = rigidbody.body->GetAngle();
					transform.SetTransformation(
						{ position.x, position.y, 0 },
						{ 0, 0, angle },
						{ transform.scale.x,transform.scale.y,transform.scale.z });

				}
				
				//3D
				if (entity.HasComponent<Rigidbody3DComponent>())
				{
					std::shared_ptr<glt::Node> node = entity.GetComponent<Node3DComponent>().node;
					auto& transform = entity.GetComponent<TransformComponent>();
					const auto& rigidbody3d = entity.GetComponent<Rigidbody3DComponent>();

					btVector3 position = rigidbody3d.rigidbody->getCenterOfMassPosition();
					transform.position = { position.x(), position.y(), position.z() };

					btTransform physics_transform;
					rigidbody3d.rigidbody->getMotionState()->getWorldTransform(physics_transform);
					glm::mat4 graphics_transform;
					physics_transform.getOpenGLMatrix(glm::value_ptr(graphics_transform));


					float xRot = 0;
					float yRot = 0;
					float zRot = 0;
					physics_transform.getRotation().getEulerZYX(zRot, yRot, xRot);
					transform.rotation = { xRot, yRot, zRot };

					transform.SetTransformation(glm::scale(graphics_transform, transform.scale));
				}

				
				//Special case for vehicle wheels...
				if (entity.HasComponent< RaycastVehicle3DComponent >())
				{
					RaycastVehicle3DComponent& rv3d = entity.GetComponent< RaycastVehicle3DComponent >();

					int i = 0;
					for (const RaycastVehicleWheel & wheel : rv3d.wheels)
					{
						TransformComponent& transform = wheel.wheelEntity->GetComponent<TransformComponent>();
						const btTransform& physicsTransform = rv3d.vehicle->getWheelInfo(i).m_worldTransform;

						glm::mat4 graphics_transform;
						physicsTransform.getOpenGLMatrix(glm::value_ptr(graphics_transform));

						const btVector3 & origin = physicsTransform.getOrigin();
						transform.position = { origin.x(), origin.y(), origin.z() };
						float xRot = 0;
						float yRot = 0;
						float zRot = 0;
						physicsTransform.getRotation().getEulerZYX(zRot, yRot, xRot);
						transform.rotation = {xRot, yRot, zRot};
						transform.scale = glm::vec3(wheel.wheelRadius);
						transform.SetTransformation(glm::scale(graphics_transform, transform.scale));

						++i;
					}
				}
				

			}
		}
	};
}