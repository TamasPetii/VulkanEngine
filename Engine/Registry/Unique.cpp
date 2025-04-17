#include "Unique.h"

std::unordered_map<std::type_index, uint32_t> Unique::typeIndices;

UniqueID Unique::identifier()
{
    static UniqueID value = 0;
    return value++;
}