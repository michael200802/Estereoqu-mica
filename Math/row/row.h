#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "../types.h"
#include "../commons/commons.h"

bool create_row(row_t* restrict const row, size_t ncols);

void subtract_row(row_t*restrict const row1, const row_t* restrict const row2);

void add_row(row_t*restrict const row1, const row_t* restrict const row2);

void multiply_row(row_t*restrict const row, integer_t num);

void divide_row(row_t* restrict const row, integer_t dividend);

void simplify_row(row_t*restrict const row);

void eliminate_column(row_t* restrict const minuend_row, size_t col, row_t* restrict const subtrahend_row);

bool is_row_all_zero(row_t* restrict const row);

void delete_row(row_t* restrict const row);
