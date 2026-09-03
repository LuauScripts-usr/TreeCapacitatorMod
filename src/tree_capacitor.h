#pragma once

namespace TreeCapacitor {
    static constexpr int MAX_CHARGE     = 10000;
    static constexpr int CHARGE_RATE    = 5;
    static constexpr int DISCHARGE_RATE = 2;

    struct CapacitorNode {
        int charge = 0;
        int x, y, z;
    };

    void Init();
    void Shutdown();
    void OnTick(void* level, bool isStorm);
    void OnBlockPlace(int blockId, int x, int y, int z);
}
