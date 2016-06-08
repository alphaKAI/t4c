#ifndef T4C_UTIL_INCLUDED
#define T4C_UTIL_INCLUDED

#define MALLOC_T(T) (T*)malloc(sizeof(T))
#define MALLOC_TN(T, N) (T*)malloc(sizeof(T) * N)
#define MALLOC_TS(T, S) (T*)malloc(S)

#endif
