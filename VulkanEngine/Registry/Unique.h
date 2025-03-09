#pragma once
#include <cstdint>

using UniqueID = uint32_t;

class Unique
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