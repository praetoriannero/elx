#pragma once

#include "core/allocator.h"

typedef struct List List;
typedef struct ListNode ListNode;

struct ListNode {
  ListNode* parent;
  ListNode* child;  
  void* value;
};

struct List {
  ListNode* root;
  Allocator* alloc;
  usize size;
};

typedef bool (*ListNodeEqualFunc)(const void* lhs, const void* rhs);

void list_init(List* self, Allocator* alloc);

List* list_new(Allocator* alloc);

void list_insert(List* self, void* value, usize idx);

void list_remove(List* self, usize idx);

void list_push(List* self, void* value);

void* list_pop(List* self);

usize list_find(List* self, void* value, ListNodeEqualFunc node_equal_func);

void* list_get(List* self, usize idx);
