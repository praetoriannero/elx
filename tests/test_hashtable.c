#include "core/allocator.h"
#include "core/hashtable.h"
#include "core/str.h"
#include "core/str_utils.h"
#include "core/xalloc.h"
#include "core/fmt.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"
#include <stdlib.h>

void test_hash_table_init(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)str_hash, (KeyEqualFunc)str_equal, NULL, NULL, NULL, NULL);

  TEST_ASSERT_TRUE(ht.hash_func != NULL);
  TEST_ASSERT_TRUE(ht.comp_func != NULL);
  TEST_ASSERT_TRUE(ht.entries.capacity == ht.entries.size);
}

void test_hash_table_insert_get(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)str_hash, (KeyEqualFunc)str_equal, NULL, NULL, NULL, NULL);

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
  hash_table_init(&ht, &alloc, (HashFunc)str_hash, (KeyEqualFunc)str_equal, NULL, NULL, NULL, NULL);

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

  allocator_deinit(&alloc);
}

void test_hash_table_remove(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)str_hash, (KeyEqualFunc)str_equal, NULL, NULL, NULL, NULL);

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

  allocator_deinit(&alloc);
}

typedef struct {
  String s;
  Allocator* alloc;
} TestKey;

typedef struct {
  i32* x;
  u64* y;
  Allocator* alloc;
} TestValue;

void key_free(TestKey* key) {
  string_deinit(&key->s);
  delete(key->alloc, key);
}

u64 key_hash(TestKey* key) {
  return str_hash(key->s.data);
}

bool key_equal(const TestKey* lhs, const TestKey* rhs) {
  return string_equal(&lhs->s, &rhs->s);
}

TestKey* key_copy(TestKey* key, Allocator* alloc) {
  TestKey* new_key = new(alloc, sizeof(TestKey));
  new_key->s = string_copy(&key->s, alloc);
  new_key->alloc = alloc;
  return new_key;
}

void value_free(TestValue* value) {
  delete(value->alloc, value->x);
  delete(value->alloc, value->y);
  delete(value->alloc, value);
}

TestValue value_create(i32 x, u64 y, Allocator* alloc) {
  TestValue val = {};
  val.x = new(alloc, sizeof(i32));
  val.y = new(alloc, sizeof(u64));

  *val.x = x;
  *val.y = y;
  val.alloc = alloc;

  return val;
}

TestValue* value_copy(TestValue* value, Allocator* alloc) {
  TestValue* value_copy = new(alloc, sizeof(*value_copy));
  value_copy->x = new(alloc, sizeof(i32));
  value_copy->y = new(alloc, sizeof(u64));
  value_copy->alloc = alloc;
  return value_copy;
}

TestKey key_create(char* cstr, Allocator* alloc) {
  return (TestKey){.s = string_from_cstr(cstr, alloc), .alloc = alloc};
}

void test_hash_table_complex_entry(void) {
  scoped_allocator(scratch);
  HashTable ht = {};
  hash_table_init(&ht, &scratch, (HashFunc)key_hash, (KeyEqualFunc)key_equal, (CopyItem)key_copy, (CopyItem)value_copy,
                  (FreeItem)key_free, (FreeItem)value_free);

  for (usize idx = 0; idx < 100; idx++) {
    fflush(stdout);
    char* key_cstr = fmt(&scratch, "%lu", idx);
    TestKey key = key_create(key_cstr, &scratch);
    TestValue value = value_create(-31, 1337, &scratch);
    hash_table_insert(&ht, &key, &value);
  }
}

void test_hash_table_iter_next(void) {
  HashTable ht = {};
  Allocator alloc = {};
  allocator_init(&alloc);
  hash_table_init(&ht, &alloc, (HashFunc)str_hash, (KeyEqualFunc)str_equal, NULL, NULL, NULL, NULL);

  usize test_size = 5000;
  for (usize idx = 0; idx < test_size; idx++) {
    char* key = allocator_alloc(ht.alloc, 16);
    sprintf(key, "%zu", idx);
    hash_table_insert(&ht, key, key);
  }

  HashTableIter ht_iter = {};
  hash_table_iter_init(&ht_iter, &ht);

  char* k = NULL;
  char* v = NULL;
  usize total_count = 0;
  while (hash_table_iter_next(&ht_iter, (void**)&k, (void**)&v)) {
    total_count++;
    TEST_ASSERT_TRUE(str_equal(k, v));
  }

  char* k_recovered = hash_table_get(&ht, "24");

  TEST_ASSERT_TRUE(total_count == test_size);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_hash_table_init);
  RUN_TEST(test_hash_table_insert_get);
  RUN_TEST(test_hash_table_rehash);
  RUN_TEST(test_hash_table_remove);
  RUN_TEST(test_hash_table_complex_entry);
  RUN_TEST(test_hash_table_iter_next);
  return UNITY_END();
}
 
