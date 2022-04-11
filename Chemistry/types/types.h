#pragma once

#include <stddef.h>
#include <stdbool.h>

#define MAXLEN_OF_SYMBOL 2

#define MAXNUMBER_OF_OXISTATES 8

#define NUMBER_OF_ELEMENTS 118

//id of an elem
typedef char atomic_num_t;

//str of 3 characters that stores a symbol
//this symbol is the simbol of an element
//it is a struct because we want to copy the contents of its instances easily
typedef struct
{
    char symbol[MAXLEN_OF_SYMBOL+1];
}elem_symbol_t;

//oxistate of a compound
//an element cannot have a current oxistate 
typedef char oxistate_t;

#define UNKNOWN_OXISTATE 0

typedef struct
{
    unsigned char n;
    oxistate_t states[MAXNUMBER_OF_OXISTATES];
}oxistates_t;

//real number
typedef double num_t;

//just an element
typedef struct
{
    atomic_num_t atomic_num;
    elem_symbol_t symbol;
    num_t molar_mass;
    oxistates_t possible_oxistates;
}elem_t;

//substance
//it can be a complex substance (it is composed of other substances) or a simple substance
typedef struct substance_t
{
    size_t amount;//amount of substance (it is not in mol, it is just the amount of the substance)

    //oxistate_t cur_oxistate;//current charge of the subtance (oxistates total between its components)

    bool is_simple_substance;//simple substance (just one element) or complex substance (union of more than one element == union of substances)

    union
    {
        elem_t elem;//simple substance

        struct
        {
            size_t nsubstances;
            struct substance_t * substances;
        }compound;//complex substance

    }substance;

    num_t molar_mass;//molar mass of the substance (g/mol) 

}substance_t;

//stores the number of atoms of each element in a substance
typedef struct
{
    size_t bucket[NUMBER_OF_ELEMENTS+1];//the index of an element is its atomic number
    size_t ncomponents;
}components_of_substance_t;

//chemical reaction
typedef struct
{
    struct
    {
        size_t nsubstances;
        substance_t * substances;
    }
    reactants,//substances set number 1
    products;//substances set number 2
}reaction_t;

