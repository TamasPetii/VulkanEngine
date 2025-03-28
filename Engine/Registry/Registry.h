#pragma once
#include "../EngineApi.h"
#include "Pool.h"
#include "Unique.h"

#include <set>
#include <bitset>
#include <unordered_map>

template<uint32_t MAX_COMPONENTS>
class Registry
{
public:
	const auto& GetCounter() const { return counter; }
	const auto& GetActiveEntity() const { return activeEntity; }
	const auto& GetDestroyedEntities() const { return destroyedEntities; }
	const auto& GetPools() const { return pools; }
	const auto& GetEntitiesWithBitset() const { return entitiesWithBitset; }

	Entity CreateEntity();
	void   DestroyEntity(Entity entity);
	template <typename... T>
	void RegisterEntitiesWithBitset();
	template <typename... T>
	const std::set<Entity>& GetEntitiesWithBitset();
	template <typename... T>
	bool HasComponents(Entity entity);
	template <typename... T>
	std::tuple<T*...> GetComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity, const T&... component);
	template <typename... T>
	void RemoveComponents(Entity entity);
protected:
	template <typename... T>
	std::bitset<MAX_COMPONENTS> GetComponentsBitset();
	template <typename T>
	void SetComponentBit(std::bitset<MAX_COMPONENTS>& bitset);
	template <typename T>
	bool HasComponent(Entity entity);
	template <typename T>
	T* GetComponent(Entity entity);
	template <typename T>
	void AddComponent(Entity entity, const T& component);
	template <typename T>
	void AddComponent(Entity entity);
	template <typename T>
	void RemoveComponent(Entity entity);
private:
	Entity counter = 0;
	Entity activeEntity = NULL_ENTITY;
	std::set<Entity> destroyedEntities;
	std::array<std::shared_ptr<PoolBase>, MAX_COMPONENTS> pools;
	std::unordered_map<std::bitset<MAX_COMPONENTS>, std::set<Entity>> entitiesWithBitset;
};

#include "Registry.inl"