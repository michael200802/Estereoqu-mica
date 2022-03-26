#pragma once

#include "../types.h"

#include "../../constants/atomicnum/get_atomicnum.h"
#include "../../constants/oxistates/get_oxistates.h"
#include "../../constants/uma/get_uma.h"

#include <ctype.h>

extern bool init_elem_symbol(const char * const restrict symbol, elem_symbol_t * const restrict elem_symbol);

extern bool init_elem(const elem_symbol_t symbol, elem_t * const restrict elem);
