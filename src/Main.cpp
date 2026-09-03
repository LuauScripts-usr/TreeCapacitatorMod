#include <jni.h>
#include <dlfcn.h>
#include <cstdint>
#include <android/log.h>
#include "dobby.h"
#include "tree_capacitor.h"
#include "Offsets_1_26_33.h"

#define LOG_TAG "TreeCap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static void (*orig_LevelTick)(void*)              = nullptr;
static void (*orig_SetBlock)(void*, void*, void*) = nullptr;

static void Hook_LevelTick(void* level) {
    int weatherType = *reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(level) + OFF_LEVEL_WEATHER_TYPE);
    TreeCapacitor::OnTick(level, weatherType >= 1);
    if (orig_LevelTick) orig_LevelTick(level);
}

static void Hook_SetBlock(void* src, void* pos, void* blk) {
    int blockId = *reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(blk) + OFF_BLOCK_ID);
    int x = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pos) + OFF_BLOCKPOS_X);
    int y = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pos) + OFF_BLOCKPOS_Y);
    int z = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pos) + OFF_BLOCKPOS_Z);
    TreeCapacitor::OnBlockPlace(blockId, x, y, z);
    if (orig_SetBlock) orig_SetBlock(src, pos, blk);
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("TreeCapacitor loading (MC 1.26.33.1) ...");

    if (OFFSETS_UNSET) {
        LOGE("STUB MODE: offsets are 0 — hooks skipped. Fill Offsets_1_26_33.h.");
        TreeCapacitor::Init();
        return JNI_VERSION_1_6;
    }

    void* mcLib = dlopen("libminecraftpe.so", RTLD_NOW | RTLD_GLOBAL);
    if (!mcLib) {
        LOGE("FATAL: cannot open libminecraftpe.so");
        return JNI_VERSION_1_6;
    }

    uintptr_t base = reinterpret_cast<uintptr_t>(mcLib);

    int rc1 = DobbyHook(reinterpret_cast<void*>(base + OFF_LEVEL_TICK),
                        reinterpret_cast<void*>(Hook_LevelTick),
                        reinterpret_cast<void**>(&orig_LevelTick));
    int rc2 = DobbyHook(reinterpret_cast<void*>(base + OFF_SET_BLOCK),
                        reinterpret_cast<void*>(Hook_SetBlock),
                        reinterpret_cast<void**>(&orig_SetBlock));

    if (rc1 != 0) LOGE("Failed to hook Level::tick (rc=%d)", rc1);
    if (rc2 != 0) LOGE("Failed to hook BlockSource::setBlock (rc=%d)", rc2);

    TreeCapacitor::Init();
    LOGI("TreeCapacitor hooks active");
    return JNI_VERSION_1_6;
}
