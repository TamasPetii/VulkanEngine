#pragma once
#include "Pool.h"
#include "Unique.h"

template<typename T>
inline Pool<T>::Pool()
{
	uniqueIndex = Unique::typeID<T>();
}

template<typename T>
inline void Pool<T>::AddEntity(Entity entity)
{
	RegisterEntity(entity);
}

template<typename T>
inline void Pool<T>::RemoveEntity(Entity entity)
{
	RemoveComponent(entity);
}

template<typename T>
inline void Pool<T>::RegisterEntity(Entity entity)
{
	auto indices = GetPageIndices(entity);

	if (indices.first >= sparseEntityPages.size())
		sparseEntityPages.resize(indices.first + 1);

	if (sparseEntityPages[indices.first].empty())
		sparseEntityPages[indices.first].resize(PAGE_SIZE, NULL_ENTITY);
}

template<typename T>
inline bool Pool<T>::HasComponent(Entity entity)
{
	auto indices = GetPageIndices(entity);
	return indices.first < sparseEntityPages.size() &&
		   !sparseEntityPages[indices.first].empty() &&
		   sparseEntityPages[indices.first][indices.second] != NULL_ENTITY;
}

template<typename T>
inline T* Pool<T>::GetComponent(Entity entity)
{
	if (!HasComponent(entity))
		return nullptr;

	auto indices = GetPageIndices(entity);
	return &denseComponents[sparseEntityPages[indices.first][indices.second]];
}

template<typename T>
inline void Pool<T>::AddComponent(Entity entity)
{
	AddComponent(entity, T{});
}

template<typename T>
inline void Pool<T>::AddComponent(Entity entity, const T& component)
{
	static_assert(std::is_default_constructible<T>::value, "T must be default constructible.");

	RegisterEntity(entity);

	if (HasComponent(entity))
		return;

	auto indices = GetPageIndices(entity);
	sparseEntityPages[indices.first][indices.second] = denseEntities.size();
	denseEntities.push_back(entity);
	denseComponents.emplace_back(component);
}

template<typename T>
inline void Pool<T>::RemoveComponent(Entity entity)
{
	if (!HasComponent(entity))
		return;

	auto deleteIndices = GetPageIndices(entity);
	auto deleteDenseIndex = sparseEntityPages[deleteIndices.first][deleteIndices.second];

	auto swapDenseIndex = denseEntities.size() - 1;
	auto swapIndices = GetPageIndices(denseEntities[swapDenseIndex]);

	//Set the swapped entity sparse index to the new dense location (Which is the delete index of the current entity)
	sparseEntityPages[swapIndices.first][swapIndices.second] = deleteDenseIndex;

	//Swap the dense indices and components between the last entity, and the current deleted entity
	std::swap(denseEntities[deleteDenseIndex], denseEntities[swapDenseIndex]);
	std::swap(denseComponents[deleteDenseIndex], denseComponents[swapDenseIndex]);

	//Delete entity's data from back
	denseEntities.pop_back();
	denseComponents.pop_back();

	//Set the deleted entity sparse index to NULL
	sparseEntityPages[deleteIndices.first][deleteIndices.second] = NULL_ENTITY;
}

template<typename T>
inline std::pair<Index, Index> Pool<T>::GetPageIndices(Entity entity)
{
	return std::make_pair(entity / PAGE_SIZE, entity % PAGE_SIZE);
}
