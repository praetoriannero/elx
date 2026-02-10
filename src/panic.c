#include <execinfo.h>
// #include <backtrace.h>
// #include <libunwind.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "panic.h"

// /* Error callback */
// static void error_callback(void *data, const char *msg, int errnum) {
//     (void)data;
//     fprintf(stderr, "libbacktrace error: %s (%d)\n", msg, errnum);
// }
//
// /* Called once per frame */
// static int full_callback(
//     void *data,
//     uintptr_t pc,
//     const char *filename,
//     int lineno,
//     const char *function
// ) {
//     (void)data;
//     (void)pc;
//
//     fprintf(stderr, "  %s:%d: %s\n",
//             filename ? filename : "??",
//             lineno,
//             function ? function : "??");
//
//     return 0; // continue
// }
//
// void print_stacktrace3(void) {
//     struct backtrace_state *state;
//
//     state = backtrace_create_state(
//         NULL,              // use current executable
//         0,                 // not thread-safe
//         error_callback,
//         NULL
//     );
//
//     fprintf(stderr, "stack trace:\n");
//
//     backtrace_full(
//         state,
//         0,                 // skip this function
//         full_callback,
//         error_callback,
//         NULL
//     );
// }
//
// void print_stacktrace2(void) {
//     unw_cursor_t cursor;
//     unw_context_t context;
//
//     // Capture machine state
//     unw_getcontext(&context);
//     unw_init_local(&cursor, &context);
//
//     fprintf(stderr, "stack trace:\n");
//
//     while (unw_step(&cursor) > 0) {
//         unw_word_t ip, sp;
//         char name[256];
//
//         unw_get_reg(&cursor, UNW_REG_IP, &ip);
//         unw_get_reg(&cursor, UNW_REG_SP, &sp);
//
//         if (ip == 0)
//             break;
//
//         unw_word_t offset = 0;
//         int ret = unw_get_proc_name(
//             &cursor, name, sizeof(name), &offset);
//
//         if (ret == 0) {
//             fprintf(stderr, "  %s + 0x%lx\n",
//                     name, (long)offset);
//         } else {
//             fprintf(stderr, "  0x%lx\n", (long)ip);
//         }
//     }
// }

static uintptr_t get_exe_base(void) {
    FILE* maps = fopen("/proc/self/maps", "r");
    if (!maps)
        return 0;

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
    void* frames[64];
    int n = backtrace(frames, 64);
    printf("%ld\n", get_exe_base());
    uintptr_t base = get_exe_base();

    fprintf(stderr, "\n\e[0;33mTraceback (most recent call last):\n");
    for (int i = n - 4; i >= 2; i--) {
        uintptr_t addr = (uintptr_t)frames[i];
        uintptr_t offset = base ? (addr - base - 1) : (addr - 1);
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
                 "addr2line -f -p -e /proc/%d/exe %p | awk '{printf(\"  "
                 "%%s\\n\", $0)}'",
                 getpid(), (void*)offset);
        system(cmd);
    }
    fprintf(stderr, "\e[0m\n");
}

void panic_impl(const char* file, int line, const char* func, const char* fmt,
                ...) {
    print_stacktrace();
    // print_stacktrace2();
    // print_stacktrace3();

    fprintf(stderr, "\033\e[0;31mPANIC at %s:%d (%s): ", file, line, func);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\e[0m\n");

    exit(1);
}
