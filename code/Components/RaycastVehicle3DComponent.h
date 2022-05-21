#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <ECS/ECS.h>

namespace engine
{
	const float CUBE_HALF_EXTENTS = 1;

	struct RaycastVehicleWheel
	{
		/// <summary>
		/// 0 is the middle, 1 extends to the full relative of a cube (so 1 is front, -1 is back)
		/// 
		/// </summary>
		//There's no documentation about these variables in Bullet. This is the generic configuration for most vehicles.

		//Can't make these variables "CONST"?
		glm::vec3 wheelDirection = { 0, -1, 0 };
		glm::vec3 wheelAxle = { -1, 0, 0 };

		float suspensionStiffness = 20.f;
		float suspensionDamping = 2.3f;
		float suspensionCompression = 4.4f;
		float wheelFriction = 1000;
		float rollInfluence = 0.1f;
		float suspensionRestLength = 0.6f;

		/// <summary>
		/// Defines the contact point relative to the vehicle's chassis position.
		/// </summary>
		glm::vec3 connectionPoint;

		bool isFrontWheel = false;

		float wheelRadius, wheelWidth;

		//An entity that represents the wheel.
		Entity* wheelEntity;

		RaycastVehicleWheel() = default;

		RaycastVehicleWheel(glm::vec3 connectionPoint, float wheelRadius, float wheelWidth, bool isFrontWheel, Entity & wheelEntity)
		{
			this->connectionPoint = connectionPoint;
			this->wheelRadius = wheelRadius;
			this->isFrontWheel = isFrontWheel;
			this->wheelEntity = &wheelEntity;
		}
	};

	/// <summary>
	/// A component that creates a BULLET vehicle using raycast for the contact points.
	/// It needs wheels added to function properly (Component -> AddWheel) or it can cause a crash.
	/// </summary>
	struct RaycastVehicle3DComponent
	{
		float connectionHeight = 1.2f;
		float wheelWidth = 1;

		std::shared_ptr<btRaycastVehicle> vehicle = nullptr;
		std::shared_ptr<btVehicleRaycaster> vehicleRaycaster = nullptr;
		btRaycastVehicle::btVehicleTuning tuning;

		std::vector<RaycastVehicleWheel> wheels;

		RaycastVehicle3DComponent() = default;

		void AddWheel(const RaycastVehicleWheel & wheel)
		{
			wheels.push_back(wheel);
		}
	};


}