#pragma once

#include "../types.h"

#include "../substance/substance.h"

#include <ctype.h>

#include <stdlib.h>//we are using dinamic memory here

#include <string.h>//strtok for parsing

bool init_reaction(const char * restrict _reactives, const char * restrict _products, reaction_t * restrict const react);

extern void destroy_reaction(reaction_t * restrict const react);
