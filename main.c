#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define NOTIMPLEMENTED                                                         \
  do {                                                                         \
    fprintf(stderr, "%s:%d ERROR: not implemented: %s\n", __FILE__, __LINE__,  \
            __func__);                                                         \
  } while (0);

#define TODO(task) printf("%s:%s TODO: %s\n", __FILE__, __func__, (task))

#define CAPACITY_HEAP_AREA 640000
#define CAPACITY_HEAP_SEGMENT 1024

typedef struct {
  void *start;
  size_t size;
} chunk;

typedef struct {
  size_t count;
  chunk chunks[CAPACITY_HEAP_SEGMENT];
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
  dst->chunks[dst->count] = item;
  dst->count += 1;
}

bool chunk_merge(segment *dst, void *start, size_t size) {
  bool result = false;
  if (dst->count > 0) {
    for (size_t i = 0; i < dst->count; ++i) {
      chunk *item = &dst->chunks[i];
      if ((item->start + item->size) == start) {
        // checking newly freed chunk is next of item
        // item -> new
        item->size += size;
        result = true;
      } else if (item->start == (start + size)) {
        // checking newly freed chunk is prev of item
        // new -> item
        item->start = start;
        item->size += size;
        result = true;
      }
    }
  }
  return result;
}

void chunk_create(segment *dst, void *start, size_t size) {
  if (!chunk_merge(dst, start, size)) {
    chunk_insert(dst, start, size);
  }
}

int chunk_find_by_start(const segment *src, void *start) {
  chunk chunk;
  for (size_t i = 0; i < src->count; ++i) {
    chunk = src->chunks[i];
    if (chunk.start == start) {
      return i;
    }
  }
  return -1;
}

int chunk_find_by_size(const segment *src, size_t size) {
  chunk chunk;
  for (size_t i = 0; i < src->count; ++i) {
    chunk = src->chunks[i];
    if (chunk.size == size) {
      return i;
    }
  }
  return -1;
}

void chunk_delete(segment *src, size_t index) {
  assert(index >= 0);
  assert(index < src->count);
  for (size_t i = index; i < src->count - 1; ++i) {
    src->chunks[i] = src->chunks[i + 1];
  }
  src->count -= 1;
}

void segment_dump(const segment *src, const char *name) {
  printf("%s \t#%zu\n", name, src->count);
  if (src->count > 0) {
    chunk chunk;
    for (size_t i = 0; i < src->count; ++i) {
      chunk = src->chunks[i];
      printf("   - start: %p, size: %zu\n", chunk.start, chunk.size);
    }
  } else {
    printf("   - no chunk freed\n");
  }
}

void *heap_alloc(size_t size) {
  if (size == 0)
    return NULL;

  // try to used already freed memory
  int index = chunk_find_by_size(&freed, size);
  if (index >= 0) {
    chunk item = freed.chunks[index];
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
    const int index = chunk_find_by_start(&alloced, ptr);
    assert(index >= 0);
    chunk item = alloced.chunks[index];
    chunk_create(&freed, item.start, item.size);
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

int main(void) {
  // simulate_alphabet_allocation();
  // simulate_invalid_allocation();
  // simulate_continuous_allocation();
  // simulate_memory_gap();
  // simulate_free_space_reusing();
  // simulate_free_space_compaction_by_prev();
  simulate_free_space_compaction_by_next();

  return 0;
}
