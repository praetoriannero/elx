#include <string.h>
#include "core/list.h"
#include "core/panic.h"

static inline void list_free_node(List* self, ListNode* node) {
  if (node->parent) {
    node->parent->child = node->child;
  }

  if (node->child) {
    node->child->parent = node->parent;
  }

  if (node == self->head) {
    self->head = node->child;
  }

  allocator_free(self->alloc, node->ptr);
  allocator_free(self->alloc, node);
}

static inline ListNode* list_get_tail(List* self) {
  ListNode* node = self->head;
  while (node) {
    if (node->child) {
      node = node->child;
    } else {
      return node; 
    }
  }

  return node;
}

void list_init(List* self, Allocator* alloc, usize item_size) {
  (*self) = (List){
      .alloc = alloc,
      .length = 0,
      .head = NULL,
      .item_size = item_size,
  };
}

void list_init_args(List* self, ListInitArgs* args) {
  list_init(self, args->alloc, args->item_size);
}

void list_deinit(List* self) {
  ListNode* node = self->head;
  ListNode* child = NULL;
  while (node) {
    if (node->parent) {
      list_free_node(self, node->parent);
    }

    if (node->child) {
      node = node->child;
    } else {
      list_free_node(self, node);
      break;
    }
  }

  list_init(self, self->alloc, self->item_size);
}

List* list_new(Allocator* alloc, usize item_size) {
  List* list = allocator_alloc(alloc, sizeof(List));
  list_init(list, alloc, item_size);
  return list;
}

void list_free(List* self) {
  list_deinit(self);
  allocator_free(self->alloc, self);  
}

void list_insert(List* self, void* value, usize idx) {
  if (idx > self->length || idx < 0) {
    panic("Invalid insertion at index %lu for list of length %lu\n", idx, self->length);
  }

  ListNode* node = self->head;
  ListNode* new_node = NULL;
  void* ptr = NULL;
  usize iter_idx = 0;

  while (iter_idx < self->length) {
    if (iter_idx == idx) {
      ptr = allocator_alloc(self->alloc, self->item_size);
      memcpy(ptr, value, self->item_size);
      new_node = allocator_alloc(self->alloc, sizeof(ListNode));

      if (node->parent) {
        node->parent->child = new_node;
      }

      if (node->child) {
        new_node->child = node->child;
        node->child->parent = new_node;
      }

      new_node->parent = node;
      self->length++;
    }

    node = node->child;
    iter_idx++;
  }
}

void list_remove(List* self, usize idx) {
  ListNode* node = self->head;
  usize iter_idx = 0;

  if (idx > self->length || idx < 0) {
    panic("Invalid removal at index %lu for list of length %lu\n", idx, self->length);
  }

  while (node) {
    if (iter_idx == idx) {
      list_free_node(self, node);
      break;
    }

    node = node->child;
    iter_idx++;
  }

  self->length--;
}

void list_push(List* self, const void* value) {
  if (!self) {
    panic("list is null\n");
  }

  void* ptr = allocator_alloc(self->alloc, self->item_size);
  ListNode* new_node = allocator_alloc(self->alloc, sizeof(ListNode));
  ListNode* tail = list_get_tail(self);

  if (!self->head) {
    self->head = new_node;
  }

  (*new_node) = (ListNode){
    .parent = tail,
    .child = NULL,
    .ptr = ptr,
  };

  memcpy(ptr, value, self->item_size);

  if (tail) {
    tail->child = new_node;
  }

  self->length++;
}

void* list_pop(List* self, Allocator* owner) {
  ListNode* tail = list_get_tail(self);
  void* ptr = NULL;

  if (tail) {
    if (tail->parent) {
      ptr = allocator_alloc(owner, self->item_size);
      memcpy(ptr, tail->ptr, self->item_size);
      list_free_node(self, tail);
    }

    if (tail == self->head) {
      self->head = NULL;
    }

    self->length--;
  }


  return ptr;
}

void* list_find(List* self, void* value, ListNodeEqualFunc node_equal_func) {
  ListNode* node = self->head;
  while (node) {
    if (node_equal_func(node->ptr, value)) {
      return node->ptr;
    }

    node = node->child;
  }

  return NULL;
}

void* list_get(List* self, usize idx) {
  ListNode* node = self->head;
  usize iter = 0;

  if (idx > self->length || idx < 0) {
    panic("Invalid get at index %lu for list of length %lu\n", idx, self->length);
  }

  while (node) {
    if (iter == idx) {
      return node->ptr;
    }

    node = node->child;
    iter++;
  }

  return NULL;
}

ListIter list_iter(List* self) {
  return (ListIter){.list = self, .iter = self->head};
}

void list_iter_init(ListIter* self, List* list) {
  (*self) = (ListIter){
    .iter = list->head,
    .list = list,
  };
}

bool list_iter_next(ListIter* self, void** value) {
  if (self->iter) {
    *value = self->iter->ptr;
    self->iter = self->iter->child;
    return true;
  }

  return false;
}

void list_item_init(List* self, ListItemInit item_init_func, void* item_args) {
  auto iter = list_iter(self);
  void* item = NULL;
  while (list_iter_next(&iter, &item)) {
    item_init_func(item, item_args);
  }
}
