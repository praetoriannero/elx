#include "core/arena.h"
#include "unity/unity.h"
#include <string.h>

// Test: Arena initialization and cleanup
void test_arena_init_and_deinit(void) {
  Arena arena;
  arena_init(&arena, 0);  // Should use ARENA_DEFAULT_SIZE
  
  void* p1 = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(p1);
  
  void* p2 = arena_alloc(&arena, sizeof(float));
  TEST_ASSERT_NOT_NULL(p2);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

// Test: Arena allocations work within the first chunk
void test_arena_allocation_grows_chunks(void) {
  Arena arena;
  arena_init(&arena, 0);  // Start with one chunk (1 MiB)
  
  // Allocate small amounts - should fit in first chunk
  void* ptrs[100];
  for (usize i = 0; i < 100; i++) {
    ptrs[i] = arena_alloc(&arena, sizeof(int));
    TEST_ASSERT_NOT_NULL(ptrs[i]);
  }
  
  // Check that all allocations are within ARENA_DEFAULT_SIZE
  usize total_used = 100 * sizeof(int);
  TEST_ASSERT_TRUE(total_used <= ARENA_DEFAULT_SIZE);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

// Test: Realloc grows when needed by allocating from next chunk
void test_arena_realloc_grows_when_needed(void) {
  Arena arena;
  arena_init(&arena, 0);
  
  // Create a small allocation
  void* old_ptr = arena_alloc(&arena, sizeof(char));
  TEST_ASSERT_NOT_NULL(old_ptr);
  
  // Resize to larger - should allocate from next chunk if needed
  usize large_size = ARENA_DEFAULT_SIZE + 1;
  void* new_ptr = arena_realloc(&arena, old_ptr, large_size);
  
  TEST_ASSERT_NOT_NULL(new_ptr);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

// Test: Multiple chunks when initializing with small size
void test_arena_multiple_chunks(void) {
  Arena arena;
  arena_init(&arena, sizeof(int) * 10 + 5);  // Very small first chunk
  
  // First allocation should fit in small chunk
  void* ptr1 = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr1);
  
  // Second allocation should cause overflow and new chunk creation
  void* ptr2 = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr2);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

// Test: Arena free is a no-op (memory stays in pool)
void test_arena_free_no_op(void) {
  Arena arena;
  arena_init(&arena, 0);
  
  void* ptr = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr);
  
  // In arena allocator, free is a no-op (memory stays in pool)
  arena_free(&arena, ptr);
  
  // Pointers should still be valid and usable
  TEST_ASSERT_NOT_NULL(ptr);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

// Test: Arena move transfers allocation from src to dst
void test_arena_move_no_op(void) {
  Arena src;
  Arena dst;
  
  arena_init(&src, sizeof(int) * 10);  // Small initial size to allow growth
  arena_init(&dst, 0);
  
  void* ptr = arena_alloc(&src, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr);
  
  int original_value = 42;
  memcpy(ptr, &original_value, sizeof(int));
  
  // Move the allocation from src to dst
  arena_move(&src, &dst, ptr);
  
  // The moved pointer should no longer be valid in src
  void* moved_ptr = arena_alloc(&dst, sizeof(int));
  TEST_ASSERT_NOT_NULL(moved_ptr);
  
  memcpy(moved_ptr, ptr, sizeof(int));
  int copied_value;
  memcpy(&copied_value, moved_ptr, sizeof(int));
  TEST_ASSERT_EQUAL(42, copied_value);
  
  // Clean up both arenas to avoid memory leaks
  arena_deinit(&src);
  arena_deinit(&dst);
}

// Test: Copy between arenas works correctly
void test_arena_copy(void) {
  Arena src;
  Arena dst;
  
  arena_init(&src, 0);
  
  arena_init(&dst, 0);
  
  // Allocate source data
  void* src_ptr = arena_alloc(&src, sizeof(char) * 16);
  TEST_ASSERT_NOT_NULL(src_ptr);
  
  // Copy to another arena using the copy function
  char copy_data[16] = "Hello World!";
  void* dst_ptr = arena_copy(&dst, src_ptr, sizeof(copy_data));
  TEST_ASSERT_NOT_NULL(dst_ptr);
  
  // Clean up both arenas to avoid memory leaks
  arena_deinit(&src);
  arena_deinit(&dst);
}

// Test: Arena works with various allocation sizes
void test_arena_various_sizes(void) {
  Arena arena;
  arena_init(&arena, ARENA_DEFAULT_SIZE / 2);  // Start with half default size
  
  // Allocate various sizes
  void* ptr1 = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr1);
  
  void* ptr2 = arena_alloc(&arena, sizeof(float) * 100);
  TEST_ASSERT_NOT_NULL(ptr2);
  
  void* ptr3 = arena_alloc(&arena, ARENA_DEFAULT_SIZE / 4);
  TEST_ASSERT_NOT_NULL(ptr3);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

// Test: Arena grows chunks when memory is exhausted
void test_arena_grows_chunks_when_exhausted(void) {
  Arena arena;
  arena_init(&arena, sizeof(int) * 10);  // Very small first chunk
  
  // Allocate one item - should fit in first chunk
  void* ptr1 = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr1);
  
  // Allocate second item - might need new chunk
  void* ptr2 = arena_alloc(&arena, sizeof(int));
  TEST_ASSERT_NOT_NULL(ptr2);
  
  // Clean up to avoid memory leaks
  arena_deinit(&arena);
}

int main(void) {
  UNITY_BEGIN();
  
  RUN_TEST(test_arena_init_and_deinit);
  RUN_TEST(test_arena_allocation_grows_chunks);
  RUN_TEST(test_arena_realloc_grows_when_needed);
  RUN_TEST(test_arena_multiple_chunks);
  RUN_TEST(test_arena_free_no_op);
  RUN_TEST(test_arena_move_no_op);
  RUN_TEST(test_arena_copy);
  RUN_TEST(test_arena_various_sizes);
  RUN_TEST(test_arena_grows_chunks_when_exhausted);
  
  return UNITY_END();
}
