#pragma once
#include "Registry.h"
#include "Relationship.h"
#include "ComponentBitset.h"

template<uint32_t MAX_COMPONENTS>
Entity Registry<MAX_COMPONENTS>::CreateEntity()
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

	//Component bit set must be the first
	AddComponent<ComponentBitset<MAX_COMPONENTS>>(entity);
	AddComponent<Relationship>(entity);

	return entity;
}

template<uint32_t MAX_COMPONENTS>
void Registry<MAX_COMPONENTS>::DestroyEntity(Entity entity)
{
	//Remove the parent-child relations

	//Remove from all the component pools
	for (auto& pool : pools)
	{
		if(pool != nullptr)
			pool->RemoveEntity(entity);
	}

	destroyedEntities.insert(entity);
}


template<uint32_t MAX_COMPONENTS>
template<typename T>
inline void Registry<MAX_COMPONENTS>::SetComponentBit(std::bitset<MAX_COMPONENTS>& bitset)
{
	bitset.set(Unique::typeID<T>(), true);
}

template<uint32_t MAX_COMPONENTS>
template <typename T>
inline bool Registry<MAX_COMPONENTS>::HasComponent(Entity entity)
{
	UniqueID type_id = Unique::typeID<T>();
	return pools[type_id] != nullptr && pools[type_id]->HasComponent(entity); //?? Use Bitset
}

template<uint32_t MAX_COMPONENTS>
template<typename T>
inline T* Registry<MAX_COMPONENTS>::GetComponent(Entity entity)
{
	if (!HasComponent<T>(entity))
		return nullptr;
	
	UniqueID type_id = Unique::typeID<T>();
	return std::static_pointer_cast<Pool<T>>(pools[type_id])->GetComponent(entity);
}

template<uint32_t MAX_COMPONENTS>
template<typename T>
inline void Registry<MAX_COMPONENTS>::AddComponent(Entity entity)
{
	AddComponent<T>(entity, T{});
}

template<uint32_t MAX_COMPONENTS>
template<typename T>
inline void Registry<MAX_COMPONENTS>::AddComponent(Entity entity, const T& component)
{
	static_assert(std::is_default_constructible<T>::value, "T must be default constructible.");

	if (!HasComponent<T>(entity))
	{
		UniqueID type_id = Unique::typeID<T>();

		if (pools[type_id] == nullptr)
			pools[type_id] = std::make_shared<Pool<T>>();

		std::static_pointer_cast<Pool<T>>(pools[type_id])->AddComponent(entity, component);
		GetComponent<ComponentBitset<MAX_COMPONENTS>>(entity)->bitset.set(type_id, true);
	}
}

template<uint32_t MAX_COMPONENTS>
template<typename T>
inline void Registry<MAX_COMPONENTS>::RemoveComponent(Entity entity)
{
	if (!HasComponent<T>(entity))
		return;

	UniqueID type_id = Unique::typeID<T>();
	pools[type_id]->RemoveComponent(entity);
	GetComponent<ComponentBitset<MAX_COMPONENTS>>(entity)->bitset.set(type_id, false);
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline void Registry<MAX_COMPONENTS>::RegisterEntitiesWithBitset()
{
	auto bitset = GetComponentsBitset<T...>();
	entitiesWithBitset[bitset];
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline std::bitset<MAX_COMPONENTS> Registry<MAX_COMPONENTS>::GetComponentsBitset()
{
	std::bitset<MAX_COMPONENTS> bitset;
	(SetComponentBit<T>(bitset), ...);
	return bitset;
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline const std::set<Entity>& Registry<MAX_COMPONENTS>::GetEntitiesWithBitset()
{
	auto bitset = GetComponentsBitset<T...>();
	return entitiesWithBitset[bitset];
}

template<uint32_t MAX_COMPONENTS>
template<typename... T>
inline bool Registry<MAX_COMPONENTS>::HasComponents(Entity entity)
{
	return (HasComponent<T>(entity) && ...);
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline std::tuple<T*...> Registry<MAX_COMPONENTS>::GetComponents(Entity entity)
{
	return { GetComponent<T>(entity)... };
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline void Registry<MAX_COMPONENTS>::AddComponents(Entity entity)
{
	(AddComponent<T>(entity), ...);

	for (auto& [requiredBitset, entities] : entitiesWithBitset)
	{
		if ((requiredBitset & GetComponent<ComponentBitset<MAX_COMPONENTS>>(entity)->bitset) == requiredBitset)
			entities.insert(entity);
	}
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline void Registry<MAX_COMPONENTS>::AddComponents(Entity entity, const T & ...component)
{
	(AddComponent<T>(entity, component), ...);

	for (auto& [requiredBitset, entities] : entitiesWithBitset)
	{
		if ((requiredBitset & GetComponent<ComponentBitset<MAX_COMPONENTS>>(entity)->bitset) == requiredBitset)
			entities.insert(entity);
	}
}

template<uint32_t MAX_COMPONENTS>
template<typename ...T>
inline void Registry<MAX_COMPONENTS>::RemoveComponents(Entity entity)
{
	auto prevBitset = GetComponent<ComponentBitset<MAX_COMPONENTS>>(entity)->bitset;

	(RemoveComponent<T>(entity), ...);

	for (auto& [requiredBitset, entities] : entitiesWithBitset)
	{
		if ((requiredBitset & prevBitset) == requiredBitset && (requiredBitset & GetComponent<ComponentBitset<MAX_COMPONENTS>>(entity)->bitset) != requiredBitset)
			entities.erase(entity);
	}
}
