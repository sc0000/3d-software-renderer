// Copyright 2025 Sebastian Cyliax

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <SDL.h>

#define DOUBLE_PTR_SIZE (sizeof(void*) * 2)

#ifndef DEFAULT_ALIGN
  #if __STDC_VERSION__ >= 201112L
    #ifndef _MSC_VER
      #define DEFAULT_ALIGN alignof(max_align_t)
    #else
      #define DEFAULT_ALIGN DOUBLE_PTR_SIZE
    #endif
  #else
    #define DEFAULT_ALIGN DOUBLE_PTR_SIZE
  #endif
#endif

#define VALUE(value, ...) value
#define SELECT_VALIDATION(_1, _2, validation, ...) validation

#define VALIDATE_PTR(...) SELECT_VALIDATION(__VA_ARGS__, VALIDATE_PTR_VAL, VALIDATE_PTR_VOID)(__VA_ARGS__)

#define VALIDATE_PTR_VAL(ptr, ...)\
do {\
  if (!ptr) {\
    return VALUE(__VA_ARGS__);\
  }\
} while(0);

#define VALIDATE_PTR_VOID(ptr, ...)\
do {\
  if (!ptr) {\
    return;\
  }\
} while (0);

uintptr_t align_size(size_t size, uintptr_t align);

static inline bool is_pow2(uintptr_t p)
{
  uintptr_t mod = p & (p - 1);
  return mod == 0; 
}

typedef struct da_hdr {
  size_t capacity;
  size_t occupied;
  size_t element_size;
} da_hdr_t;

static inline da_hdr_t* darray_get_hdr(void* darray)
{
  uintptr_t hdr_size = align_size(sizeof(da_hdr_t), DEFAULT_ALIGN);
  return darray ? (da_hdr_t*)((uintptr_t)darray - hdr_size) : NULL;
}

void* darray_init(size_t element_size, size_t num);

void* darray_alloc(void* darray, size_t element_size, size_t num);

#define darray_push(darray, element)\
do {\
  _Static_assert(sizeof(*(darray)) == sizeof(element), "darray element size mismatch");\
  \
  (darray) = darray_alloc(darray, sizeof(element), 1);\
  (darray)[darray_size(darray) - 1] = (element);\
}	while (0);

#define darray_insert(darray, pos, element)\
do {\
  if (sizeof(*(darray)) != sizeof(element)) {\
    exit(EXIT_FAILURE);\
  }\
  \
  (darray) = darray_make_space(darray, pos, sizeof(element), 1);\
  (darray)[pos] = (element);\
} while (0);

void* darray_make_space(void* darray, size_t pos, size_t element_size, size_t num);

void darray_pop_last(void* darray);

void darray_pop_first(void* darray);

void darray_clear(void* darray);

size_t darray_size(void* darray);

size_t darray_capacity(void* darray);

void darray_reset_size(void* darray, size_t size);

#define darray_reserve(darray, new_cap)\
do {\
  darray_reserve_base((void**)&(darray), new_cap);\
} while (0);

void darray_reserve_base(void** darray, size_t new_cap);

#define darray_shrink_to_fit(darray)\
do {\
  darray_shrink_to_fit_base((void**)&(darray));\
} while (0);

void darray_shrink_to_fit_base(void** darray);
