#pragma once
/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <memory>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include "../Pool/Pool.h"
#include "../Task/Task.h"

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
	int id;

public:
	Entity() = default;
	Entity(int id) : id(id) {};
	//Not gonna change any value of internal member variables or attributes, so the method is const.
	int GetId() const { return id; }

	template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename TComponent> void RemoveComponent();
	template <typename TComponent> bool HasComponent() const;
	template <typename TComponent> TComponent& GetComponent() const;

	//Forward declaration so we can use it prior to its implementation.
	class Registry* registry;

	//We need comparison operator overloads as std::set needs comparators to work with the type it's managing.
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
public:
	/// <summary>
	/// Returns the unique ID of the component
	/// </summary>
	/// <returns>unsigned ID of component</returns>
	static unsigned GetId() {
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
class System : public Task{
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
class Registry : public Task{
private:
	//Keeps track of how many entities were added to the scene.
	int numEntities = 0;

	/// <summary>
	/// Vector of component pools, each pools contains all the data for a certain component type.
	/// [Vector index	=	component type id]
	/// [Pool index		=	entityId]
	/// componentPools[0][123] returns component with ID "0" of the entity "123"
	/// </summary>
	std::vector<std::shared_ptr<IPool>> componentPools;

	/// <summary>
	/// Vector of component signatures per entity, saying which component is turned "on" for a certain entity.
	/// [index		=	entityId]
	/// </summary>
	std::vector<Signature> entityComponentSignatures;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;
	
public:
	Registry() = default;

	/*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*
	* Entity management
	*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/
	Entity CreateEntity();

	/*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*
	* Component management
	*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/

	/// <summary>
	/// Add a component of type TComponent to a given entity.
	/// There can be components where we might want (or need) to pass certain additional information 
	/// to add them, so we support that using a variadic template.
	/// 
	/// Example: AddComponent -TransformComponent- (entity, position, rotation).
	/// </summary>
	/// <typeparam name="TComponent">Component to add</typeparam>
	/// <typeparam name="...TArgs">Additional arguments</typeparam>
	/// <param name="entity">Entity that holds this component</param>
	/// <param name="...args">Additional arguments</param>
	template <typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);
	// Why TArgs&&??:
	//  https://stackoverflow.com/questions/61234704/can-someone-help-me-enlighten-this-template/61237996#61237996
	//  https://en.cppreference.com/w/cpp/language/reference#Forwarding_references
	//  https://www.geeksforgeeks.org/lvalue-and-rvalue-in-c-language/
	//  https://docs.microsoft.com/en-us/cpp/cpp/lvalues-and-rvalues-visual-cpp?view=msvc-160

	template<typename TComponent>
		bool HasComponent(Entity entity) const;
	template<typename TComponent>
		void RemoveComponent(Entity entity);
	template <typename TComponent>
		TComponent& GetComponent(Entity entity) const;

	/*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*
	* System management
	*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/
	template <typename TSystem, typename ...TArgs>
		void AddSystem(TArgs&& ...args);
	template <typename TSystem>
		void RemoveSystem();
	template <typename TSystem>
		bool HasSystem() const;
	template <typename TSystem>
		TSystem& GetSystem() const;

	// Checks the component signature of an entity and add the entity to the systems
	//that are interested in it.
	void AddEntityToSystems(Entity entity);

	/*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*
	* Game management
	*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/
	
	/// <summary>
	/// Updates the registry for the first time.
	/// </summary>
	bool Initialize();
	/// <summary>
	/// Updates the registry
	/// </summary>
	/// <param name="deltaTime"></param>
	void Run(float deltaTime);
};





/****************************************************************************************\
 *  GENERICS
\****************************************************************************************/

template <typename TComponent>
void System::RequireComponent(){
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	//We resize the pool if needed
	if (componentId >= componentPools.size())
	{
		componentPools.resize(componentId + 1, nullptr);
	}

	//We check if there is any component in the index we are checking. If there isn't, create it.
	if (!componentPools[componentId])
	{
		// Only deallocate this when all the references go out of scope
		// It's a shared_ptr as we immediately assign it to a different variable, with a different scope.
		// The object of the shared ptr will destroy when everything that makes reference to it goes out of scope.
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	if (entityId >= componentPool->GetSize())
	{
		componentPool->Resize(numEntities);
	}

	TComponent newComponent(std::forward<TArgs>(args)...);
	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	auto componentId = Component<TComponent>::GetId();
	return entityComponentSignatures[entity.GetId()].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	auto componentId = Component<TComponent>::GetId();
	entityComponentSignatures[entity.GetId()].set(componentId, false);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
	//.end() in an unordered map is the element following the last element,
	//returned if it can't find the key.
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto system = systems.find(	std::type_index(	typeid(TSystem)	)	);
	//.find() returns a pair, where ->first is the first element and ->second the second one.
	//We need to dereference it so it doesn't return a memory address, but its content.
	return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>(*this);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
	registry->RemoveComponent<TComponent>(*this);
}


