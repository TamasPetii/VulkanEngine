#pragma once
#include "../EngineApi.h"
#include "Pool.h"
#include "Unique.h"

#include <set>
#include <memory>
#include <unordered_map>

using ComponentBitsetMask = std::bitset<32>;

class Registry
{
public:
	Registry();
	Entity CreateEntity();
	void DestroyEntity(Entity entity);
	void SetActiveEntity(Entity entity);
	Entity GetActiveEntity();
public:
	template<typename... T>
	std::vector<Entity>& View();
	template<typename... T>
	void RegisterView();
	template<typename T>
	std::shared_ptr<Pool<T>> GetPool();
	template <typename... T>
	std::tuple<std::shared_ptr<Pool<T>>...> GetPools();
	template <typename T>
	bool HasComponent(Entity entity);
	template <typename... T>
	bool HasComponents(Entity entity);
	template <typename T>
	T* GetComponent(Entity entity);
	template <typename... T>
	std::tuple<T*...> GetComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity, const T&... component);
	template <typename... T>
	void RemoveComponents(Entity entity);
private:
	template <typename T>
	void AddComponent(Entity entity, const T& component);
	template <typename T>
	void RemoveComponent(Entity entity);
	template <typename T>
	void SetBitMaskBit(ComponentBitsetMask& bitMask, bool valid = true);
	template <typename... T>
	ComponentBitsetMask GenerateMask();
private:
	Entity counter = 0;
	Entity activeEntity = NULL_ENTITY;
	std::set<Entity> destroyedEntities;
	Pool<std::shared_ptr<PoolBase>> pools;
	std::unordered_map<ComponentBitsetMask, Pool<uint32_t>> views;
};

#include "Registry.inl"