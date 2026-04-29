#include "core/list.h"

void list_init(List* self, Allocator* alloc) {
  (*self) = (List){
      .alloc = alloc,
      .size = 0,
      .root = NULL,

  };
}

List* list_new(Allocator* alloc) {
  List* list = allocator_alloc(alloc, sizeof(List));
  list_init(list, alloc);
  return list;
}

void list_insert(List* self, void* value, usize idx);

void list_remove(List* self, usize idx);

void list_push(List* self, void* value);

void* list_pop(List* self);

usize list_find(List* self, void* value, ListNodeEqualFunc node_equal_func);

void* list_get(List* self, usize idx);
