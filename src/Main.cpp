#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include "dobby.h"
#include "tree_capacitor.h"
#include "offsets_1_26_33.h"

#define LOG_TAG "TreeCap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static uintptr_t g_Base = 0;

// ─── Originals ─────────────────────────────────────────────
static void (*orig_LevelTick)(void*)             = nullptr;
static void (*orig_SetBlock)(void*,void*,void*)  = nullptr;

// ─── Hooked Level::tick ────────────────────────────────────
static void Hook_LevelTick(void* level) {
    // Read weather
    int weatherType = *reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(level) + OFF_LEVEL_WEATHER_TYPE
    );
    // 0=clear  1=rain  2=thunder
    bool isStorm = (weatherType >= 1);

    TreeCapacitor::OnTick(level, isStorm);

    if (orig_LevelTick) orig_LevelTick(level);
}

// ─── Hooked BlockSource::setBlock ──────────────────────────
static void Hook_SetBlock(void* blockSource, void* blockPos, void* block) {
    // Read block ID
    int blockId = *reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(block) + OFF_BLOCK_ID
    );

    // Read position
    int x = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(blockPos) + OFF_BLOCKPOS_X);
    int y = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(blockPos) + OFF_BLOCKPOS_Y);
    int z = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(blockPos) + OFF_BLOCKPOS_Z);

    TreeCapacitor::OnBlockPlace(blockId, x, y, z);

    if (orig_SetBlock) orig_SetBlock(blockSource, blockPos, block);
}

// ─── Entry ─────────────────────────────────────────────────
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("TreeCapacitor loading for MC 1.26.33.1 ...");

    void* mcLib = dlopen("libminecraftpe.so", RTLD_NOW | RTLD_GLOBAL);
    if (!mcLib) {
        LOGE("FATAL: cannot open libminecraftpe.so");
        return JNI_VERSION_1_6;
    }

    g_Base = reinterpret_cast<uintptr_t>(mcLib);

    // Install hooks
    int rc1 = DobbyHook(
        reinterpret_cast<void*>(g_Base + OFF_LEVEL_TICK),
        reinterpret_cast<void*>(Hook_LevelTick),
        reinterpret_cast<void**>(&orig_LevelTick)
    );

    int rc2 = DobbyHook(
        reinterpret_cast<void*>(g_Base + OFF_SET_BLOCK),
        reinterpret_cast<void*>(Hook_SetBlock),
        reinterpret_cast<void**>(&orig_SetBlock)
    );

    if (rc1 != 0) LOGE("Failed to hook Level::tick");
    if (rc2 != 0) LOGE("Failed to hook BlockSource::setBlock");

    TreeCapacitor::Init();
    LOGI("TreeCapacitor hooks active");

    return JNI_VERSION_1_6;
}
