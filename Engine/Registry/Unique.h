#pragma once
#include "../EngineApi.h"
#include <cstdint>

using UniqueID = uint32_t;

class ENGINE_API Unique
{
public:
    template<typename T>
    static UniqueID typeID();
private:
    static UniqueID identifier();
};

template<typename T>
UniqueID Unique::typeID()
{
    static const UniqueID value = identifier();
    return value;
}