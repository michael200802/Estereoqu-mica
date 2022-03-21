#pragma once

#include <stddef.h>

#define MAXLEN_OF_SYMBOL 2

#define MAXNUMBER_OF_OXISTATES 8

#define NUMBER_OF_ELEMENTS 118

typedef double num_t;

typedef struct
{
    char n;
    char states[MAXNUMBER_OF_OXISTATES];
}oxistates_t;

typedef struct
{
    char symbol[MAXLEN_OF_SYMBOL+1];
    oxistates_t oxistates;
    num_t mass;
    num_t mol;
}element_t;

typedef struct
{
    size_t size;
    element_t * array;
}element_array_t;

typedef struct
{
    element_array_t elemsarr;
    num_t mol;
    num_t pm;
    char * name;
}compound_t;

typedef struct
{
    size_t size;
    compound_t * array;
}compound_array_t;

typedef struct
{
    compound_array_t reactants;

    compound_array_t products;

    char * str;

}reaction_t;

typedef struct
{
    size_t nreactants;
    compound_t * reactants;

    size_t nproducts;
    compound_t * products;
}variables_t;