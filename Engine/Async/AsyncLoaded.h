#pragma once
#include "Engine/EngineApi.h"

#include <atomic>

enum class LoadState : uint8_t {
    NotLoaded,      
    Loaded,        
    Ready,
    Failed
};

class ENGINE_API AsyncLoaded
{
public:
    std::atomic<LoadState> state = LoadState::NotLoaded;
};

