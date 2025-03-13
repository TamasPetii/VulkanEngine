#pragma once
#include "Pool.h"
#include "Unique.h"

#include <set>
#include <tuple>
#include <bitset>
#include <typeindex>
#include <unordered_map>

constexpr uint32_t MAX_COMPONENTS = 32;

class Registry
{
public:
	Entity CreateEntity();
	void   DestroyEntity(Entity entity);

	template <typename... T>
	void RegisterComponentBitset();
	template <typename... T>
	const std::set<Entity>& GetEntitiesWithBitset() const;
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
	void GetComponentsBitset();
	template <typename T>
	void SetComponentBit(std::bitset<MAX_COMPONENTS>& bitset);
	template <typename T>
	bool HasComponent(Entity entity);
	template <typename T>
	T* GetComponent(Entity entity);
	template <typename T>
	void AddComponent(Entity entity);
	template <typename T>
	void AddComponent(Entity entity, const T& component);
	template <typename T>
	void RemoveComponent(Entity entity);
private:
	Entity counter;
	Entity activeEntity;
	std::set<Entity> destroyedEntities;
	std::unordered_map<std::type_index, std::shared_ptr<PoolBase>> pools;
	std::vector<std::bitset<MAX_COMPONENTS>> bitsetComponents;
	std::unordered_map<std::bitset<MAX_COMPONENTS>, std::set<Entity>> bitsetEntities;
};

#include "Registry.inl"