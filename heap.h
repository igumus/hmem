#ifndef HEAP_H_
#define HEAP_H_

#include <stdbool.h>
#include <stddef.h>

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

// void chunk_insert(segment *dst, void *start);
//
// segment_node *chunk_find_by_start(const segment *src, void *start);
//
// segment_node *chunk_find_by_size(const segment *src, size_t size);
//
// void chunk_delete(segment *src, segment_node *node);
//
// void chunk_merge(segment *dst, void *start, size_t size);
//
// void segment_dump(const segment *src, const char *name);

void check_pointer(void *, size_t);

bool check_pointer_freed(void *);

void *heap_alloc(size_t size);

void heap_dump();

void heap_free(void *ptr);

#endif // HEAP_H_
