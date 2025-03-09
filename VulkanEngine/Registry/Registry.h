#pragma once
#include "Pool.h"
#include "Relationship.h"

#include <set>
#include <tuple>
#include <unordered_map>
#include <typeindex>

class Registry
{
public:
	Entity CreateEntity();
	void   DestroyEntity(Entity entity);
	
	template <typename... T>
	bool HasComponents(Entity entity);
	template <typename... T>
	std::tuple<T*...> GetComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity, const T&... component);
protected:
	template <typename T>
	bool HasComponent(Entity entity);
	template <typename T>
	T* GetComponent(Entity entity);
	template <typename T>
	void AddComponent(Entity entity);
	template <typename T>
	void AddComponent(Entity entity, const T& component);
private:
	Entity counter;
	Entity activeEntity;
	std::set<Entity> destroyedEntities;
	std::unordered_map<std::type_index, std::shared_ptr<PoolBase>> pools;
};

#include "Registry.inl"