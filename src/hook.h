#pragma once
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

static void hook_arm64(void* target, void* hook_func, void** backup) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    void* page = (void*)((uintptr_t)target & ~(page_size - 1));
    
    // Make memory writable and executable
    mprotect(page, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC);

    // Save original bytes for backup
    if (backup) {
        *backup = malloc(16);
        memcpy(*backup, target, 16);
    }

    // Write ARM64 branch: ADRP X16, hook_func; BR X16
    // This is a simplified trampoline for short distances.
    uint32_t* code = (uint32_t*)target;
    int64_t offset = ((int64_t)hook_func - (int64_t)target) >> 12;
    
    code[0] = 0x90000010 | ((offset & 0x1FFFFF) << 5); // ADRP X16, hook_func@PAGE
    code[1] = 0x91000210 | ((offset & 0xFFF) << 10);   // ADD X16, X16, hook_func@PAGEOFF
    code[2] = 0xD61F0200;                              // BR X16
    code[3] = 0xD503201F;                              // NOP (padding)

    // Flush instruction cache
    __builtin___clear_cache((char*)target, (char*)target + 16);
}
