#include "unity/unity.h"
#include "core/allocator.h"


void test_alloc_init_free(void) {
  Allocator alloc = {};
  allocator_init(&alloc);

  int* int_arr[256];
  for (usize idx = 0; idx < 500; idx++) {
    int_arr[idx] = allocator_alloc(&alloc, sizeof(int));
  }

  allocator_free(&alloc, int_arr[17]);
}


int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_alloc_init_free);
  return UNITY_END();
}
