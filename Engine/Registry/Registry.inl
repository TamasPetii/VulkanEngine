#pragma once
#include "Registry.h"
#include "Relationship.h"

template<typename ...T>
inline const std::vector<Entity>& Registry::View()
{
	ComponentBitsetMask bitMask = GenerateMask<T...>();

	if (views.find(bitMask) == views.end())
	{
		static std::vector<Entity> defaultView{};
		return defaultView;
	}

	return views.at(bitMask).GetDenseIndices();
}

template<typename ...T>
inline void Registry::RegisterView()
{
	ComponentBitsetMask bitMask = GenerateMask<T...>();

	if (views.find(bitMask) == views.end())
		views[bitMask];
}

template<typename T>
inline ComponentPool<T>* Registry::GetPool()
{
	UniqueID type_index = Unique::typeIndex<T>();

	if (!pools.ContainsIndex(type_index))
		return nullptr;

	return static_cast<ComponentPool<T>*>(pools.GetData(type_index));
}

template<typename ...T>
inline std::tuple<ComponentPool<T>*...> Registry::GetPools()
{
	return { GetPool<T>()... };
}

template <typename T>
inline bool Registry::HasComponent(Entity entity)
{
	UniqueID type_index = Unique::typeIndex<T>();
	return pools.ContainsIndex(type_index) && (pools.GetData(type_index))->HasComponent(entity);
}

template<typename... T>
inline bool Registry::HasComponents(Entity entity)
{
	return (HasComponent<T>(entity) && ...);
}

template<typename T>
inline T& Registry::GetComponent(Entity entity)
{
	[[likely]]
	if (!HasComponent<T>(entity))
		throw std::runtime_error("Invalid index for accesing component!");

	return static_cast<ComponentPool<T>*>(pools.GetData(Unique::typeIndex<T>()))->GetData(entity);
}

template<typename ...T>
inline std::tuple<T&...> Registry::GetComponents(Entity entity)
{
	return { GetComponent<T>(entity)... };
}

template<typename T>
inline void Registry::AddComponent(Entity entity, const T& component)
{
	if (HasComponent<T>(entity))
		return;

	UniqueID type_index = Unique::typeIndex<T>();

	[[unlikely]]
	if (!pools.ContainsIndex(type_index))
		pools.Add(type_index, new ComponentPool<T>);

	static_cast<ComponentPool<T>*>(pools.GetData(type_index))->Add(entity, component);
}

template<typename ...T>
inline void Registry::AddComponents(Entity entity)
{
	static_assert((std::is_default_constructible_v<T> && ...), "All types must be default constructible.");
	AddComponents<T...>(entity, T{}...);
}

template<typename ...T>
inline void Registry::AddComponents(Entity entity, const T & ...component)
{
	(AddComponent<T>(entity, component), ...);

	auto& entityBitSet = GetComponent<ComponentBitsetMask>(entity);

	(SetBitMaskBit<T>(entityBitSet), ...);

	for (auto& [viewBitMask, entityPool] : views)
	{
		if ((viewBitMask & entityBitSet) == viewBitMask)
			entityPool.Add(entity);
	}
}

template<typename T>
inline void Registry::RemoveComponent(Entity entity)
{
	[[likely]]
	if (!HasComponent<T>(entity))
		return;

	UniqueID type_index = Unique::typeIndex<T>();
	(pools.GetData(type_index))->RemoveComponent(entity);
}

template<typename ...T>
inline void Registry::RemoveComponents(Entity entity)
{
	auto prevEntityBitset = GetComponent<ComponentBitsetMask>(entity);
	
	(RemoveComponent<T>(entity), ...);

	auto& entityBitset = GetComponent<ComponentBitsetMask>(entity);

	(SetBitMaskBit<T>(entityBitset, false), ...);

	for (auto& [viewBitMask, entityPool] : views)
	{
		if ((viewBitMask & prevEntityBitset) == viewBitMask && (viewBitMask & entityBitset) != viewBitMask)
			entityPool.Remove(entity);
	}
}

template<typename T>
inline void Registry::SetBitMaskBit(ComponentBitsetMask& bitMask, bool valid)
{
	bitMask.set(Unique::typeIndex<T>(), valid);
}

template<typename ...T>
inline ComponentBitsetMask Registry::GenerateMask()
{
	ComponentBitsetMask bitmask;

	(SetBitMaskBit<T>(bitmask), ...);

	return bitmask;
}
