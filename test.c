#include "./common.h"
#include "./heap.h"
#include <assert.h>
#include <stdio.h>

#ifndef DEBUG
#define DEBUG 0
#endif

// checks heap allocatin with zero byte
void test_zero_allocation() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  size_t size = 0;
  void *ptr = heap_alloc(size);
  check_pointer(ptr, size);
  // zero byte allocation will not change heap
  assert(alloced == allocated_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(ptr);
  assert(alloced == allocated_object_count());
  assert(freed == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void test_alphabet_allocation() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  size_t size = 26;
  char *alphabet = heap_alloc(size);
  check_pointer(alphabet, size);

  for (size_t i = 0; i < size; ++i) {
    alphabet[i] = i + 'A';
  }

  for (size_t i = 0; i < size; ++i) {
    assert(alphabet[i] == (char)i + 'A');
  }
  assert((alloced + 1) == allocated_object_count());

#if DEBUG
  heap_dump();
#endif
  heap_free(alphabet);
  assert(alloced == allocated_object_count());
  assert((freed + 1) == freed_object_count());
  alphabet = NULL;
  printf("### Finished: %s\n\n", __func__);
}

void test_continuous_allocation() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
    check_pointer(ptrs[i], i);
  }
  assert((9 + alloced) == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    heap_free(ptrs[i]);
    ptrs[i] = NULL;
  }
  assert(alloced == allocated_object_count());
  assert(freed == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void test_memory_gap() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
    check_pointer(ptrs[i], i);
    if (ptrs[i] != NULL && (i & 1) == 0) {
      heap_free(ptrs[i]);
      ptrs[i] = NULL;
    }
  }
  assert((5 + alloced) == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    heap_free(ptrs[i]);
    ptrs[i] = NULL;
  }
  assert(alloced == allocated_object_count());
  assert(freed == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

void test_free_space_compaction_by_prev() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  size_t size = 10;
  void *p0 = heap_alloc(size);
  check_pointer(p0, size);
  assert(1 + alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  void *p1 = heap_alloc(10);
  check_pointer(p1, size);
  assert(2 + alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(p0);
  assert(1 + alloced == allocated_object_count());
  assert(1 + freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(p1);
  assert(alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  p0 = NULL;
  p1 = NULL;
  printf("### Finished: %s\n\n", __func__);
}

void test_free_space_compaction_by_next() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  size_t size = 10;
  void *p0 = heap_alloc(size);
  check_pointer(p0, size);
  assert(1 + alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  void *p1 = heap_alloc(10);
  check_pointer(p0, size);
  assert(2 + alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(p1);
  assert(1 + alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(p0);
  assert(alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  p0 = NULL;
  p1 = NULL;
  printf("### Finished: %s\n\n", __func__);
}

void test_free_space_compaction() {
#if DEBUG
  printf("### Starting: %s\n", __func__);
  heap_dump();
#endif
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();
  char *ptrs[10] = {0};
  for (size_t i = 0; i < 10; ++i) {
    ptrs[i] = heap_alloc(i);
    check_pointer(ptrs[i], i);
  }
  assert(9 + alloced == allocated_object_count());
  assert(freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    if (ptrs[i] != NULL && (i & 1) == 0) {
      heap_free(ptrs[i]);
      ptrs[i] = NULL;
    }
  }
  assert(5 + alloced == allocated_object_count());
  assert(4 + freed == freed_object_count());
#if DEBUG
  heap_dump();
#endif
  heap_free(ptrs[3]);
  assert(4 + alloced == allocated_object_count());
  assert(3 + freed == freed_object_count());
  ptrs[3] = NULL;
#if DEBUG
  heap_dump();
#endif
  for (size_t i = 0; i < 10; ++i) {
    heap_free(ptrs[i]);
    ptrs[i] = NULL;
  }
  assert(alloced == allocated_object_count());
  assert(freed == freed_object_count());
  printf("### Finished: %s\n\n", __func__);
}

int main(void) {
  size_t freed = freed_object_count();
  size_t alloced = allocated_object_count();

  test_zero_allocation();
  test_alphabet_allocation();
  test_continuous_allocation();
  test_memory_gap();
  test_free_space_compaction_by_prev();
  test_free_space_compaction_by_next();
  test_free_space_compaction();

  // after all simulations/tests;
  // heap zero allocated objects, one freed object
  assert(alloced == allocated_object_count());
  assert(1 + freed == freed_object_count());

  return 0;
}
