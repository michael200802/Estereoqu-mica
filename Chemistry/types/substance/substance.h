
#pragma once

#include "../types.h"

#include "../elem/elem.h"

#include <ctype.h>

#include <stdlib.h>//we are using dinamic memory here

#include <stdio.h>

extern bool init_substance(const char * str, substance_t * const restrict sub);

#define IS_STR_SUBSTANCE_UNKNOWN_LEN -1
extern bool is_str_substance(const char * restrict str, ssize_t len);

extern size_t print_substance(const substance_t * const restrict sub, char * buffer, size_t buffer_len);

extern void get_components_of_substance(const substance_t * const restrict sub, components_of_substance_t * const restrict comp);

extern void sum_components_of_substance(const components_of_substance_t * const comp1, const components_of_substance_t * const comp2, components_of_substance_t * const result);

//true if they are equal
//otherwise it reaturns false
extern bool compare_components_of_substance(const components_of_substance_t * const comp1, const components_of_substance_t * const comp2);

//unused
extern long long get_nelems_in_substance(const substance_t * const restrict sub, const elem_symbol_t elem);

extern void destroy_substance(substance_t * const restrict sub);
