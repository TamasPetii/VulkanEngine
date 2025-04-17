#pragma once
#include "Engine/EngineApi.h"
#include <vector>
#include <optional>

constexpr uint32_t L = 4; // For PAGE_SIZE = 16
constexpr uint32_t PAGE_SIZE = 1 << L;
constexpr uint32_t NULL_INDEX = std::numeric_limits<uint32_t>::max();

class ENGINE_API SparseSet
{
public:
	virtual ~SparseSet() = default;
	void RegisterIndex(uint32_t index);
	bool ContainsIndex(uint32_t index);
	virtual void Remove(uint32_t index);
	virtual void Add(uint32_t index);
public:
	uint32_t GetDenseIndex(uint32_t index);
	const auto& GetSparseIndices() { return sparseIndices; }
	const auto& GetDenseIndices() { return denseIndices; }
	const auto& GetDenseSize() { return denseIndices.size(); }
protected:
	struct RemoveContext
	{
		std::pair<uint32_t, uint32_t> deleteIndices;
		uint32_t deleteDenseIndex;
		std::pair<uint32_t, uint32_t> swapIndices;
		uint32_t swapDenseIndex;
	};
	struct AddContext
	{
		std::pair<uint32_t, uint32_t> pageIndices;
		uint32_t index;
	};
protected:
	void AddIndex(const AddContext& context);
	void RemoveIndex(const RemoveContext& context);
	inline uint32_t GetPageIndex(uint32_t index);
	inline uint32_t GetPageOffset(uint32_t index);
	inline std::pair<uint32_t, uint32_t> GetPageIndices(uint32_t index);
	inline std::optional<RemoveContext> GetRemoveContext(uint32_t index);
	inline std::optional<AddContext> GetAddContext(uint32_t index);
protected:
	std::vector<std::vector<uint32_t>> sparseIndices;
	std::vector<uint32_t> denseIndices;
};