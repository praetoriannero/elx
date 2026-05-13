#ifndef ELX_DEBUG
#define ELX_DEBUG
#endif

#include "core/allocator.h"
#include "unity/unity.h"

void test_alloc_init_free(void) {
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
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_alloc_init_free);
  return UNITY_END();
}
