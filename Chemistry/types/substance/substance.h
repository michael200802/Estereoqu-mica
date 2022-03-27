
#pragma once

#include "../types.h"

#include "../../constants/atomicnum/get_atomicnum.h"
#include "../../constants/oxistates/get_oxistates.h"
#include "../../constants/uma/get_uma.h"

#include "../elem/elem.h"

#include <ctype.h>

#include <stdlib.h>//we are using dinamic memory here

#include <stdio.h>

bool init_substance(const char * str, substance_t * const restrict sub);

bool print_substance(const substance_t * const restrict sub, char * buffer, size_t buffer_len);

void destroy_substance(substance_t * const restrict sub);
