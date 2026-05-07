#include <string.h>
#include "core/panic.h"
#include "core/list.h"

static inline ListNode* _get_tail(List* self) {
  ListNode* node = self->root;
  while (node) {
    node = node->child;
  }

  return node;
}

void list_init(List* self, Allocator* alloc, usize item_size) {
  (*self) = (List){
      .alloc = alloc,
      .length = 0,
      .root = NULL,
      .item_size = item_size,
  };
}

List* list_new(Allocator* alloc, usize item_size) {
  List* list = allocator_alloc(alloc, sizeof(List));
  list_init(list, alloc, item_size);
  return list;
}

void list_insert(List* self, void* value);

void list_remove(List* self, usize idx);

void list_push(List* self, const void* value) {
  ListNode* tail = _get_tail(self);

  ListNode* new_node = allocator_alloc(self->alloc, sizeof(ListNode) + self->item_size);
  (*new_node) = (ListNode){
    .parent = tail,
    .child = NULL,
  };

  memcpy(new_node + 1, value, self->item_size);

  if (tail) {
    tail->child = new_node;
  }

  self->length++;
}

#define safe_deref(ptr, field, value) if (!ptr) panic("null pointer dereference"); ptr->field = value;

#define box(alloc, type) allocator_alloc(alloc, sizeof(type))

typedef struct {
  void* inner;
  void* (*unwrap)(void);
} Box;

void* list_pop(List* self) {
  ListNode* tail = _get_tail(self);
  ListNode* head = box(self->alloc, ListNode);
  safe_deref(head, parent, NULL);
}

usize list_find(List* self, void* value, ListNodeEqualFunc node_equal_func);

void* list_get(List* self, usize idx);
