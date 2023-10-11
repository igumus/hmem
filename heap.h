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

void check_pointer(void *, size_t);

bool check_pointer_freed(void *);

size_t freed_object_count();

size_t allocated_object_count();

void *heap_alloc(size_t size);

void heap_dump();

void heap_free(void *ptr);

#endif // HEAP_H_
