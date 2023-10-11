#include "./heap.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
    segment_node *node = src->head;
    while (node != NULL) {
      char *item = (char *)node->item;
      printf("   - meta: %p, start: %p, end: %p, size: %zu\n", item,
             item + sizeof(chunk), item + sizeof(chunk) + node->item->size,
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
    chunk *meta = (chunk *)(ptr - sizeof(size_t));
    assert(meta->size >= size);
  }
}

bool check_pointer_freed(void *ptr) {
  if (ptr != NULL) {
    chunk *meta = (chunk *)(ptr - sizeof(size_t));
    return chunk_find_by_start(&freed, meta) != NULL;
  }
  assert(0 && "unreachable");
}

size_t freed_object_count() { return freed.count; }

size_t allocated_object_count() { return alloced.count; }

void *heap_alloc(size_t size) {
  if (size == 0)
    return NULL;

  size_t csize = size + sizeof(chunk);

  chunk *item = NULL;
  segment_node *node = chunk_find_by_size(&freed, csize);
  if (node != NULL) {
    item = node->item;
    size_t remain = item->size - csize;
    char *ret = ((char *)item) + sizeof(chunk);
    item->size = size;
    chunk_insert(&alloced, item);
    if (remain > 0) {
      char *end = ret + item->size;
      node->item = (chunk *)end;
      node->item->size = remain;
    } else {
      chunk_delete(&freed, node);
    }
    return ret;
  } else {
    assert((heap.size + csize) <= CAPACITY_HEAP_AREA);
    char *head = heap.area + heap.size;
    item = (chunk *)head;
    item->size = size;
    void *start = head + sizeof(chunk);
    heap.size += csize;

    chunk_insert(&alloced, item);

    return start;
  }
  return NULL;
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
