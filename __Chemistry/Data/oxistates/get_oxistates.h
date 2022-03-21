#pragma once

#include "../types.h"
#include "../constants.h"

//if success, it returns the oxistate of the symbol
//if failure, it returns err (.n == '-1')
oxistates_t get_oxistate(const char * str);