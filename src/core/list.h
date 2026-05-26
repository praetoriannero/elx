/**
 * @file list.h
 * @brief Generic doubly-linked list implementation.
 *
 * Provides a type-erased, allocator-backed linked list with configurable
 * item initialization and cleanup callbacks. Supports insertion, removal,
 * lookup, and iteration.
 */

#pragma once

#include "core/allocator.h"

/**
 * @struct List
 * @brief Forward declaration of the doubly-linked list.
 */
typedef struct List List;

/**
 * @struct ListNode
 * @brief Forward declaration of a list node.
 */
typedef struct ListNode ListNode;

/**
 * @typedef ListItemInit
 * @brief Function pointer type for initializing list items.
 *
 * @param item      Pointer to the item to initialize.
 * @param item_args  Additional arguments for initialization.
 */
typedef void (*ListItemInit)(void* item, void* item_args);

/**
 * @typedef ListItemFree
 * @brief Function pointer type for freeing list items.
 *
 * @param item  Pointer to the item to free.
 */
typedef void (*ListItemFree)(void* item);

/**
 * @struct ListInitArgs
 * @brief Configuration arguments for list initialization.
 */
typedef struct {
  /** Function to initialize each list item. */
  ListItemInit item_init;
  /** Function to free each list item. */
  ListItemFree item_free;
  /** Allocator used for node allocations. */
  Allocator* alloc;
  /** Size of each item in bytes. */
  usize item_size;
} ListInitArgs;

/**
 * @struct ListIter
 * @brief Iterator over a linked list.
 *
 * Tracks the current position during sequential traversal.
 */
typedef struct {
  /** Pointer to the list being iterated. */
  List* list;
  /** Current node in the iteration. */
  ListNode* iter;
} ListIter;

/**
 * @struct ListNode
 * @brief A node in the doubly-linked list.
 *
 * Each node holds a pointer to the item data and links to parent and
 * child nodes for bidirectional traversal.
 */
struct ListNode {
  /** Pointer to the parent node, or NULL if this is the head. */
  ListNode* parent;
  /** Pointer to the child node, or NULL if this is the tail. */
  ListNode* child;
  /** Pointer to the item data stored in this node. */
  void* ptr;
};

/**
 * @struct List
 * @brief Doubly-linked list with configurable item lifecycle.
 *
 * Manages a sequence of items with configurable initialization and
 * cleanup callbacks. The list grows dynamically as nodes are added.
 */
struct List {
  /** Pointer to the head node. */
  ListNode* head;
  /** Function to initialize each list item. */
  ListItemInit item_init;
  /** Function to free each list item. */
  ListItemFree item_free;
  /** Allocator used for node allocations. */
  Allocator* alloc;
  /** Size of each item in bytes. */
  usize item_size;
  /** Number of items currently in the list. */
  usize length;
};

/**
 * @typedef ListNodeEqualFunc
 * @brief Function pointer type for comparing list nodes.
 *
 * @param lhs  The first node's item data.
 * @param rhs  The second node's item data.
 * @return      true if the nodes are considered equal, false otherwise.
 */
typedef bool (*ListNodeEqualFunc)(const void* lhs, const void* rhs);

/**
 * @brief Initialize a list with a basic allocator and item size.
 *
 * @param self       Pointer to the List to initialize.
 * @param alloc      Allocator for node allocations.
 * @param item_size  Size of each item in bytes.
 */
void list_init(List* self, Allocator* alloc, usize item_size);

/**
 * @brief Initialize a list with full configuration including item callbacks.
 *
 * @param self         Pointer to the List to initialize.
 * @param alloc        Allocator for node allocations.
 * @param item_size    Size of each item in bytes.
 * @param item_init    Function to initialize each item, or NULL.
 * @param item_free    Function to free each item, or NULL.
 */
void list_init_full(List* self, Allocator* alloc, usize item_size, ListItemInit item_init, ListItemFree item_free);

/**
 * @brief Initialize a list using a ListInitArgs configuration struct.
 *
 * @param self   Pointer to the List to initialize.
 * @param args   Configuration arguments for the list.
 */
void list_init_args(List* self, ListInitArgs* args);

/**
 * @brief Deinitialize and free all resources owned by the list.
 *
 * Frees all nodes and calls the item_free callback for each item.
 * The List struct itself is not freed.
 *
 * @param self  Pointer to the List to deinitialize.
 */
void list_deinit(List* self);

/**
 * @brief Allocate and initialize a new list.
 *
 * @param alloc      Allocator for node allocations.
 * @param item_size  Size of each item in bytes.
 * @return           A newly allocated List on success, or NULL on failure.
 */
List* list_new(Allocator* alloc, usize item_size);

/**
 * @brief Free the List struct itself.
 *
 * Does not free the list's internal resources — use list_deinit()
 * for that.
 *
 * @param self  Pointer to the List to free.
 */
void list_free(List* self);

/**
 * @brief Insert an item at a given index in the list.
 *
 * Shifts existing nodes to make room. A new node is allocated and
 * the item is copied into it.
 *
 * @param self    Pointer to the List.
 * @param value   Pointer to the item to insert.
 * @param idx     Index at which to insert.
 */
void list_insert(List* self, void* value, usize idx);

/**
 * @brief Remove the item at a given index from the list.
 *
 * The item is freed using the item_free callback if registered.
 *
 * @param self  Pointer to the List.
 * @param idx   Index of the item to remove.
 */
void list_remove(List* self, usize idx);

/**
 * @brief Append an item to the end of the list.
 *
 * @param self    Pointer to the List.
 * @param value   Pointer to the item to append.
 */
void list_push(List* self, const void* value);

/**
 * @brief Remove and return the last item from the list.
 *
 * @param self    Pointer to the List.
 * @param owner   Allocator for the returned item's memory, or NULL.
 * @return        Pointer to the removed item, or NULL if the list is empty.
 */
void* list_pop(List* self, Allocator* owner);

/**
 * @brief Find the first node whose item matches the given value.
 *
 * @param self              Pointer to the List.
 * @param value             The value to search for.
 * @param node_equal_func   Function to compare items for equality.
 * @return                  Pointer to the matching item, or NULL if not found.
 */
void* list_find(List* self, void* value, ListNodeEqualFunc node_equal_func);

/**
 * @brief Get the item at a given index.
 *
 * @param self  Pointer to the List.
 * @param idx   Index of the item to retrieve.
 * @return      Pointer to the item at the given index, or NULL if out of bounds.
 */
void* list_get(List* self, usize idx);

/**
 * @brief Create a new iterator over the list.
 *
 * @param self  Pointer to the List.
 * @return      A ListIter initialized at the beginning of the list.
 */
ListIter list_iter(List* self);

/**
 * @brief Initialize a list iterator.
 *
 * @param iter  Pointer to the iterator to initialize.
 * @param list  Pointer to the list to iterate over.
 */
void list_iter_init(ListIter* iter, List* list);

/**
 * @brief Advance the iterator to the next item and retrieve it.
 *
 * @param iter   Pointer to the iterator.
 * @param item   Output pointer for the item, or NULL.
 * @return        true if an item was retrieved, false if iteration is complete.
 */
bool list_iter_next(ListIter* iter, void** item);

/**
 * @brief Set the item initialization callback for the list.
 *
 * @param self              Pointer to the List.
 * @param item_init_func    Function to initialize each item.
 * @param item_args         Additional arguments passed to the init function.
 */
void list_item_init(List* self, ListItemInit item_init_func, void* item_args);
