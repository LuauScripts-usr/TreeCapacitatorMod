#include "tree_capacitor.h"
#include "offsets_1_26_33.h"
#include <android/log.h>
#include <unordered_map>
#include <vector>
#include <cmath>

#define LOG_TAG "TreeCap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace TreeCapacitor {

    static std::unordered_map<uint64_t, CapacitorNode> g_Nodes;
    static int g_TickCounter = 0;

    static inline uint64_t Pack(int x, int y, int z) {
        return (uint64_t)(uint32_t)x << 40 |
               (uint64_t)(uint32_t)z << 16 |
               (uint64_t)(uint16_t)y;
    }

    static inline bool IsLog(int blockId) {
        return blockId == BLOCK_ID_OAK_LOG  ||
               blockId == BLOCK_ID_OAK_LOG2 ||
               blockId == BLOCK_ID_MANGROVE_LOG ||
               blockId == BLOCK_ID_CHERRY_LOG ||
               blockId == BLOCK_ID_CRIMSON_STEM ||
               blockId == BLOCK_ID_WARPED_STEM;
    }

    void OnTick(void* level, bool isStorm) {
        // Throttle: process every 10th tick
        if (++g_TickCounter % 10 != 0) return;

        for (auto& [key, node] : g_Nodes) {
            if (isStorm && node.charge < MAX_CHARGE) {
                node.charge += CHARGE_RATE;
                if (node.charge > MAX_CHARGE) node.charge = MAX_CHARGE;
            }
            if (node.charge > 0) {
                node.charge -= DISCHARGE_RATE;
                if (node.charge < 0) node.charge = 0;
            }
        }
    }

    void OnBlockPlace(int blockId, int x, int y, int z) {
        if (!IsLog(blockId)) return;

        uint64_t key = Pack(x, y, z);
        if (g_Nodes.find(key) == g_Nodes.end()) {
            g_Nodes[key] = {0, x, y, z};
            LOGI("Capacitor node registered: %d %d %d", x, y, z);
        }
    }

    void Init() {
        g_Nodes.reserve(256);
        LOGI("TreeCapacitor init complete — MC 1.26.33.1");
    }

    void Shutdown() {
        g_Nodes.clear();
    }
}
