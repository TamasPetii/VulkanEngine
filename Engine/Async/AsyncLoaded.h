#pragma once
#include "Engine/EngineApi.h"

#include <atomic>

enum class LoadState : uint8_t {
    NotLoaded,      
    CpuLoaded,
    GpuUploaded,
    Ready,
    Failed
};

class ENGINE_API AsyncLoaded
{
public:
    std::atomic<LoadState> state = LoadState::NotLoaded;
};

