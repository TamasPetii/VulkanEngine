#include "Registry.h"

Registry::Registry()
{
}

Entity Registry::CreateEntity()
{
	Entity entity = NULL_ENTITY;

	if (destroyedEntities.empty())
	{
		entity = counter++;
	}
	else
	{
		auto it = destroyedEntities.begin();
		entity = *it;
		destroyedEntities.erase(it);
	}

	AddComponent<Relationship>(entity);
	return entity;
}

void Registry::DestroyEntity(Entity entity)
{
	//Remove the parent-child relations

	//Remove from all the component pools
	for (auto& pool : pools.GetDenseComponents())
			pool->RemoveEntity(entity);

	destroyedEntities.insert(entity);
}

void Registry::SetActiveEntity(Entity entity)
{
	activeEntity = entity;
}

Entity Registry::GetActiveEntity()
{
	return activeEntity;
}
