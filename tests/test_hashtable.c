#include "core/allocator.h"
#include "core/hash.h"
#include "core/hashtable.h"
#include "core/str_utils.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"

void test_hash_table_init(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)str_equal, NULL, NULL);

  TEST_ASSERT_TRUE(ht.hash_func != NULL);
  TEST_ASSERT_TRUE(ht.comp_func != NULL);
  TEST_ASSERT_TRUE(ht.entries.capacity == ht.entries.size);
}

void test_hash_table_insert_get(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)str_equal, NULL, NULL);

  char* key = "key";
  char* value = "value";
  hash_table_insert(&ht, key, value);

  char* stored_value = hash_table_get(&ht, key);
  TEST_ASSERT_TRUE(stored_value != NULL);
  TEST_ASSERT_TRUE(str_equal(value, stored_value));
}

void test_hash_table_rehash(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)str_equal, NULL, NULL);

  for (usize idx = 0; idx < 5000; idx++) {
    char* key = allocator_alloc(ht.alloc, 16);
    sprintf(key, "%zu", idx);
    hash_table_insert(&ht, key, key);
  }

  for (usize idx = 0; idx < 5000; idx++) {
    char* key = allocator_alloc(ht.alloc, 16);
    sprintf(key, "%zu", idx);
    char* value = hash_table_get(&ht, key);
    TEST_ASSERT_TRUE(str_equal(key, value));
  }
}

void test_hash_table_remove(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)str_equal, NULL, NULL);

  for (usize idx = 0; idx < 5000; idx++) {
    char* key = allocator_alloc(ht.alloc, 16);
    sprintf(key, "%zu", idx);
    hash_table_insert(&ht, key, key);
  }

  for (usize idx = 0; idx < 5000; idx++) {
    char* key = allocator_alloc(ht.alloc, 16);
    sprintf(key, "%zu", idx);
    hash_table_remove(&ht, key);
    char* value = hash_table_get(&ht, key);
    TEST_ASSERT_TRUE(value == NULL);
  }
}


int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_hash_table_init);
  RUN_TEST(test_hash_table_insert_get);
  RUN_TEST(test_hash_table_rehash);
  RUN_TEST(test_hash_table_remove);
  return UNITY_END();
}
