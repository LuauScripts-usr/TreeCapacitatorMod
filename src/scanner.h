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

    // Finds the base address and size of a module in memory
    inline ModuleInfo GetModuleInfo(const char* moduleName) {
        ModuleInfo info = {0, 0};
        
        struct CallbackData {
            const char* name;
            ModuleInfo* result;
        };
        
        CallbackData data = {moduleName, &info};
        
        dl_iterate_phdr([](struct dl_phdr_info *phdr_info, size_t size, void *userdata) -> int {
            auto* cbdata = static_cast<CallbackData*>(userdata);
            if (phdr_info->dlpi_name && strstr(phdr_info->dlpi_name, cbdata->name)) {
                cbdata->result->base = phdr_info->dlpi_addr;
                for (int i = 0; i < phdr_info->dlpi_phnum; i++) {
                    if (phdr_info->dlpi_phdr[i].p_type == PT_LOAD) {
                        size_t end = phdr_info->dlpi_phdr[i].p_vaddr + phdr_info->dlpi_phdr[i].p_memsz;
                        if (end > cbdata->result->size) cbdata->result->size = end;
                    }
                }
                return 1;
            }
            return 0;
        }, &data);
        
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
