#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "./common.h"

#define CAPACITY_HEAP_AREA 640000
#define CAPACITY_HEAP_SEGMENT 1024

typedef struct {
  size_t size;
} chunk;

typedef struct {
  void *prev;
  void *next;
  chunk *item;
} segment_node;

typedef struct {
  size_t count;
  segment_node *head;
} segment;

typedef struct {
  size_t size;
  char area[CAPACITY_HEAP_AREA];
} memory;

memory heap = {0};
segment freed = {0};
segment alloced = {0};

void chunk_insert(segment *dst, void *start) {
  assert(dst->count <= CAPACITY_HEAP_SEGMENT);
  dst->count += 1;

  if (dst->head == NULL) {
    dst->head = malloc(sizeof(segment_node));
    dst->head->item = start;
  } else {
    segment_node *node = malloc(sizeof(segment_node));
    node->item = start;
    node->next = dst->head;
    dst->head->prev = node;
    dst->head = node;
  }
}

segment_node *chunk_find_by_start(const segment *src, void *start) {
  if (src->head == NULL) {
    return NULL;
  }
  segment_node *current = src->head;
  while (current != NULL) {
    if (current->item == start)
      return current;
    current = current->next;
  }
  return NULL;
}

segment_node *chunk_find_by_size(const segment *src, size_t size) {
  if (src->head == NULL) {
    return NULL;
  }
  segment_node *current = src->head;
  chunk *item = NULL;
  while (current != NULL) {
    item = (chunk *)current->item;
    if (item->size == size) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void chunk_delete(segment *src, segment_node *node) {
  if (src->head == NULL || node == NULL) {
    return;
  }

  src->count -= 1;
  if (src->head == node) {
    src->head = node->next;
  } else {
    segment_node *nprev = node->prev;
    nprev->next = node->next;
    if (node->next != NULL) {
      segment_node *nnext = node->next;
      nnext->prev = nprev;
    } else {
      nprev->next = NULL;
    }
  }
  free(node);
}

void chunk_merge(segment *dst, void *start, size_t size) {
  bool found = false;
  segment_node *current = dst->head;
  chunk *item = NULL;
  void *current_addr;
  void *new_addr;
  while (current != NULL) {
    item = (chunk *)current->item;
    current_addr = (void *)current->item + sizeof(chunk) + item->size;
    new_addr = (void *)start + sizeof(chunk) + size;

    if (current_addr == start) {
      // checking newly freed chunk is next of item
      // item -> new
      item->size += size + sizeof(chunk);

      if (!found) {
        found = true;
      } else {
        chunk_delete(dst, current->prev);
      }
    } else if (current->item == new_addr) {
      size_t new_size = size + item->size + sizeof(chunk);
      current->item = start;
      item = (chunk *)current->item;
      item->size = new_size;

      if (!found) {
        found = true;
        start = current->item;
        size = item->size;
      } else {
        // check this branch is usable
        current = current->prev;
        chunk_delete(dst, current->next);
      }
    }
    current = current->next;
  }
  if (!found) {
    chunk_insert(dst, start);
  }
}

void segment_dump(const segment *src, const char *name) {
  printf("%s \t#%zu\n", name, src->count);
  if (src->count > 0) {
    chunk *item;
    segment_node *node = src->head;
    while (node != NULL) {
      item = (chunk *)node->item;
      printf("   - meta: %p, start: %p, end: %p, size: %zu\n", node->item,
             (void *)node->item + sizeof(chunk),
             (void *)node->item + sizeof(chunk) + item->size, item->size);
      node = node->next;
    }
  } else {
    printf("   - no chunk found\n");
  }
}

void *heap_alloc(size_t size) {
  if (size == 0)
    return NULL;

  chunk *item = NULL;
  assert((heap.size + size) <= CAPACITY_HEAP_AREA);

  char *head = heap.area + heap.size;
  item = (chunk *)head;
  item->size = size;
  void *start = head + sizeof(chunk);
  heap.size += sizeof(chunk) + size;

  chunk_insert(&alloced, item);

  return start;
}

void heap_free(void *ptr) {
  if (ptr != NULL) {
    void *start = (char *)ptr - sizeof(chunk);
    segment_node *index = chunk_find_by_start(&alloced, start);
    assert(index != NULL);
    chunk *item = index->item;
    chunk_merge(&freed, start, item->size);
    chunk_delete(&alloced, index);
  }
}

void heap_dump() {
  printf("----------------------------------\n");
  segment_dump(&alloced, "Alloced");
  segment_dump(&freed, "Freed   ");
  printf("----------------------------------\n");
}

void simulate_alphabet_allocation() {
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  char *alphabet = heap_alloc(26);
  for (size_t i = 0; i < 26; ++i) {
    alphabet[i] = i + 'A';
  }

  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_invalid_allocation() {
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  heap_alloc(0);
  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_continuous_allocation() {
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  for (size_t i = 0; i < 10; ++i) {
    heap_alloc(i);
  }
  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_memory_gap() {
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  for (size_t i = 0; i < 10; ++i) {
    void *ptr = heap_alloc(i);
    if (ptr != NULL && (i & 1) == 0) {
      heap_free(ptr);
    }
  }
  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_free_space_reusing() {
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  void *str = heap_alloc(10);
  heap_dump();
  heap_free(str);
  heap_dump();
  str = heap_alloc(10);
  heap_dump();
  heap_free(str);
  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_free_space_compaction_by_prev() {
  TODO("Add Description item->new");
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  void *p0 = heap_alloc(10);
  heap_dump();
  void *p1 = heap_alloc(10);
  heap_dump();
  heap_free(p0);
  heap_dump();
  heap_free(p1);
  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_free_space_compaction_by_next() {
  TODO("Add Description new->item");
  printf("### %s: Initial State\n", __func__);
  heap_dump();
  void *p0 = heap_alloc(10);
  heap_dump();
  void *p1 = heap_alloc(10);
  heap_dump();
  heap_free(p1);
  heap_dump();
  heap_free(p0);
  heap_dump();
  printf("### %s: Final State\n\n", __func__);
}

void simulate_free_space_compaction_by_gap() {
  TODO("Add Description");
  printf("### %s: Initial State\n", __func__);

  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
  }
  heap_dump();

  for (size_t i = 0; i < 10; ++i) {
    if (ptrs[i] != NULL && (i & 1) == 0) {
      heap_free(ptrs[i]);
    }
  }
  heap_dump();
  heap_free(ptrs[3]);
  heap_dump();

  printf("### %s: Final State\n\n", __func__);
}

int main(void) {
  // simulate_alphabet_allocation();
  // simulate_invalid_allocation();
  // simulate_continuous_allocation();
  // simulate_memory_gap();
  // simulate_free_space_reusing();
  // simulate_free_space_compaction_by_prev();
  // simulate_free_space_compaction_by_next();
  simulate_free_space_compaction_by_gap();

  return 0;
}
