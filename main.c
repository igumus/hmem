#include "./common.h"
#include "./heap.h"
#include <stdio.h>

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
