#pragma once

#include "../types.h"

#include "../substance/substance.h"

#include <ctype.h>

#include <stdlib.h>//we are using dinamic memory here

#include <string.h>//strtok for parsing

typedef enum {
    REACTION_ERR_INIT_UNKNOWN_SYMBOL = false, REACTION_ERR_INIT_SUCCESS = true, REACTION_ERR_INIT_UNBALANCED, REACTION_ERR_INIT_CANNOT_BALANCE,
    REACTION_ERR_BALANCE_SUCESS, REACTION_ERR_BALANCE_ALREADY_BALANCED, REACTION_ERR_BALANCE_CANNOT_BALANCE,
    } reaction_err_t;

reaction_err_t init_reaction(const char * restrict _reactants, const char * restrict _products, reaction_t * restrict const react, bool balance);

reaction_err_t balance_reaction(reaction_t * restrict const react);

typedef enum {REACTION_IS_BALANCED, REACTION_IS_NOT_BALANCED, REACTION_CANNOT_BE_BALANCED} is_reaction_balanced_returnv_t; 
extern is_reaction_balanced_returnv_t is_reaction_balanced(const reaction_t * restrict const react);

extern reaction_err_t init_balanced_reaction(const char * restrict _reactants, const char * restrict _products, reaction_t * restrict const react);

extern void destroy_reaction(reaction_t * restrict const react);
