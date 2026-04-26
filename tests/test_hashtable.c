#include "core/hashtable.h"
#include "core/hash.h"
#include "core/allocator.h"
#include "unity/unity.h"
#include <string.h>

void test_hash_table_init(void) {
	TEST_ASSERT_TRUE(1);
	HashTable ht = {};
	Allocator alloc = {};
	allocator_init(&alloc);
	// hash_table_init(&ht, &alloc, fnv1a)
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_hash_table_init);
	return UNITY_END();
}
