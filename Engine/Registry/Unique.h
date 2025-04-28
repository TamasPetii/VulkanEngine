#pragma once
#include "../EngineApi.h"
#include <cstdint>
#include <typeindex>
#include <unordered_map>

using UniqueID = uint32_t;

class ENGINE_API Unique
{
public:
    template<typename T>
    static UniqueID typeIndex();
    template<typename T>
    static std::type_index typeID();
private:
    static UniqueID identifier();
    static std::unordered_map<std::type_index, uint32_t> typeIndices;
};

template<typename T>
UniqueID Unique::typeIndex()
{
    [[unlikely]]
    if (typeIndices.find(typeid(T)) == typeIndices.end())
    {
        static const UniqueID value = identifier();
        typeIndices[typeid(T)] = value;
    }

    return typeIndices[typeid(T)];
}

template<typename T>
std::type_index Unique::typeID()
{
    return typeid(T);
}