#pragma once
#include "DataPool.h"
#include "BitsetPool.h"

template<typename T>
class ComponentPool : public DataPool<T>, public BitsetPool
{
public:
	virtual ~ComponentPool<T>() = default;
	virtual void Remove(uint32_t index) override;
	virtual void Add(uint32_t index) override;
	virtual void Add(uint32_t index, const T& data);
};

template<typename T>
inline void ComponentPool<T>::Remove(uint32_t index)
{
	auto context = GetRemoveContext(index);
	if (context.has_value())
	{
		BitsetPool::RemoveBitset(context.value());
		DataPool<T>::RemoveData(context.value());
		SparseSet::RemoveIndex(context.value());
	}
}

template<typename T>
inline void ComponentPool<T>::Add(uint32_t index)
{
	static_assert(std::is_default_constructible<T>::value, "DataPool<T>::AddData T must be default constructible.");
	Add(index, T{});
}

template<typename T>
inline void ComponentPool<T>::Add(uint32_t index, const T& data)
{
	auto context = GetAddContext(index);
	SparseSet::RegisterIndex(index);

	if (context.has_value())
	{
		BitsetPool::AddBitset();
		DataPool<T>::AddData(data);
		SparseSet::AddIndex(context.value());
	}
}
