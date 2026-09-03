#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

namespace TreeCapacitor {
    static constexpr int    MAX_CHARGE       = 10000;
    static constexpr int    CHARGE_RATE      = 5;
    static constexpr int    DISCHARGE_RATE   = 2;
    static constexpr float  SCAN_RADIUS      = 6.0f;

    struct CapacitorNode {
        int charge = 0;
        int x, y, z;
    };

    void Init();
    void Shutdown();
    void OnTick(void* levelPtr);          // hooked into Level::tick
    void OnBlockPlace(void* blockSourcePtr, void* posPtr, void* blockPtr);
}
