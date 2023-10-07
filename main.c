#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "./common.h"

#define CAPACITY_HEAP_AREA 640000
#define CAPACITY_HEAP_SEGMENT 1024

typedef struct {
  void *start;
  size_t size;
} chunk;

typedef struct {
  void *prev;
  void *next;
  chunk item;
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

void chunk_insert(segment *dst, void *start, size_t size) {
  assert(dst->count <= CAPACITY_HEAP_SEGMENT);
  chunk item = {
      .start = start,
      .size = size,
  };
  dst->count += 1;

  if (dst->head == NULL) {
    dst->head = malloc(sizeof(segment_node));
    dst->head->item = item;
  } else {
    segment_node *node = malloc(sizeof(segment_node));
    node->item = item;
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
    if (current->item.start == start) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

segment_node *chunk_find_by_size(const segment *src, size_t size) {
  if (src->head == NULL) {
    return NULL;
  }
  segment_node *current = src->head;
  while (current != NULL) {
    if (current->item.size == size) {
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
  while (current != NULL) {
    item = &current->item;
    if ((item->start + item->size) == start) {
      // checking newly freed chunk is next of item
      // item -> new
      item->size += size;
      start = item->start;
      size = item->size;

      if (!found) {
        found = true;
      } else {
        chunk_delete(dst, current->prev);
      }
    } else if (item->start == (start + size)) {
      // checking newly freed chunk is prev of item
      // new -> item
      item->start = start;
      item->size += size;
      start = item->start;
      size = item->size;

      if (!found) {
        found = true;
      } else {
        chunk_delete(dst, current->prev);
      }
    }
    current = current->next;
  }
  if (!found) {
    chunk_insert(dst, start, size);
  }
}

void segment_dump(const segment *src, const char *name) {
  printf("%s \t#%zu\n", name, src->count);
  if (src->count > 0) {
    chunk chunk;
    segment_node *node = src->head;
    while (node != NULL) {
      chunk = node->item;
      printf("   - start: %p, size: %zu\n", chunk.start, chunk.size);
      node = node->next;
    }
  } else {
    printf("   - no chunk freed\n");
  }
}

void *heap_alloc(size_t size) {
  if (size == 0)
    return NULL;

  // try to used already freed memory
  segment_node *index = chunk_find_by_size(&freed, size);
  if (index != NULL) {
    chunk item = index->item;
    chunk_delete(&freed, index);
    chunk_insert(&alloced, item.start, item.size);
    return item.start;
  }

  assert((heap.size + size) <= CAPACITY_HEAP_AREA);
  void *result = heap.area + heap.size;
  heap.size += size;

  chunk_insert(&alloced, result, size);

  return result;
}

void heap_free(void *ptr) {
  if (ptr != NULL) {
    segment_node *index = chunk_find_by_start(&alloced, ptr);
    assert(index != NULL);
    chunk item = index->item;
    chunk_merge(&freed, item.start, item.size);
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
