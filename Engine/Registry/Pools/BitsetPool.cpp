#include "BitsetPool.h"
#include <stdexcept>

void BitsetPool::Remove(uint32_t index)
{
	auto context = GetRemoveContext(index);
	if (context.has_value())
	{
		BitsetPool::RemoveBitset(context.value());
		SparseSet::RemoveIndex(context.value());
	}
}

void BitsetPool::Add(uint32_t index)
{
	auto context = GetAddContext(index);
	SparseSet::RegisterIndex(index);

	if (context.has_value())
	{
		BitsetPool::AddBitset();
		SparseSet::AddIndex(context.value());
	}
}

BitsetFlag& BitsetPool::GetBitset(uint32_t index)
{
	[[unlikely]]
	if (!ContainsIndex(index))
		throw std::runtime_error("Invalid index for accesing bitset component!");

	return denseBitsets[sparseIndices[GetPageIndex(index)][GetPageOffset(index)]];
}

void BitsetPool::AddBitset()
{
	denseBitsets.push_back(BitsetFlag{});
	denseBitsets.back().set(REGENERATE_BIT, true);
	denseBitsets.back().set(UPDATE_BIT, true);
	denseBitsets.back().set(INDEX_CHANGED_BIT, true);
}

void BitsetPool::RemoveBitset(const RemoveContext& context)
{
	SetBit<REGENERATE_BIT, UPDATE_BIT, INDEX_CHANGED_BIT>(context.swapDenseIndex);
	std::swap(denseBitsets[context.deleteDenseIndex], denseBitsets[context.swapDenseIndex]);
	denseBitsets.pop_back();
}
