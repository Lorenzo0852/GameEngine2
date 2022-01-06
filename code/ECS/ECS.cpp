#include "ECS.h"

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

// Everytime you have a static variable, we need to initialize that variable.
int IComponent::nextId = 0;

void System::AddEntityToSystem(Entity entity){
	entities.push_back(entity);
}

/// <summary>
/// Test all the entities, from start to end, and removes the entity with the same ID the parameter.
/// </summary>
/// <param name="entity">Entity to remove.</param>
void System::RemoveEntityFromSystem(Entity entity){

	/* /!\ How this works - Erase-remove idiom:
	* Vector::erase() can receive either a single element to remove, or a range of elements.
	* In this case, we are removing a range of elements that starts at the result of the std::remove_if
	* function and ends in the last element in the vector, the iterator entities.end() pointing to the
	* last element.
	*
	* To understand where this starts and the whole range we are going to erase, we have to know how
	* std::remove_if works:
	* 
	* std::remove_if swaps elements inside the vector in order to put all elements that do not match
	* the function (the lambda function used here) towards the beginning of the container. So if the
	* function returns true, the element will be placed at the end of the vector.
	* 
	* Then, std::remove_if returns an iterator pointing to the first element WHICH MATCHES the function,
	* so in other words, returns the iterator pointing to the start of all entities that are equal.
	* 
	* Vector::erase() then simply removes everything from that point to the end of the vector, which
	* should be everything that matched the content of the function.
	* 
	* More on this: https://stackoverflow.com/questions/39019806/using-erase-remove-if-idiom/39019851#39019851
	* An example:	https://en.wikipedia.org/wiki/Erase–remove_idiom
	*/

	entities.erase(
		std::remove_if(
			entities.begin(),
			entities.end(),
			[&entity](Entity other) {
				return entity == other;
			}), entities.end());
}

const Signature& System::GetComponentSignature() const{
	return componentSignature;
}

Entity Registry::CreateEntity() {
	size_t entityId = numEntities++;
	Entity entity(entityId);

	entitiesToBeAdded.insert(entity);
	return entity;
}

void Registry::AddEntityToSystems(Entity entity){
	const auto entityId = entity.GetId();
	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	for (auto& system : systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update(){
	
}