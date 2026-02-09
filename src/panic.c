#include <execinfo.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "panic.h"

static uintptr_t get_exe_base(void) {
    FILE* maps = fopen("/proc/self/maps", "r");
    if (!maps) return 0;

    char line[512];
    uintptr_t base = 0;

    while (fgets(line, sizeof(line), maps)) {
        unsigned long start;
        char path[256] = {0};
        char perms[5];
        if (sscanf(line, "%lx-%*lx %*s %*s %*s %*s %255s", &start, path) == 2) {
            if (strstr(path, "/proc/self/exe") && strchr(perms, 'x')) {
                base = (uintptr_t)start;
                break;
            }
        }
    }

    fclose(maps);
    return base;
}

void print_stacktrace(void) {
    void *frames[64];
    int n = backtrace(frames, 64);
    printf("%ld\n", get_exe_base());
    uintptr_t base = get_exe_base();

    fprintf(stderr, "\n\e[0;33mTraceback (most recent call last):\n");
    for (int i = n - 4; i >= 2; i--) {
        uintptr_t addr = (uintptr_t)frames[i];
        uintptr_t offset = base ? (addr - base - 1) : (addr - 1);
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
                 "addr2line -f -p -e /proc/%d/exe %p | awk '{printf(\"  %%s\\n\", $0)}'",
                 getpid(), (void*)offset);
        system(cmd);
    }
    fprintf(stderr, "\e[0m\n");
}

void panic_impl(
    const char* file,
    int line,
    const char* func,
    const char* fmt,
    ...
) {
    print_stacktrace();

    fprintf(stderr, "\033\e[0;31mPANIC at %s:%d (%s): ", file, line, func);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\e[0m\n");

    exit(1);
}

