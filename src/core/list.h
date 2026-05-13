#pragma once

#include "core/allocator.h"

typedef struct List List;
typedef struct ListNode ListNode;

typedef struct {
  List* list;
  ListNode* iter;
} ListIter;

struct ListNode {
  ListNode* parent;
  ListNode* child;
  void* ptr;
};

struct List {
  ListNode* head;
  Allocator* alloc;
  usize item_size;
  usize length;
};

typedef bool (*ListNodeEqualFunc)(const void* lhs, const void* rhs);

void list_init(List* self, Allocator* alloc, usize item_size);

void list_deinit(List* self);

List* list_new(Allocator* alloc, usize item_size);

void list_free(List* self);

void list_insert(List* self, void* value, usize idx);

void list_remove(List* self, usize idx);

void list_push(List* self, const void* value);

void* list_pop(List* self, Allocator* owner);

void* list_find(List* self, void* value, ListNodeEqualFunc node_equal_func);

void* list_get(List* self, usize idx);

void list_iter_init(ListIter* iter, List* list);

bool list_iter_next(ListIter* iter, void** item);

// #define list_iter_next(list_iter, type, item) (type*)_list_iter_next(list_iter, (void*)&item)
