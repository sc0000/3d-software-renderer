// Copyright 2025 Sebastian Cyliax

#include <stdlib.h>
#include <memory.h>
#include "darray.h"

uintptr_t align_size(size_t size, uintptr_t align)
{
  if (!is_pow2(align)) {
    exit(EXIT_FAILURE);
  }

  uintptr_t size_ptr = (uintptr_t)size;
  uintptr_t mod = size_ptr & (align - 1);

  if (mod != 0)
    size_ptr += align - mod;

  return size_ptr;
}

void* darray_init(size_t element_size, size_t num)
{
  uintptr_t hdr_size = align_size(sizeof(da_hdr_t), DEFAULT_ALIGN);
  
  size_t raw_size = hdr_size + element_size * num;
  void* raw = malloc(raw_size);

  if (!raw) exit(EXIT_FAILURE);

  da_hdr_t* hdr = (da_hdr_t*)raw;
  hdr->capacity = num;
  hdr->occupied = 0;
  hdr->element_size = element_size;

  void* buf = (void*)((uintptr_t)hdr + hdr_size);

  return buf;
}

void* darray_alloc(void* darray, size_t element_size, size_t num)
{
  if (!darray)
    darray = darray_init(element_size, num);

  da_hdr_t* hdr = darray_get_hdr(darray);
  uintptr_t hdr_size = align_size(sizeof(da_hdr_t), DEFAULT_ALIGN);
  size_t needed_size = (size_t)hdr->occupied + num;

  if (needed_size > hdr->capacity) {
    size_t double_cap = hdr->capacity * 2;
    size_t new_cap = (needed_size > double_cap) ? needed_size : double_cap;
    size_t new_size = (size_t)hdr_size + new_cap * element_size;

    void* new_raw = realloc(hdr, new_size);
    
    if (!new_raw) {
        exit(EXIT_FAILURE);
    }

    hdr = (da_hdr_t*)new_raw;
    hdr->capacity = new_cap; 
  }

  hdr->occupied += num;

  return (void*)((uintptr_t)hdr + hdr_size);
}

void* darray_make_space(void* darray, size_t pos, size_t element_size, size_t num)
{
  darray = darray_alloc(darray, element_size, num);

  // darray_alloc() guarantees a valid darray (or crash).

  uintptr_t ptr = (uintptr_t)darray;
  da_hdr_t* hdr = darray_get_hdr(darray);

  uintptr_t mem_occ = (uintptr_t)(hdr->occupied * element_size);
  uintptr_t mem_left = (uintptr_t)(pos * element_size);
  uintptr_t mem_right = mem_occ - mem_left;
  uintptr_t old_pos = ptr + mem_left;
  uintptr_t offset = (uintptr_t)(element_size * num);
  uintptr_t new_pos = old_pos + offset;

  memcpy((void*)new_pos, (void*)old_pos, mem_right);

  return darray;
}

void darray_pop_last(void* darray)
{
  VALIDATE_PTR(darray);

  da_hdr_t* hdr = darray_get_hdr(darray);
  hdr->occupied -= 1;
}

void darray_pop_first(void* darray)
{
  VALIDATE_PTR(darray);

  da_hdr_t* hdr = darray_get_hdr(darray);
  uintptr_t second_element_ptr = (uintptr_t)darray + (uintptr_t)hdr->element_size;
  hdr->occupied -= 1;
  size_t size = hdr->occupied * hdr->element_size;

  memcpy(darray, (void*)second_element_ptr, size);
}

void darray_clear(void* darray)
{
  VALIDATE_PTR(darray);

  da_hdr_t* hdr = darray_get_hdr(darray);
  hdr->occupied = 0;
}

void darray_reset_size(void* darray, size_t size)
{
  VALIDATE_PTR(darray);

  da_hdr_t* hdr = darray_get_hdr(darray);

  if (size > hdr->occupied) return;

  hdr->occupied = size;
}

size_t darray_size(void* darray)
{
  VALIDATE_PTR(darray, 0);

  da_hdr_t* hdr = darray_get_hdr(darray);
  return hdr->occupied;
}

size_t darray_capacity(void* darray)
{
  VALIDATE_PTR(darray, 0);

  da_hdr_t* hdr = darray_get_hdr(darray);
  return hdr->capacity;
}

void darray_reserve_base(void** darray, size_t new_cap)
{
  VALIDATE_PTR(*darray);

  da_hdr_t* hdr = darray_get_hdr(*darray);

  if (new_cap < hdr->capacity) return;

  uintptr_t hdr_size = align_size(sizeof(da_hdr_t), DEFAULT_ALIGN);
  size_t new_size = (size_t)hdr_size + new_cap * hdr->element_size;

  void* new_raw = realloc(hdr, new_size);
    
  VALIDATE_PTR(new_raw);

  hdr = (da_hdr_t*)new_raw;
  hdr->capacity = new_cap; 

  *darray = (void*)((uintptr_t)hdr + hdr_size);
}

void darray_shrink_to_fit_base(void** darray)
{
  VALIDATE_PTR(*darray);

  da_hdr_t* hdr = darray_get_hdr(*darray);
  uintptr_t hdr_size = align_size(sizeof(da_hdr_t), DEFAULT_ALIGN);

  size_t new_cap = hdr->occupied;
  size_t new_size = (size_t)hdr_size + hdr->element_size * new_cap; 
  void* new_raw = realloc(hdr, new_size);
  
  if (!new_raw) return;

  hdr = (da_hdr_t*)new_raw;
  hdr->capacity = new_cap;

  *darray = (void*)((uintptr_t)hdr + hdr_size);
}
