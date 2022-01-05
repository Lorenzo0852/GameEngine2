#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

#include "../Pool/Pool.h"

/****************************************************************************************\
 *  ENTITY
\****************************************************************************************/
/*	We could have all of this separated in different files, but having all of this
*	centralized helps us see how the ECS system is working at first glance. It's also
*	easier to configure if needed.
*/

const unsigned int MAX_COMPONENTS = 32;

/// <summary>
/// We use a bitset (1s and 0s) to keep track of which component/s an entity has.
/// It also helps keep track of which entities a given system is interested in.
/// </summary>
typedef std::bitset<MAX_COMPONENTS> Signature;





/****************************************************************************************\
 *  ENTITY
\****************************************************************************************/

/// <summary>
/// Base entity class
/// </summary>
class Entity {
private:
	unsigned id;

public:
	Entity(unsigned id) : id(id) {};
	//Not gonna change any value of internal member variables or attributes, so the method is const.
	unsigned GetId() const { return id; }
	bool operator <  (const Entity& other) const { return id < other.id; }
	bool operator >  (const Entity& other) const { return id > other.id; }
	bool operator <=  (const Entity& other) const { return id <= other.id; }
	bool operator >=  (const Entity& other) const { return id >= other.id; }
	bool operator == (const Entity& other) const { return id == other.id; }
	bool operator != (const Entity& other) const { return id != other.id; }
	Entity& operator = (const Entity& other) = default;
};





/****************************************************************************************\
 *  COMPONENT
\****************************************************************************************/
/*
* The compiler will generate different classes per type TComponent. So being this the case,
* everytime a new class is generated (say Component_TransformComponent for Component<TransformComponent>,
* we can get an unique ID using its static method GetId().
* 
* A static member is shared by all objects of the same class, meaning that GetId() will always return
* the same ID for the same Component class.
* 
* Local static variables are initialized ONLY the first time control enters the function, so its ID doesn't
* keep getting incremented using nextId++ while on the same component type. A new component type will, however
* enter control of the function again (as it is essentially a new class created by the compiler), incrementing
* the ID and being unique to that new class (new component type).
* 
* More on static members: https://en.cppreference.com/w/cpp/language/static
*/


struct IComponent
{
protected:
	static int nextId;
};

/// <summary>
/// Used to assign a unique ID to a component type
/// </summary>
template <typename TComponent>
class Component : public IComponent {
	/// <summary>
	/// Returns the unique ID of component<T>
	/// </summary>
	/// <returns></returns>
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};





/****************************************************************************************\
 *  SYSTEM
\****************************************************************************************/

/// <summary>
/// The system processes entities that contain a specific signature.
/// </summary>
class System {
private:
	/// <summary>
	/// Components required by the system.
	/// </summary>
	Signature componentSignature;
	/// <summary>
	/// Entities affected by this system.
	/// </summary>
	std::vector<Entity> entities;
public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const { return entities; };
	const Signature& GetComponentSignature() const;

	/// <summary>
	/// Defines the component type that entities must have to be considered by the system.
	/// </summary>
	/// <typeparam name="TComponent">Component type</typeparam>
	template <typename TComponent> void RequireComponent();
};





/****************************************************************************************\
 *  REGISTRY
\****************************************************************************************/
/*
* The registry manages the creation and destruction of entities, adds systems, and components.
* Also called World, Manager... in different architectures.
*/
/// <summary>
/// Base registry/manager class. Used to coordinate the different ECS implementations.
/// </summary>
class Registry {
private:
	//Keeps track of how many entities were added to the scene.
	size_t numEntities = 0;

	/// <summary>
	/// Vector of component pools, each pools contains all the data for a certain component type.
	/// [Vector index	=	component type id]
	/// [Pool index		=	entity id]
	/// componentPools[0][123] returns component with ID "0" of the entity "123"
	/// </summary>
	std::vector<IPool*> componentPools;
	/// <summary>
	/// Vector of component signatures per entity, saying which component is turned "on" for a certain entity.
	/// </summary>
	std::vector<Signature> entityComponentSignatures;
	std::unordered_map<std::type_index, System*> systems;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;
	
public:
	Registry() = default;

	Entity CreateEntity();
	void Update();

};





/****************************************************************************************\
 *  GENERICS
\****************************************************************************************/

template <typename TComponent>
void System::RequireComponent(){
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}