#pragma once
#include "../EngineApi.h"
#include "Entity.h"
#include <memory>
#include <vector>
#include <bitset>

constexpr uint32_t PAGE_SIZE = 16;
constexpr uint32_t REGENERATE_BIT = 0;
constexpr uint32_t UPDATE_BIT = 1;
constexpr uint32_t CHANGED_BIT = 2;
constexpr uint32_t INDEX_CHANGED_BIT = 3;

class ENGINE_API PoolBase
{
public:
	virtual ~PoolBase() = default;
	virtual uint32_t GetDenseSize() = 0;
	virtual Index GetIndex(Entity entity) = 0;
	virtual void AddEntity(Entity entity) = 0;
	virtual void RemoveEntity(Entity entity) = 0;
	virtual bool HasComponent(Entity entity) = 0;
	virtual void RemoveComponent(Entity entity) = 0;
	virtual std::bitset<8>& GetBitset(Entity entity) = 0;
};

template<typename T>
class Pool : public PoolBase
{
public:
	Pool() = default;
	void AddEntity(Entity entity) override;
	void RemoveEntity(Entity entity) override;
	uint32_t GetDenseSize() override { return static_cast<uint32_t>(denseEntities.size()); }

	bool HasComponent(Entity entity) override;
	T*  GetComponent(Entity entity);
	void AddComponent(Entity entity);
	void AddComponent(Entity entity, const T& component);
	void RemoveComponent(Entity entity) override;

	//Getter
	virtual Index GetIndex(Entity entity) override;
	auto& GetSparseEntityPages() { return sparseEntityPages; }
	auto& GetDenseEntities() { return denseEntities; }
	auto& GetDenseComponents() { return denseComponents; }

	template<uint32_t... index>
	bool IsBitSet(Entity entity);
	template<uint32_t... index>
	void SetBit(Entity entity);
	template<uint32_t... index>
	void ResetBit(Entity entity);
	virtual std::bitset<8>& GetBitset(Entity entity) override;
protected:
	void RegisterEntity(Entity entity);
	std::pair<Index, Index> GetPageIndices(Entity entity);
private:
	std::vector<std::vector<Entity>> sparseEntityPages;
	std::vector<Entity> denseEntities;
	std::vector<T> denseComponents;
	std::vector<std::bitset<8>> denseBitsets;
};

#include "Pool.inl"
