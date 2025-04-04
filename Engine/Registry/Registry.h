#pragma once
#include "../EngineApi.h"
#include "Pool.h"
#include "Unique.h"

#include <set>
#include <bitset>
#include <memory>
#include <array>
#include <unordered_map>

constexpr uint32_t DEFAULT_MAX_COMPONENTS = 32;

template<uint32_t MAX_COMPONENTS>
class Registry
{
public:
	Entity CreateEntity();
	void   DestroyEntity(Entity entity);

	template <typename... T>
	std::tuple<std::shared_ptr<Pool<T>>...> GetPools();
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
	template <typename T>
	std::shared_ptr<Pool<T>> GetPool();
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