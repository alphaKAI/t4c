#ifndef T4C_UTIL_INCLUDED
#define T4C_UTIL_INCLUDED

#define MALLOC_T(T) (T*)xmalloc(sizeof(T))
#define MALLOC_TN(T, N) (T*)xmalloc(sizeof(T) * N)
#define MALLOC_TS(T, S) (T*)xmalloc(S)

#include <stdlib.h>
#include <stdio.h>

static inline void* xmalloc(size_t size) {
  void* mem = malloc(size);

  if (mem == NULL) {
    fprintf(stderr, "Fatal Error - Couldn't allocate heap\n");
    exit(EXIT_FAILURE);
  }

  return mem;
}

#endif
