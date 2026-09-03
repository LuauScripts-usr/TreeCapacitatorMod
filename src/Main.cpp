#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include "dobby.h"
#include "tree_capacitor.h"

#define LOG_TAG "TreeCap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ─── Original function pointers (filled by Dobby) ────────────
static void (*orig_LevelTick)(void*) = nullptr;
static void (*orig_SetBlock)(void*, void*, void*) = nullptr;

// ─── Hook trampolines ────────────────────────────────────────
static void Hooked_LevelTick(void* level) {
    TreeCapacitor::OnTick(level);
    if (orig_LevelTick) orig_LevelTick(level);
}

static void Hooked_SetBlock(void* src, void* pos, void* blk) {
    TreeCapacitor::OnBlockPlace(src, pos, blk);
    if (orig_SetBlock) orig_SetBlock(src, pos, blk);
}

// ─── Entry point called by Levi Launcher ─────────────────────
// Levi loads the .so and calls JNI_OnLoad or a custom entry.
// Adjust the exported symbol name to match what Levi expects.
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("=== TreeCapacitor .so loaded ===");

    // Resolve libminecraftpe.so base
    void* mcLib = dlopen("libminecraftpe.so", RTLD_NOW);
    if (!mcLib) {
        LOGE("Failed to open libminecraftpe.so");
        return JNI_VERSION_1_6;
    }

    // ──────────────────────────────────────────────────────────
    // CRITICAL: Replace these offsets with the real ones for
    // YOUR target Minecraft Bedrock version.
    // Use Ghidra or IDA Pro on the extracted libminecraftpe.so
    // to find:
    //   • Level::tick()          → e.g. 0x1A3F4C0
    //   • BlockSource::setBlock() → e.g. 0x0E82B14
    // ──────────────────────────────────────────────────────────
    uintptr_t base = reinterpret_cast<uintptr_t>(mcLib);

    uintptr_t levelTickAddr   = base + 0x1A3F4C0;  // ← FIND REAL OFFSET
    uintptr_t setBlockAddr    = base + 0x0E82B14;  // ← FIND REAL OFFSET

    // Install hooks via Dobby
    DobbyHook(
        reinterpret_cast<void*>(levelTickAddr),
        reinterpret_cast<void*>(Hooked_LevelTick),
        reinterpret_cast<void**>(&orig_LevelTick)
    );

    DobbyHook(
        reinterpret_cast<void*>(setBlockAddr),
        reinterpret_cast<void*>(Hooked_SetBlock),
        reinterpret_cast<void**>(&orig_SetBlock)
    );

    TreeCapacitor::Init();
    LOGI("Hooks installed successfully");

    return JNI_VERSION_1_6;
}
