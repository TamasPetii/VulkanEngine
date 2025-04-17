#pragma once
#include "SparseSet.h"
#include <bitset>

constexpr uint32_t REGENERATE_BIT = 0;
constexpr uint32_t UPDATE_BIT = 1;
constexpr uint32_t CHANGED_BIT = 2;
constexpr uint32_t INDEX_CHANGED_BIT = 3;

using BitsetFlag = std::bitset<8>;

class BitsetPool : public virtual SparseSet
{
public:
	virtual ~BitsetPool() = default;
	virtual void Remove(uint32_t index) override;
	virtual void Add(uint32_t index) override;
	template<uint32_t... bitIndex>
	bool IsBitSet(uint32_t index);
	template<uint32_t... bitIndex>
	void SetBit(uint32_t index);
	template<uint32_t... bitIndex>
	void ResetBit(uint32_t index);
	virtual BitsetFlag* GetBitset(uint32_t index);
	const auto& GetDenseBitsets() { return denseBitsets; }
protected:
	void AddBitset();
	void RemoveBitset(const RemoveContext& context);
protected:
	std::vector<BitsetFlag> denseBitsets;
};

template<uint32_t ...bitIndex>
inline bool BitsetPool::IsBitSet(uint32_t index)
{
	auto bitset = GetBitset(index);
	return (bitset->test(bitIndex) && ...);
}

template<uint32_t ...bitIndex>
inline void BitsetPool::SetBit(uint32_t index)
{
	auto bitset = GetBitset(index);
	(bitset->set(bitIndex, true), ...);
}

template<uint32_t ...bitIndex>
inline void BitsetPool::ResetBit(uint32_t index)
{
	auto bitset = GetBitset(index);
	(bitset->set(bitIndex, false), ...);
}
