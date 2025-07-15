/********************************************************************//**
 *
 *  @pace3d     Parallel Algorithms for Crystal Evolution in 3D
 *
 *  @copyright  2001 IAF - Institut fuer Angewandte Forschung @n
 *                   University of Applied Sciences @n
 *                   Karlsruhe @n
 *
 *  @file       mem.h
 *  @ingroup    wrapper
 *  @brief      Helpers for allocating and freeing of memory.
 *
 ********************************************************************
 *
 *  @lastmodified 10.12.07    Michael Selzer
 *  @lastmodified 20.10.11    Michael Selzer
 *  @lastmodified 31.03.15    Constantin Heisler
 *
 ********************************************************************/

#ifndef LIB_WRAPPER_MEM_H
#define LIB_WRAPPER_MEM_H

static inline __attribute__ ((malloc)) void* Malloc_internal(size_t size, const char* const file, int line, const char* const function) {
  void *ptr;

#ifdef DEBUG
  if (size == 0) {
    mydebug_internal(file, line, function, "trying to allocate 0 bytes.");
    return NULL;
  }
#endif
#ifdef VALGRIND_DEBUG
  size += 8 - (size % 8);
  if ((ptr=calloc(size, 1)) == NULL) {
#else
  if ((ptr=malloc(size)) == NULL) {
#endif
    myexit(ERROR_BUG, "%s() (File: %s:%d): error allocating %zd bytes.", function, file, line, size);
  }

  return ptr;
}

#define Malloc(size) Malloc_internal(size, __FILE__, __LINE__, __func__)

static inline void* Realloc_internal(void* ptr, size_t size, const char* const file, int line, const char* const function) {
#ifdef VALGRIND_DEBUG
  size += 8 - (size % 8);
#endif
  if ((ptr=realloc(ptr,size)) == NULL) {
#ifdef DEBUG
    if (size == 0) {
      mydebug_internal(file, line, function, "trying to reallocate 0 bytes.");
      return NULL;
    }
#endif
    myexit(ERROR_BUG, "%s() (File: %s:%d): error reallocating %zd bytes.", function, file, line, size);
  }

  return ptr;
}

#define Realloc(ptr, size) Realloc_internal(ptr, size, __FILE__, __LINE__, __func__)

static inline __attribute__ ((malloc)) void* Calloc_internal(size_t n, size_t size, const char* const file, int line, const char* const function) {
  void *ptr;

#ifdef DEBUG
  if (size == 0) {
    mydebug_internal(file, line, function, "trying to allocate 0 bytes.");
    return NULL;
  }
#endif
#ifdef VALGRIND_DEBUG
  size += 8 - (size % 8);
#endif
  if ((ptr=calloc(n,size)) == NULL) {
    myexit(ERROR_BUG, "%s() (File: %s:%d): error allocating %zd blocks with %zd bytes (%zd bytes total).", function, file, line, n, size, n*size);
  }

  return ptr;
}

#define Calloc(n, size) Calloc_internal((n), size, __FILE__, __LINE__, __func__)

#define MallocFV(n)     (REAL*)Calloc((n), sizeof(REAL))
#define MallocIV(n)     (long*)Calloc((n), sizeof(long))
#define MallocULV(n)   (ulong*)Calloc((n), sizeof(ulong))
#define MallocS(n)      (char*)Calloc((n), sizeof(char))
#define MallocBV(n)     (bool*)Calloc((n), sizeof(bool))


static inline __attribute__ ((malloc)) void** MallocM_internal(size_t n, size_t m, size_t typesize, size_t ptrsize, const char* const file, int line, const char* const function) {
  void **ptr = (void**)Malloc_internal(n*ptrsize, file, line, function);

  for (size_t i=0; i<n; i++) {
    ptr[i] = (void*)Calloc_internal(m, typesize, file, line, function);
  }
  return ptr;
}

#define MallocM(n, m, type) (type**)MallocM_internal(n, m, sizeof(type), sizeof(type*), __FILE__, __LINE__, __func__)

#define MallocFM(n, m) MallocM(n, m, REAL)
#define MallocIM(n, m) MallocM(n, m, long)
#define MallocSM(n, m) MallocM(n, m, char)
#define MallocBM(n, m) MallocM(n, m, bool)

static inline __attribute__ ((malloc)) void*** Malloc3M_internal(size_t n, size_t m, size_t o, size_t typesize, size_t ptrsize, size_t ptrsize2, const char* const file, int line, const char* const function) {
  void ***ptr = (void***)Malloc_internal(n*ptrsize2, file, line, function);

  for (size_t i=0; i<n; i++) {
    ptr[i] = MallocM_internal(m, o, typesize, ptrsize, file, line, function);
  }

  return ptr;
}

#define Malloc3M(n, m, o, type) (type***)Malloc3M_internal(n, m, o, sizeof(type), sizeof(type*), sizeof(type**), __FILE__, __LINE__, __func__)

#define MallocF3M(n, m, o) Malloc3M(n, m, o, REAL)
#define MallocI3M(n, m, o) Malloc3M(n, m, o, long)
#define MallocS3M(n, m, o) Malloc3M(n, m, o, char)


static inline __attribute__ ((malloc)) void**** Malloc4M_internal(size_t n, size_t m, size_t o, size_t p, size_t typesize, size_t ptrsize, size_t ptrsize2, size_t ptrsize3, const char* const file, int line, const char* const function) {
  void ****ptr = (void****)Malloc_internal(n*ptrsize3, file, line, function);

  for (size_t i=0; i<n; i++) {
    ptr[i] = Malloc3M_internal(m, o, p, typesize, ptrsize, ptrsize2, file, line, function);
  }

  return ptr;
}

#define Malloc4M(n, m, o, p, type) (type****)Malloc4M_internal(n, m, o, p, sizeof(type), sizeof(type*), sizeof(type**), sizeof(type***), __FILE__, __LINE__, __func__)

#define MallocF4M(n, m, o, p) Malloc4M(n, m, o, p, REAL)
#define MallocI4M(n, m, o, p) Malloc4M(n, m, o, p, long)
#define MallocS4M(n, m, o, p) Malloc4M(n, m, o, p, char)


static inline void Free_internal(void* ptr, const char* const file, int line, const char* const function) {
  if (ptr == NULL) {
    mydebug_internal(file, line, function, "pointer already freed.");
  } else {
    free(ptr);
  }
}

#define Free(ptr) \
do { \
  Free_internal(ptr, __FILE__, __LINE__, __func__); \
  ptr = NULL; \
} while (0)


static inline void FreeM_internal(void** ptr, size_t n, const char* const file, int line, const char* const function) {
  if (ptr == NULL) {
    mydebug_internal(file, line, function, "pointer already freed.");
  } else {
    for (size_t i=0; i<n; i++) {
      Free_internal(ptr[i], file, line, function);
    }
    free(ptr);
  }
}

#define FreeM(ptr, n) \
do { \
  FreeM_internal((void**)ptr, n, __FILE__, __LINE__, __func__); \
  ptr = NULL; \
} while (0)


static inline void Free3M_internal(void*** ptr, size_t n, size_t m, const char* const file, int line, const char* const function) {
  if (ptr == NULL) {
    mydebug_internal(file, line, function, "pointer already freed.");
  } else {
    for (size_t i=0; i<n; i++) {
      FreeM_internal(ptr[i], m, file, line, function);
    }
    free(ptr);
  }
}

#define Free3M(ptr, n, m) \
do { \
  Free3M_internal((void***)ptr, n, m, __FILE__, __LINE__, __func__); \
  ptr = NULL; \
} while (0)


static inline void Free4M_internal(void**** ptr, size_t n, size_t m, size_t o, const char* const file, int line, const char* const function) {
  if (ptr == NULL) {
    mydebug_internal(file, line, function, "pointer already freed.");
  } else {
    for (size_t i=0; i<n; i++) {
      Free3M_internal(ptr[i], m, o, file, line, function);
    }
    free(ptr);
  }
}

#define Free4M(ptr, n, m, o) \
do { \
  Free4M_internal((void****)ptr, n, m, o, __FILE__, __LINE__, __func__); \
  ptr = NULL; \
} while (0)

#if defined(DEBUG) || defined(VALGRIND_DEBUG)
/** @brief Copy a string and returns a pointer to the newly allocated memory.
  *
  * The allocated memory should be freed with Free()
  */
static inline char* Strdup_internal(const char* src, const char* const file, int line, const char* const function) {
  char *dst;

  if (src == NULL) {
    myexit(ERROR_BUG, "%s() (File: %s:%d): tried to duplicate string which is NULL.", function, file, line);
  }

#ifdef VALGRIND_DEBUG
  dst = strcpy(MallocS(strlen(src)+1), src);
#else
  if ((dst = strdup(src)) == NULL) {
    myexit(ERROR_BUG, "%s() (File: %s:%d): error allocating %zd bytes.", function, file, line, strlen(src)+1);
  }
#endif

  return dst;
}

#define Strdup(src) Strdup_internal(src, __FILE__, __LINE__, __func__)

/** @brief Copy a string and returns a pointer to the newly allocated memory.
  *
  * The allocated memory should be freed with Free()
  */
static inline char* Strndup_internal(const char* src, size_t n, const char* const file, int line, const char* const function) {
  char *dst;

  if (src == NULL) {
    myexit(ERROR_BUG, "%s() (File: %s:%d): tried to duplicate string which is NULL.", function, file, line);
  }

#if defined VALGRIND_DEBUG || (defined(_WIN32) || defined(_WIN64))
  dst = strncpy(MallocS(n+1), src, n);
#else
  if ((dst = strndup(src, n)) == NULL) {
    myexit(ERROR_BUG, "%s() (File: %s:%d): error allocating %zd bytes.", function, file, line, n+1);
  }
#endif

  return dst;
}

#define Strndup(src, n) Strndup_internal(src, n, __FILE__, __LINE__, __func__)

#else

#define Strdup(src) strdup(src)

#if !defined(_WIN32) && !defined(_WIN64)
#define Strndup(src, n) strndup(src, (n))
#else
#define Strndup(src, n) strncpy(MallocS(n+1), src, n)

#define posix_memalign(p, a, s)  (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)

#endif

#endif

#if defined(_WIN32) || defined(_WIN64)
static inline char* stpcpy(char* dst, const char* src) {
  size_t dst_len = strlen(dst);
  size_t src_len = strlen(src);
  strcpy(&dst[dst_len], src);
  return &dst[dst_len+src_len];
}
#endif

#ifdef DEBUG

static inline void* Memmove_internal(void *dst, const void *src, size_t n, const char* const file, int line, const char* const function) {
  if (!dst) {
    myexit(ERROR_BUG, "%s() (File: %s:%d) : Memmove: dst == NULL", function, file, line);
  }
  if (!src) {
    myexit(ERROR_BUG, "%s() (File: %s:%d) : Memmove: src == NULL", function, file, line);
  }
  if (n == 0) {
    mydebug_internal(file, line, function, "Memmove: n == 0");
  }

  return memmove(dst, src, n);
}

#define Memmove(dst, src, n) Memmove_internal(dst, src, n, __FILE__, __LINE__, __func__)

static inline void* Memcpy_internal(void *dst, const void *src, size_t n, const char* const file, int line, const char* const function) {
  if (!dst) {
    myexit(ERROR_BUG, "%s() (File: %s:%d) : Memcpy: dst == NULL", function, file, line);
  }
  if (!src) {
    myexit(ERROR_BUG, "%s() (File: %s:%d) : Memcpy: src == NULL", function, file, line);
  }
  if (n == 0) {
    mydebug_internal(file, line, function, "Memcpy: n == 0");
  }

  return memcpy(dst, src, n);
}

#define Memcpy(dst, src, n) Memcpy_internal(dst, src, n, __FILE__, __LINE__, __func__)

#else

#define Memmove(dst, src, n) memmove(dst, src, n)
#define Memcpy(dst, src, n) memcpy(dst, src, n)

#endif

/** @brief Reorder a memory segment inside a memory block to another position and does not overwrite the destination segment.
  *
  * Target memory is moved to the previous occupied memory from the moved segment. This function uses a buffer
  * of size. It is assumed that src and dst are inside the same memory allocation. The destination segment must
  * lie inside the allocated memory block.
  *
  * @param dst pointer to the position where the segment should be shifted to
  * @param src pointer to the src of the memory segment to move
  * @param size size of the memory segment in bytes
  */
static inline void MemReorder(void* dst, void* src, size_t size) {
  size_t  relocationsize;     // mem moved into space of shifted mem
  void   *relocationdst;
  void   *relocationsrc;

#if DEBUG
  if (src == NULL || dst == NULL) myexit(ERROR_BUG, "MemReorder(): NULL argument! Not allowed");
#endif

  if (size == 0 || src == dst) return;

  // fill buffer with moved objects so they will not be overwritten
  void *buffer = Memcpy(Malloc(size), src, size);

  // move dst area depending of shift direction
  if (src < dst) {
    relocationsize = (size_t)dst - (size_t)src;
    relocationdst  = src;
    relocationsrc  = (char*)src + size;
  } else {
    relocationsize = (size_t)src - (size_t)dst;
    relocationdst  = (char*)src + (size - relocationsize);
    relocationsrc  = dst;
  }
  // move mem at dst to old mem position so they will not be overwritten
  Memmove(relocationdst, relocationsrc, relocationsize);

  // cpy mem in buffer to dst
  Memcpy(dst, buffer, size);

  Free(buffer);
}

/** @brief Swap parts of memory.
  *
  * @param m1    pointer to the first memory segment
  * @param m2    pointer to the second memory segment
  * @param size  size of the memory segment in bytes
  */
static inline void Memswap(void* m1, void* m2, size_t size) {
  if (m1 == m2) return;

  char *p = (char*) m1;
  char *q = (char*) m2;

  while (size--) {
    char t = *p;
    *p++ = *q;
    *q++ = t;
  }
}

#endif
