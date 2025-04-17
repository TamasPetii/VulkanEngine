#pragma once
#include <cstdint>
#include <limits>

using Entity = uint32_t;
using Parent = Entity;
using Child = Entity;
using Index = Entity;
constexpr Entity NULL_ENTITY = std::numeric_limits<Entity>::max();