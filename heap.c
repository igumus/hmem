#include "./heap.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

memory heap = {0};
segment freed = {0};
segment alloced = {0};

void *chunk_end_addr(chunk *ptr) {
  return ((char *)ptr) + sizeof(chunk) + ptr->size;
}

void *chunk_user_addr(chunk *ptr) { return ((char *)ptr) + sizeof(chunk); }

chunk *pointer_to_chunk(void *ptr) { return (chunk *)(ptr - sizeof(chunk)); }

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
    if (item->size >= size) {
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

void chunk_merge(segment *dst, void *candidate_start_addr,
                 size_t candidate_size) {
  bool found = false;
  segment_node *current = dst->head;
  void *item_end_addr;
  void *candidate_end_addr;
  while (current != NULL) {
    item_end_addr = chunk_end_addr(current->item);
    candidate_end_addr = candidate_start_addr + sizeof(chunk) + candidate_size;

    if (item_end_addr == candidate_start_addr) {
      current->item->size += candidate_size + sizeof(chunk);

      if (!found) {
        found = true;
        candidate_start_addr = current->item;
        candidate_size = current->item->size;
      } else {
        chunk_delete(dst, current->prev);
      }
    } else if (candidate_end_addr == current->item) {
      size_t new_size = candidate_size + current->item->size + sizeof(chunk);
      current->item = candidate_start_addr;
      current->item->size = new_size;

      if (!found) {
        found = true;
        candidate_start_addr = current->item;
        candidate_size = current->item->size;
      } else {
        current = current->prev;
        chunk_delete(dst, current->next);
      }
    }
    current = current->next;
  }
  if (!found) {
    chunk_insert(dst, candidate_start_addr);
  }
}

void segment_dump(const segment *src, const char *name) {
  printf("%s \t#%zu\n", name, src->count);
  if (src->count > 0) {
    segment_node *node = src->head;
    while (node != NULL) {
      printf("   - meta: %p, start: %p, end: %p, size: %zu\n", node->item,
             chunk_user_addr(node->item), chunk_end_addr(node->item),
             node->item->size);
      node = node->next;
    }
  } else {
    printf("   - no chunk found\n");
  }
}

void check_pointer(void *ptr, size_t size) {
  if (size == 0) {
    assert(ptr == NULL);
  } else {
    assert(ptr != NULL);
    chunk *meta = pointer_to_chunk(ptr);
    assert(meta->size >= size);
  }
}

bool check_pointer_freed(void *ptr) {
  if (ptr != NULL) {
    chunk *meta = pointer_to_chunk(ptr);
    return chunk_find_by_start(&freed, meta) != NULL;
  }
  assert(0 && "unreachable");
}

size_t freed_object_count() { return freed.count; }

size_t allocated_object_count() { return alloced.count; }

void *heap_alloc(size_t requested_size) {
  if (requested_size == 0)
    return NULL;

  chunk *item = NULL;
  size_t actual_size = requested_size + sizeof(chunk);

  segment_node *node = chunk_find_by_size(&freed, actual_size);
  if (node != NULL) {
    item = node->item;
    size_t remain = item->size - actual_size;
    item->size = requested_size;
    chunk_insert(&alloced, item);
    if (remain > 0) {
      node->item = (chunk *)chunk_end_addr(item);
      node->item->size = remain;
    } else {
      chunk_delete(&freed, node);
    }
  } else {
    assert((heap.size + actual_size) <= CAPACITY_HEAP_AREA);
    item = (chunk *)(heap.area + heap.size);
    item->size = requested_size;
    heap.size += actual_size;

    chunk_insert(&alloced, item);
  }
  assert(item != NULL);
  return chunk_user_addr(item);
}

void heap_free(void *ptr) {
  if (ptr != NULL) {
    chunk *item = pointer_to_chunk(ptr);
    segment_node *index = chunk_find_by_start(&alloced, item);
    assert(index != NULL);
    chunk_merge(&freed, item, item->size);
    chunk_delete(&alloced, index);
  }
}

void heap_dump() {
  printf("----------------------------------\n");
  segment_dump(&alloced, "Alloced");
  segment_dump(&freed, "Freed   ");
  printf("----------------------------------\n");
}
