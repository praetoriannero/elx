#include "core/list.h"
#include "unity/unity.h"

void test_list_init(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List list = {};
    list_init(&list, &scratch, sizeof(int));

    TEST_ASSERT_TRUE(list.item_size == sizeof(int));
}

void test_list_deinit(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List list = {};
    list_init(&list, &scratch, sizeof(int));
    list_deinit(&list);
    TEST_ASSERT_TRUE(list.length == 0);
}

void test_list_push(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    TEST_ASSERT_TRUE(list->length == 256);
    TEST_ASSERT_TRUE(list->head);
}

void test_list_pop(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    TEST_ASSERT_TRUE(list->length == 256);

    for (usize i = 0; i < 256; i++) {
        list_pop(list, &scratch);
    }

    TEST_ASSERT_TRUE(!list->head);
    TEST_ASSERT_TRUE(list->length == 0);
}

void test_list_new(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(int));
    allocator_free(&scratch, list);
}

void test_list_free(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    TEST_ASSERT_TRUE(list->length == 256);

    list_deinit(list);
    TEST_ASSERT_TRUE(list->length == 0);
}

void test_list_insert(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    TEST_ASSERT_TRUE(list->length == 256);
}

void test_list_remove(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    TEST_ASSERT_TRUE(list->length == 256);

    usize indices[] = {0, 77, 124};
    for (usize j = 0; j < 3; j++) {
        list_remove(list, indices[j]);
    }

    TEST_ASSERT_TRUE(list->length == 253);
}

static inline bool usize_equal(const void* lhs, const void* rhs) { return *(usize*)lhs == *(usize*)rhs; }

void test_list_find(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    usize indices[] = {0, 77, 124};
    for (usize j = 0; j < 3; j++) {
        TEST_ASSERT_TRUE(*(usize*)list_find(list, &indices[j], usize_equal) == indices[j]);
    }
}

void test_list_get(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    usize indices[] = {0, 77, 124};
    for (usize j = 0; j < 3; j++) {
        TEST_ASSERT_TRUE(*(usize*)list_get(list, indices[j]) == indices[j]);
    }
}

void test_list_iter_init(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    ListIter iter = {};
    list_iter_init(&iter, list);
    TEST_ASSERT_TRUE(iter.list == list);
    TEST_ASSERT_TRUE(iter.iter == list->head);
}

void test_list_iter_next(void) {
    Allocator scratch = {};
    allocator_init(&scratch);
    List* list = list_new(&scratch, sizeof(usize));
    for (usize i = 0; i < 256; i++) {
        list_push(list, &i);
    }

    ListIter iter = {};
    list_iter_init(&iter, list);
    usize* value = NULL;
    usize i = 0;
    while (list_iter_next(&iter, (void**)&value)) {
        TEST_ASSERT_TRUE(*value == *(usize*)list_get(list, i));
        i++;
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_list_init);
    RUN_TEST(test_list_deinit);
    RUN_TEST(test_list_push);
    RUN_TEST(test_list_pop);
    RUN_TEST(test_list_new);
    RUN_TEST(test_list_free);
    RUN_TEST(test_list_insert);
    RUN_TEST(test_list_remove);
    RUN_TEST(test_list_find);
    RUN_TEST(test_list_get);
    RUN_TEST(test_list_iter_init);
    RUN_TEST(test_list_iter_next);
    return UNITY_END();
}
