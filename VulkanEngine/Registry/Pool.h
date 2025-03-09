#pragma once
#include "Entity.h"
#include <memory>
#include <vector>

constexpr uint32_t PAGE_SIZE = 16;

class PoolBase
{
public:
	virtual ~PoolBase() = default;
	virtual void AddEntity(Entity entity) = 0;
	virtual void RemoveEntity(Entity entity) = 0;
};

template<typename T>
class Pool : public PoolBase
{
public:
	void AddEntity(Entity entity) override;
	void RemoveEntity(Entity entity) override;

	bool HasComponent(Entity entity);
	T*   GetComponent(Entity entity);
	void AddComponent(Entity entity);
	void AddComponent(Entity entity, const T& component);
	void RemoveComponent(Entity entity);

	//Getter
	const auto& GetSparseEntityPages() { return sparseEntityPages; }
	const auto& GetDenseEntities() { return denseEntities; }
	const auto& GetDenseComponents() { return denseComponents; }
protected:
	void RegisterEntity(Entity entity);
	std::pair<Index, Index> GetPageIndices(Entity entity);
private:
	std::vector<std::vector<Entity>> sparseEntityPages;
	std::vector<Entity> denseEntities;
	std::vector<T> denseComponents;
};

#include "Pool.inl"