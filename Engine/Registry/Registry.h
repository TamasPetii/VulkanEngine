#pragma once
#include "../EngineApi.h"
#include "Pool.h"
#include "Unique.h"

#include <set>
#include <memory>
#include <unordered_map>

class Registry
{
public:
	Registry();
	Entity CreateEntity();
	void   DestroyEntity(Entity entity);

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

	template <typename T>
	void AddComponent(Entity entity);
	template <typename T>
	void AddComponent(Entity entity, const T& component);
	template <typename... T>
	void AddComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity, const T&... component);

	template <typename T>
	void RemoveComponent(Entity entity);
	template <typename... T>
	void RemoveComponents(Entity entity);

private:
	Entity counter = 0;
	Entity activeEntity = NULL_ENTITY;
	std::set<Entity> destroyedEntities;
	Pool<std::shared_ptr<PoolBase>> pools;
};

#include "Registry.inl"