#pragma once

#include <stdbool.h>
#include "../Chemistry/types/reaction/reaction.h"

typedef enum {INPUT_NOERRROR,INPUT_ERROR_MUTEX,INPUT_ERROR_CONDV_SIGNAL,INPUT_ERROR_CONDV_WAIT,INPUT_ERROR_READING,INPUT_ERROR_WND_CLOSED} input_errcode_t;

typedef struct
{
    size_t nsubstances;
    struct
    {
        num_t mol;
        num_t molar_mass;
        char unit;//'k' for kg, 'g' for g, and 'm' for mol
    }*substances;
}var_arr_t;

typedef struct
{
    input_errcode_t error_code;
    const reaction_t * react;
    const var_arr_t * var_arr_reactants, * var_arr_products;//variables
}input_t;

#define init_var_arr(var_arr,nsubs)\
    var_arr.nsubstances = nsubs;\
    var_arr.substances = calloc(var_arr.nsubstances,nsubs*sizeof(substance_t));\
    if(var_arr.substances == NULL)\
    {\
        msg_app("Error critico:","calloc() ha fallado; no se puede obtener memoria.");\
        exit(EXIT_FAILURE);\
    }\

#define destroy_var_arr(var_arr)\
    free(var_arr.substances);

//input API-------------------------------------------------------

//send new input from the main window
//waits until main() shows the output
input_errcode_t send_new_input(const reaction_t * restrict const react, const var_arr_t * restrict const reactants, const var_arr_t * restrict const products);

input_t get_cur_input(void);//allows main to get the current input

void send_endwnd_signal(void);//allows the GUI to send a signal in order to notify that the main window was closed 

int end_input(void); //free resources

//input API-------------------------------------------------------

//output API------------------------------------------------------

void catstr_to_output_buffer(const char * str, size_t str_len); //cats a str to the buffer

void print_num_in_output_buffer(num_t num);

//void print_reaction_in_output_buffer()

void flush_output_buffer(void); //sends the buffer to the GUI

void end_output(void); //free resources

//output API------------------------------------------------------

//GUI API---------------------------------------------------------
extern int start_app(void);//starts the application (creates GUI)

void * show_output(void * args);

extern void msg_app(const char * MBTitle, const char * MBMsg);//msg box: mostly used for showing errors

extern int end_app(void);//free GUI resources and then calls end_input()
//GUI API---------------------------------------------------------
