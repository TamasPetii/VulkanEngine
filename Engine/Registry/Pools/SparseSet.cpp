#include "SparseSet.h"
#include <algorithm>

void SparseSet::RegisterIndex(uint32_t index)
{
	uint32_t pageIndex = GetPageIndex(index);
	uint32_t pageOffset = GetPageOffset(index);

	if (pageIndex >= sparseIndices.size())
		sparseIndices.resize(GetPageIndex(index) + 1);

	if (sparseIndices[pageIndex].empty())
		sparseIndices[pageIndex].resize(PAGE_SIZE, NULL_INDEX);
}

void SparseSet::Remove(uint32_t index)
{
	auto context = GetRemoveContext(index);

	if (context.has_value())
		SparseSet::RemoveIndex(context.value());
}

void SparseSet::Add(uint32_t index)
{
	auto context = GetAddContext(index);
	SparseSet::RegisterIndex(index);

	if (context.has_value())
		SparseSet::AddIndex(context.value());
}

void SparseSet::AddIndex(const AddContext& context)
{
	sparseIndices[context.pageIndices.first][context.pageIndices.second] = denseIndices.size();
	denseIndices.push_back(context.index);
}

void SparseSet::RemoveIndex(const RemoveContext& context)
{
	std::swap(denseIndices[context.deleteDenseIndex], denseIndices[context.swapDenseIndex]);
	denseIndices.pop_back();
	sparseIndices[context.swapIndices.first][context.swapIndices.second] = context.deleteDenseIndex;
	sparseIndices[context.deleteIndices.first][context.deleteIndices.second] = NULL_INDEX;
}

inline std::optional<SparseSet::RemoveContext> SparseSet::GetRemoveContext(uint32_t index)
{
	if (!ContainsIndex(index))
		return std::nullopt;

	auto deleteIndices = GetPageIndices(index);
	auto deleteDenseIndex = sparseIndices[deleteIndices.first][deleteIndices.second];

	auto swapDenseIndex = static_cast<uint32_t>(denseIndices.size() - 1);
	auto swapIndices = GetPageIndices(denseIndices[swapDenseIndex]);

	return RemoveContext{ deleteIndices, deleteDenseIndex, swapIndices, swapDenseIndex };
}

inline std::optional<SparseSet::AddContext> SparseSet::GetAddContext(uint32_t index)
{
	if (ContainsIndex(index))
		return std::nullopt;

	return AddContext{ GetPageIndices(index), index };
}

bool SparseSet::ContainsIndex(uint32_t index)
{
	uint32_t pageIndex = GetPageIndex(index);
	uint32_t pageOffset = GetPageOffset(index);

	return pageIndex < sparseIndices.size() &&
		   !sparseIndices[pageIndex].empty() &&
		   sparseIndices[pageIndex][pageOffset] != NULL_INDEX;
}

uint32_t SparseSet::GetDenseIndex(uint32_t index)
{
	if (!ContainsIndex(index))
		return NULL_INDEX;

	return sparseIndices[GetPageIndex(index)][GetPageOffset(index)];
}

inline uint32_t SparseSet::GetPageIndex(uint32_t index)
{
	return index >> L;
}

inline uint32_t SparseSet::GetPageOffset(uint32_t index)
{
	return index - (GetPageIndex(index) << L);
}

inline std::pair<uint32_t, uint32_t> SparseSet::GetPageIndices(uint32_t index)
{
	return { GetPageIndex(index), GetPageOffset(index) };
}