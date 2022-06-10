#pragma once

#include "../types.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../row/row.h"

bool create_matrix(matrix_t* const restrict matrix, size_t nrows, size_t ncols);

#define matrix_get_row(matrix,i) matrix.rows[i]

#define matrix_get_elem(matrix,i,j) matrix.rows[i].arr[j]

void show_matrix(const matrix_t* const matrix);

void swap_rows(matrix_t* const restrict matrix, size_t row1, size_t row2);

//By swaping rows and making the elements below a pivot zero, this function try to make the matrix REF (row echelon form)
void make_matrix_REF(matrix_t* const restrict matrix);

//By swaping rows and making the elements below a pivot zero, this function try to make the matrix RREF (reduced-row echelon form)
//But it will not make all pivots be equal to 1, it at least simplifies all rows
void make_matrix_RREF(matrix_t* const restrict matrix);

void delete_matrix(matrix_t* const restrict matrix);
