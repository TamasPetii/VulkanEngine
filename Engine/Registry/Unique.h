#pragma once
#include "../EngineApi.h"
#include <cstdint>
#include <typeindex>

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
};

template<typename T>
UniqueID Unique::typeIndex()
{
    static const UniqueID value = identifier();
    return value;
}

template<typename T>
std::type_index Unique::typeID()
{
    return typeid(T);
}