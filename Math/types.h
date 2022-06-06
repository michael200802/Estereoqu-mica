#pragma once

#include <stddef.h>

typedef ssize_t integer_t;

typedef struct
{
    integer_t* arr;
    size_t len;
}row_t;

typedef struct
{
    row_t* rows;
    size_t nrows;
    size_t ncols;
}matrix_t;

