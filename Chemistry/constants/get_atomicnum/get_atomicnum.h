#pragma once

#include "../../types/types.h"

//get atomic num given its symbol
//it returns -1 if failure
extern atomic_num_t get_atomicnum(const elem_symbol_t elem_symbol);
