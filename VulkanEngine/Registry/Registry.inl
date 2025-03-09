#pragma once
#include "Registry.h"

template <typename T>
inline bool Registry::HasComponent(Entity entity)
{
	std::type_index typeID = typeid(T);
	return pools.find(typeID) != pools.end() && pools[typeID] != nullptr && std::static_pointer_cast<Pool<T>>(pools[typeID])->HasComponent(entity);
}

template<typename T>
inline T* Registry::GetComponent(Entity entity)
{
	if (!HasComponent<T>(entity))
		return nullptr;
	
	return std::static_pointer_cast<Pool<T>>(pools[typeid(T)])->GetComponent(entity);
}

template<typename T>
inline void Registry::AddComponent(Entity entity)
{
	static_assert(std::is_default_constructible<T>::value, "T must be default constructible.");

	AddComponent<T>(entity, T{});
}

template<typename T>
inline void Registry::AddComponent(Entity entity, const T& component)
{
	static_assert(std::is_default_constructible<T>::value, "T must be default constructible.");

	if (!HasComponent<T>(entity))
	{
		if (pools.find(typeid(T)) == pools.end())
			pools[typeid(T)] = std::make_shared<Pool<T>>();

		std::static_pointer_cast<Pool<T>>(pools[typeid(T)])->AddComponent(entity, component);
	}
}

template<typename... T>
inline bool Registry::HasComponents(Entity entity)
{
	return (HasComponent<T>(entity) && ...);
}

template<typename ...T>
inline std::tuple<T*...> Registry::GetComponents(Entity entity)
{
	return { GetComponent<T>(entity)... };
}

template<typename ...T>
inline void Registry::AddComponents(Entity entity)
{
	static_assert((std::is_default_constructible<T>::value && ...), "T must be default constructible.");
	(AddComponent<T>(entity), ...);
}

template<typename ...T>
inline void Registry::AddComponents(Entity entity, const T & ...component)
{
	static_assert((std::is_default_constructible<T>::value && ...), "T must be default constructible.");

	(AddComponent<T>(entity, component), ...);

}
