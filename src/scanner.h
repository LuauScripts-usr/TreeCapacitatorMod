#pragma once
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <link.h>
#include <android/log.h>

#define LOG_TAG "TreeCap_Scan"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace Scanner {
    struct ModuleInfo {
        uintptr_t base;
        size_t size;
    };

    // Finds the base address and size of libminecraftpe.so in memory
    inline ModuleInfo GetModuleInfo(const char* moduleName) {
        ModuleInfo info = {0, 0};
        dl_iterate_phdr([](struct dl_phdr_info *info, size_t size, void *data) {
            if (info->dlpi_name && strstr(info->dlpi_name, moduleName)) {
                auto *modInfo = static_cast<ModuleInfo*>(data);
                modInfo->base = info->dlpi_addr;
                for (int i = 0; i < info->dlpi_phnum; i++) {
                    if (info->dlpi_phdr[i].p_type == PT_LOAD) {
                        size_t end = info->dlpi_phdr[i].p_vaddr + info->dlpi_phdr[i].p_memsz;
                        if (end > modInfo->size) modInfo->size = end;
                    }
                }
                return 1; 
            }
            return 0;
        }, &info);
        return info;
    }

    // Scans memory for a specific byte pattern
    inline uintptr_t FindPattern(uintptr_t start, size_t length, const char* pattern, const char* mask) {
        size_t patternLen = strlen(mask);
        for (size_t i = 0; i < length - patternLen; i++) {
            bool found = true;
            for (size_t j = 0; j < patternLen; j++) {
                if (mask[j] != '?' && pattern[j] != *((char*)(start + i + j))) {
                    found = false;
                    break;
                }
            }
            if (found) return start + i;
        }
        return 0;
    }
}
