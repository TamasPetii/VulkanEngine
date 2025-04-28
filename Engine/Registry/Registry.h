#pragma once
#include "Engine/EngineApi.h"
#include "Pools/ComponentPool.h"
#include "Unique.h"

#include <set>
#include <array>
#include <memory>
#include <unordered_map>
#include "Relationship.h"

using ComponentBitsetMask = std::bitset<32>;

class ENGINE_API Registry
{
public:
	Registry();
	Entity CreateEntity();
	void DestroyEntity(Entity entity);
	void SetActiveEntity(Entity entity);
	Entity GetActiveEntity();
	void SetParent(Entity entity, Parent parent);
	bool IsDeepConnected(Entity entity, Parent parent);
	auto& GetLevels() { return levels; }
public:
	template<typename... T>
	void RegisterView();
	template<typename... T>
	const std::vector<Entity>& View();
	template<typename T>
	ComponentPool<T>* GetPool();
	template <typename... T>
	std::tuple<ComponentPool<T>*...> GetPools();
	template <typename T>
	bool HasComponent(Entity entity);
	template <typename... T>
	bool HasComponents(Entity entity);
	template <typename T>
	T& GetComponent(Entity entity);
	template <typename... T>
	std::tuple<T&...> GetComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity);
	template <typename... T>
	void AddComponents(Entity entity, const T&... component);
	template <typename... T>
	void RemoveComponents(Entity entity);
private:
	void RefreshChildLevelDeep(Entity entity);
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
	DataPool<IComponentPool*> pools;
	std::unordered_map<ComponentBitsetMask, SparseSet> views;
	std::array<std::set<Entity>, MAX_RELATIONSHIP_DEPTH> levels;
};

#include "Registry.inl"