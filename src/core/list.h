#pragma once

#include "core/allocator.h"

typedef struct List List;
typedef struct ListNode ListNode;

typedef struct {
  List* list;
  usize index;
} ListIter;

struct ListNode {
  ListNode* parent;
  ListNode* child;
};

struct List {
  ListNode* root;
  Allocator* alloc;
  usize item_size;
  usize length;
};

typedef bool (*ListNodeEqualFunc)(const void* lhs, const void* rhs);

void list_init(List* self, Allocator* alloc, usize item_size);

List* list_new(Allocator* alloc, usize item_size);

void list_insert(List* self, void* value);

void list_remove(List* self, usize idx);

void list_push(List* self, const void* value);

void* list_pop(List* self);

usize list_find(List* self, void* value, ListNodeEqualFunc node_equal_func);

void* list_get(List* self, usize idx);

void list_iter_init(ListIter* iter, List* list);

void* _list_iter_next(ListIter* iter, void* item);

#define list_iter_next(list_iter, type, item) (type*)_list_iter_next(list_iter, (void*)item)
