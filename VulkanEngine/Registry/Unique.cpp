#include "Unique.h"

UniqueID Unique::identifier()
{
    static UniqueID value = 0;
    return value++;
}