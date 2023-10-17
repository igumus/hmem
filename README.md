# hmem
`hmem` - heap memory experiments

## Quick Start
```console
$ make test
```

## Notes
- `user pointer`: is a pointer which allocator returns with `heap_alloc` function.
- `sys  pointer`: is a pointer which points to `data segment` internals.

## Development Steps
1. Created heap components; `data segment`, `object (freed, allocated) segments`. Freed and allocated object segments was responsible of storing user pointer (which returned with `heap_alloc` function) and requested memory size in their structure. 
2. Introduced `meta arena` in `data segment`. While allocating memory, allocator creates additional `meta arena` before user which stores requested memory size in it ([`sys_pointer` + `user_pointer`]). 
This approach helped to;
    - remove redundant size information in `object (freed, allocated) segments`
    - change `object segments` to store only `sys pointer`s
    - switch between user and sys pointers with simple pointer arithmetics.

## Tests
  - `test_zero_allocation`: Tests zero sized allocation does not changes anything on heap.
  - `test_alphabet_allocation`: Tests allocation and writing of English alphabet through custom heap.
  - `test_continuous_allocation`: Tests continuous allocation. Allocates memory with size 1 byte to 10 bytes, and free heap.
  - `test_memory_gap`: Like `test_continuous_allocation` but creates gap by freeing even sized memories.
  - `test_free_space_compaction_by_prev`: Allocates to memory space (p0,p1), and frees p0 then p1 to test free space compaction. 
  - `test_free_space_compaction_by_next`: Allocates to memory space (p0,p1), and frees p1 then p0 to test free space compaction. 
  - `test_free_space_compaction`: Test case is like `test_memory_gap`. Additionally frees 3th pointer/memory space, to check 2nd and 4th is merged with 3th pointer/memory space.
 
## Plans
 - TBD

## References
 - TBD
