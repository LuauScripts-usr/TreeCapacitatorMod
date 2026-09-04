#include <jni.h>
#include <dlfcn.h>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <android/log.h>
#include "tree_capacitor.h"
#include "scanner.h"

#define LOG_TAG "TreeCap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static void (*orig_LevelTick)(void*)              = nullptr;
static void (*orig_SetBlock)(void*, void*, void*) = nullptr;

// Minimal ARM64 inline hook
static void hook_arm64(void* target, void* hook_func, void** backup) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    void* page = (void*)((uintptr_t)target & ~(page_size - 1));
    mprotect(page, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC);

    if (backup) {
        *backup = malloc(16);
        memcpy(*backup, target, 16);
    }

    uint32_t* code = (uint32_t*)target;
    int64_t offset = ((int64_t)hook_func - (int64_t)target) >> 12;
    
    code[0] = 0x90000010 | ((offset & 0x1FFFFF) << 5); 
    code[1] = 0x91000210 | ((offset & 0xFFF) << 10);   
    code[2] = 0xD61F0200;                              
    code[3] = 0xD503201F;                              

    __builtin___clear_cache((char*)target, (char*)target + 16);
}

static void Hook_LevelTick(void* level) {
    // Weather offset is typically around 0x5A8 in Level struct for recent versions
    // If this crashes, the struct layout has changed.
    int weatherType = *reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(level) + 0x5A8); 
    TreeCapacitor::OnTick(level, weatherType >= 1);
    if (orig_LevelTick) orig_LevelTick(level);
}

static void Hook_SetBlock(void* src, void* pos, void* blk) {
    int blockId = *reinterpret_cast<int*>(
        reinterpret_cast<uintptr_t>(blk) + 0x08); // Block ID offset
    int x = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pos) + 0x00);
    int y = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pos) + 0x04);
    int z = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pos) + 0x08);
    TreeCapacitor::OnBlockPlace(blockId, x, y, z);
    if (orig_SetBlock) orig_SetBlock(src, pos, blk);
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("TreeCapacitor loading (MC 1.26.33.1) with Pattern Scanner...");

    auto modInfo = Scanner::GetModuleInfo("libminecraftpe.so");
    if (modInfo.base == 0) {
        LOGE("FATAL: Could not find libminecraftpe.so in memory");
        return JNI_VERSION_1_6;
    }
    LOGI("Found libminecraftpe.so at base: 0x%lx", modInfo.base);

    // Pattern for Level::tick (Common ARM64 prologue + specific call)
    // STP X29, X30, [SP, #-0x30]! ... 
    const char* tickPattern = "\xFD\x7B\xBF\xA9\xFD\x03\x00\x91\xF3\x03\x00\xAA\xE0\x03\x1F\xAA";
    const char* tickMask    = "xxxxxxxxxxxxxxxx";
    
    uintptr_t tickAddr = Scanner::FindPattern(modInfo.base, modInfo.size, tickPattern, tickMask);
    if (tickAddr) {
        LOGI("Found Level::tick at 0x%lx", tickAddr);
        hook_arm64((void*)tickAddr, (void*)Hook_LevelTick, (void**)&orig_LevelTick);
    } else {
        LOGE("Failed to find Level::tick pattern");
    }

    // Pattern for BlockSource::setBlock
    const char* setBlockPattern = "\xFD\x7B\xBF\xA9\xFD\x03\x00\x91\xF4\x03\x00\xAA\xE0\x03\x1F\xAA";
    const char* setBlockMask    = "xxxxxxxxxxxxxxxx";

    uintptr_t setBlockAddr = Scanner::FindPattern(modInfo.base, modInfo.size, setBlockPattern, setBlockMask);
    if (setBlockAddr) {
        LOGI("Found BlockSource::setBlock at 0x%lx", setBlockAddr);
        hook_arm64((void*)setBlockAddr, (void*)Hook_SetBlock, (void**)&orig_SetBlock);
    } else {
        LOGE("Failed to find BlockSource::setBlock pattern");
    }

    TreeCapacitor::Init();
    LOGI("TreeCapacitor initialization complete");
    return JNI_VERSION_1_6;
}
