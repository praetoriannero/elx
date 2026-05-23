#include "core/allocator.h"
#include "unity/unity.h"

void test_alloc_init_free(void) {
#ifndef ELX_DEBUG
    Allocator alloc = {};
    allocator_init(&alloc);

    int* int_arr[256];
    for (usize idx = 0; idx < 256; idx++) {
        int_arr[idx] = allocator_alloc(&alloc, sizeof(int));
    }

    TEST_ASSERT_TRUE(alloc.total_alloc == 256 * sizeof(int));

    for (usize idx = 0; idx < 256; idx++) {
        allocator_free(&alloc, int_arr[idx]);
    }

    TEST_ASSERT_TRUE(alloc.total_alloc == 0);
    allocator_deinit(&alloc);
#endif
}

void test_alloc_macro(void) {
    scoped_allocator(scratch);
    i32* my_int_ptr = scratch.alloc(&scratch, sizeof(i32));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_alloc_init_free);
    RUN_TEST(test_alloc_macro);
    return UNITY_END();
}
