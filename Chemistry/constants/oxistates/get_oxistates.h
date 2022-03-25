#pragma once

#include "../../types/types.h"

//if success, it returns the oxistate of the symbol
//if failure, it returns err (.n == '-1')
extern oxistates_t get_oxistates(atomic_num_t atomic_num);
