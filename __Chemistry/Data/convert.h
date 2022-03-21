#pragma once

#include "types.h"

//get element from a inmutable string
element_t convert_strto_elem(const char * str);

//get compound which name is the string passed as argument
compound_t convert_strto_compound(char * str);