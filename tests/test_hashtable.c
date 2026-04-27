#include "core/hashtable.h"
#include "core/hash.h"
#include "core/allocator.h"
#include "unity/unity.h"
#include "core/str_utils.h"
#include "unity/unity_internals.h"

void test_hash_table_init(void) {
	HashTable ht = {};
	Allocator alloc = {};
	allocator_init(&alloc);
	hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)streq, NULL, NULL);

	TEST_ASSERT_TRUE(ht.hash_func != NULL);
	TEST_ASSERT_TRUE(ht.comp_func != NULL);
	TEST_ASSERT_TRUE(ht.entries.capacity == ht.entries.size);
}

void test_hash_table_insert_get(void) {
	HashTable ht = {};
	Allocator alloc = {};
	allocator_init(&alloc);
	hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)streq, NULL, NULL);

	char* key = "key";
	char* value = "value";
	hash_table_insert(&ht, key, value);

	char* stored_value = hash_table_get(&ht, key);
	TEST_ASSERT_TRUE(stored_value != NULL);
	TEST_ASSERT_TRUE(streq(value, stored_value));
}

void test_hash_table_rehash(void) {
	HashTable ht = {};
	Allocator alloc = {};
	allocator_init(&alloc);
	hash_table_init(&ht, &alloc, (HashFunc)hash_str, (KeyEqualFunc)streq, NULL, NULL);

	for (usize idx = 0; idx < 5000; idx++) {
		char* char_buff = allocator_alloc(ht.alloc, 16);
		sprintf(char_buff, "%zu", idx);
		printf("inserting %zu\n", idx);
		hash_table_insert(&ht, char_buff, char_buff);
	}
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_hash_table_init);
	RUN_TEST(test_hash_table_insert_get);
	RUN_TEST(test_hash_table_rehash);
	return UNITY_END();
}
