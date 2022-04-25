#pragma once

#include "../types.h"

#include "../../constants/get_elem_symbol/get_elem_symbol.h"
#include "../../constants/get_atomicnum/get_atomicnum.h"
#include "../../constants/get_oxistates/get_oxistates.h"
#include "../../constants/get_uma/get_uma.h"

#include <ctype.h>

extern bool init_elem(const elem_symbol_t symbol, elem_t * const restrict elem);

#define elem_get_atomicnum(elem) elem.atomic_num

#define elem_get_oxistates(elem) get_oxistates(elem_get_atomicnum(elem))

#define elem_get_symbol(elem) get_elem_symbol(elem_get_atomicnum(elem))

#define elem_get_uma(elem) get_uma(elem_get_atomicnum(elem))

