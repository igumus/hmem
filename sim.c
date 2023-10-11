#include "./common.h"
#include "./heap.h"
#include <assert.h>
#include <stdio.h>

#ifndef DEBUG
#define DEBUG 0
#endif

// checks heap allocatin with zero byte
void simulate_zero_allocation() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t size = 0;
  void *ptr = heap_alloc(size);
  check_pointer(ptr, size);
  // zero byte allocation will not change heap
  assert(0 == allocated_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(ptr);
  // zero byte allocation will not change heap
  assert(0 == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void simulate_alphabet_allocation() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t size = 26;
  char *alphabet = heap_alloc(size);
  check_pointer(alphabet, size);
  assert(1 == allocated_object_count());
  assert(0 == freed_object_count());

  for (size_t i = 0; i < size; ++i) {
    alphabet[i] = i + 'A';
  }

  for (size_t i = 0; i < size; ++i) {
    assert(alphabet[i] == (char)i + 'A');
  }

#if DEBUG
  heap_dump();
#endif
  heap_free(alphabet);
  assert(0 == allocated_object_count());
  assert(1 == freed_object_count());
  alphabet = NULL;
  printf("### Finished: %s\n\n", __func__);
}

void simulate_continuous_allocation() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
    check_pointer(ptrs[i], i);
  }
  assert(9 == allocated_object_count());
  // all freed objects merged together.
  assert(1 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    heap_free(ptrs[i]);
    ptrs[i] = NULL;
  }
  assert(0 == allocated_object_count());
  // all freed objects are merged together.
  assert(1 == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void simulate_memory_gap() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
    check_pointer(ptrs[i], i);
    if (ptrs[i] != NULL && (i & 1) == 0) {
      heap_free(ptrs[i]);
      ptrs[i] = NULL;
    }
  }
  assert(5 == allocated_object_count());
  assert(5 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    heap_free(ptrs[i]);
    ptrs[i] = NULL;
  }
  assert(0 == allocated_object_count());
  assert(1 == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void simulate_free_space_compaction_by_prev() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t size = 10;
  void *p0 = heap_alloc(size);
  check_pointer(p0, size);
  assert(1 == allocated_object_count());
  assert(1 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  void *p1 = heap_alloc(10);
  check_pointer(p1, size);
  assert(2 == allocated_object_count());
  assert(1 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(p0);
#if DEBUG
  heap_dump();
#endif
  heap_free(p1);
#if DEBUG
  heap_dump();
#endif
  assert(0 == allocated_object_count());
  assert(1 == freed_object_count());
  p0 = NULL;
  p1 = NULL;
  printf("### Finished: %s\n\n", __func__);
}

void simulate_free_space_compaction_by_next() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t size = 10;
  void *p0 = heap_alloc(size);
  check_pointer(p0, size);
  assert(1 == allocated_object_count());
  assert(1 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  void *p1 = heap_alloc(10);
  check_pointer(p0, size);
  assert(2 == allocated_object_count());
  assert(1 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(p1);
#if DEBUG
  heap_dump();
#endif
  heap_free(p0);
#if DEBUG
  heap_dump();
#endif
  p0 = NULL;
  p1 = NULL;
  assert(0 == allocated_object_count());
  assert(1 == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void simulate_free_space_compaction() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
    check_pointer(ptrs[i], i);
  }
  assert(9 == allocated_object_count());
  assert(1 == freed_object_count());

#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    if (ptrs[i] != NULL && (i & 1) == 0) {
      heap_free(ptrs[i]);
      assert(true == check_pointer_freed(ptrs[i]));
      ptrs[i] = NULL;
    }
  }
  assert(5 == allocated_object_count());
  assert(5 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(ptrs[3]);
  // because ptrs[3] merge with ptrs[2] and ptrs[4]
  assert(false == check_pointer_freed(ptrs[3]));
  ptrs[3] = NULL;
  assert(4 == allocated_object_count());
  assert(4 == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    heap_free(ptrs[i]);
    ptrs[i] = NULL;
  }
  assert(0 == allocated_object_count());
  assert(1 == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

int main(void) {
  simulate_zero_allocation();
  simulate_alphabet_allocation();
  simulate_continuous_allocation();
  simulate_memory_gap();
  simulate_free_space_compaction_by_prev();
  simulate_free_space_compaction_by_next();
  simulate_free_space_compaction();

  // after all simulations/tests;
  // heap zero allocated objects, one freed object
  assert(0 == allocated_object_count());
  assert(1 == freed_object_count());

  return 0;
}
