#pragma once

#include <stdbool.h>
#include "../Chemistry/types/reaction/reaction.h"

int start_app(void);

typedef enum {INPUT_NOERRROR,INPUT_ERROR_MUTEX,INPUT_ERROR_CONDV_SIGNAL,INPUT_ERROR_CONDV_WAIT,INPUT_ERROR_READING,INPUT_ERROR_WND_CLOSED} input_errcode_t;

typedef struct
{
    size_t nsubstances;
    struct
    {
        num_t mol;
        num_t molar_mass;
    }*substances;
}var_arr_t;

typedef struct
{
    input_errcode_t error_code;
    reaction_t react;
    var_arr_t var_arr_reactants, var_arr_products;//variables
}input_t;

#define init_var_arr(var_arr,nsubs)\
    var_arr.nsubstances = nsubs;\
    var_arr.substances = calloc(var_arr.nsubstances,nsubs*sizeof(substance_t));

#define destroy_var_arr(var_arr)\
    free(var_arr.substances);

input_errcode_t set_cur_input(const reaction_t * restrict const react, const var_arr_t * restrict const reactants, const var_arr_t * restrict const products);

input_t get_cur_input();

void free_cur_input();

int end_input(); 

extern void show_output(const char * str);

extern int end_app(void);

extern void msg_app(const char * MBTitle, const char * MBMsg);
