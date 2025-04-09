#pragma once
#include "Registry.h"
#include "Relationship.h"

template<typename T>
inline std::shared_ptr<Pool<T>> Registry::GetPool()
{
	UniqueID type_index = Unique::typeIndex<T>();

	if (!pools.HasComponent(type_index))
		return nullptr;

	return std::static_pointer_cast<Pool<T>>(*pools.GetComponent(type_index));
}

template<typename ...T>
inline std::tuple<std::shared_ptr<Pool<T>>...> Registry::GetPools()
{
	return { GetPool<T>()... };
}

template <typename T>
inline bool Registry::HasComponent(Entity entity)
{
	UniqueID type_index = Unique::typeIndex<T>();
	return pools.HasComponent(type_index) && (*pools.GetComponent(type_index))->HasComponent(entity);
}

template<typename... T>
inline bool Registry::HasComponents(Entity entity)
{
	return (HasComponent<T>(entity) && ...);
}

template<typename T>
inline T* Registry::GetComponent(Entity entity)
{
	if (!HasComponent<T>(entity))
		return nullptr;

	UniqueID type_index = Unique::typeIndex<T>();
	return std::static_pointer_cast<Pool<T>>(*pools.GetComponent(type_index))->GetComponent(entity);
}

template<typename ...T>
inline std::tuple<T*...> Registry::GetComponents(Entity entity)
{
	return { GetComponent<T>(entity)... };
}

template<typename T>
inline void Registry::AddComponent(Entity entity)
{
	AddComponent<T>(entity, T{});
}

template<typename T>
inline void Registry::AddComponent(Entity entity, const T& component)
{
	static_assert(std::is_default_constructible<T>::value, "T must be default constructible.");

	if (HasComponent<T>(entity))
		return;

	UniqueID type_index = Unique::typeIndex<T>();

	if (!pools.HasComponent(type_index))
		pools.AddComponent(type_index, std::make_shared<Pool<T>>());

	std::static_pointer_cast<Pool<T>>(*pools.GetComponent(type_index))->AddComponent(entity, component);
}

template<typename ...T>
inline void Registry::AddComponents(Entity entity)
{
	(AddComponent<T>(entity), ...);
}

template<typename ...T>
inline void Registry::AddComponents(Entity entity, const T & ...component)
{
	(AddComponent<T>(entity, component), ...);
}

template<typename T>
inline void Registry::RemoveComponent(Entity entity)
{
	if (!HasComponent<T>(entity))
		return;

	UniqueID type_index = Unique::typeIndex<T>();
	(*pools.GetComponent(type_index))->RemoveComponent(entity);
}

template<typename ...T>
inline void Registry::RemoveComponents(Entity entity)
{
	(RemoveComponent<T>(entity), ...);
}
