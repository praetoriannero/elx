#include "unity/unity.h"
#include "core/vector.h"
#include "core/allocator.h"

void test_vector_init(void) {
  Allocator alloc = {};
  allocator_init(&alloc);

  Vector vec = {};
  vector_init(&vec, &alloc, sizeof(u64), 0, NULL);
  TEST_ASSERT_TRUE(vec.alloc != NULL);
  TEST_ASSERT_TRUE(vec.capacity == 0);
  TEST_ASSERT_TRUE(vec.free_item_cb == NULL);
  TEST_ASSERT_TRUE(vec.size == 0);
  TEST_ASSERT_TRUE(vec.data != NULL);
}

void test_vector_push(void) {
  Allocator alloc = {};
  allocator_init(&alloc);

  Vector vec = {};
  vector_init(&vec, &alloc, sizeof(u64), 0, NULL);

  u64 t = 0;
  vector_push(&vec, &t);

  TEST_ASSERT_TRUE(vec.capacity == 1);
  TEST_ASSERT_TRUE(vec.size == 1);

  for (usize idx = 0; idx < 10; idx++) {
    vector_push(&vec, &t);
  }

  TEST_ASSERT_TRUE(vec.capacity == 16);
  TEST_ASSERT_TRUE(vec.size == 11);
}

void test_vector_init_with_capacity(void) {
  Allocator alloc = {};
  allocator_init(&alloc);

  Vector vec = {};
  vector_init(&vec, &alloc, sizeof(u64), 13, NULL);

  TEST_ASSERT_TRUE(vec.capacity == 13);

  u64 t = 3;
  for (usize idx = 0; idx < 15; idx++) {
    vector_push(&vec, &t);
  }

  TEST_ASSERT_TRUE(vec.capacity == 26);
}

void test_vector_get(void) {
  Allocator alloc = {};
  allocator_init(&alloc);

  Vector vec = {};
  vector_init(&vec, &alloc, sizeof(u64), 13, NULL);
  
  u64 t = 3;
  for (usize idx = 0; idx < 15; idx++) {
    vector_push(&vec, &t);
  }

  TEST_ASSERT_TRUE(vector_get(&vec, u64, 4) == 3);
}

void test_vector_insert(void) {
  Allocator alloc = {};
  allocator_init(&alloc);

  Vector vec = {};
  vector_init(&vec, &alloc, sizeof(u64), 13, NULL);
  
  u64 t = 3;
  for (usize idx = 0; idx < 15; idx++) {
    vector_push(&vec, &t);
  }

  t = 7;
  vector_insert(&vec, 5, &t);
  
  TEST_ASSERT_TRUE(vector_get(&vec, u64, 5) == 7);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_vector_init);
	RUN_TEST(test_vector_init_with_capacity);
	RUN_TEST(test_vector_push);
	RUN_TEST(test_vector_get);
	RUN_TEST(test_vector_insert);
	return UNITY_END();
}
