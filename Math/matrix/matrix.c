#include "matrix.h"

inline bool create_matrix(matrix_t* const restrict matrix, size_t nrows, size_t ncols)
{
    *matrix = (matrix_t){.nrows = nrows, .ncols = ncols};

    matrix->rows = malloc(nrows*sizeof(row_t));

    if(matrix->rows == NULL) return false;

    for(size_t i = 0; i < nrows; i++)
    {
        if(create_row(&matrix->rows[i],ncols) == false)
        {
            matrix->nrows = i;
            delete_matrix(matrix);

            return false;
        }
    }

    return true;
}

#include <stdio.h>
void show_matrix(const matrix_t* const matrix)
{
    char buffer[100];
    size_t max_len = 0;
    for(size_t i = 0; i < matrix->nrows; i++)
    {
        for(size_t j = 0; j < matrix->ncols; i++)
        {
            size_t newlen = sprintf(buffer,"%lli",matrix_get_elem((*matrix),i,j));
            if(max_len < newlen)
            {
                max_len = newlen;
            }
        }
    }
    for(size_t i = 0; i < matrix->nrows; i++)
    {
        for(size_t j = 0; j < matrix->ncols; j++)
        {
            size_t strlen = sprintf(buffer,"%lli",matrix_get_elem((*matrix),i,j));
            size_t nspaces = max_len-strlen;
            for(size_t i = 0, maxi = nspaces/2+nspaces%2; i < maxi; i++)
            {
                putchar(' ');
            }
            fputs(buffer,stdout);
            for(size_t i = 0, maxi = nspaces/2; i < maxi; i++)
            {
                putchar(' ');
            }
        }
    }
}

inline void swap_rows(matrix_t* const restrict matrix, size_t row1, size_t row2)
{
    row_t aux = matrix->rows[row1];
    matrix->rows[row1] = matrix->rows[row2];
    matrix->rows[row2] = aux;
}

inline void make_matrix_REF(matrix_t* const restrict matrix)
{
    //Go through all columns
    for(
        size_t cur_col = 0, max_cur_col = matrix->ncols, cur_row = 0, max_cur_row = matrix->nrows;
        cur_col < max_cur_col; 
        cur_col++
        )
    {
        //Try to set pivot
        size_t pivot_val = matrix_get_elem((*matrix),cur_row,cur_col);
        if(pivot_val == 0)
        {
            size_t row_with_newpivot;
            for(row_with_newpivot = cur_row+1; row_with_newpivot < max_cur_row; row_with_newpivot++)
            {
                pivot_val = matrix_get_elem((*matrix),row_with_newpivot,cur_col);
                if(pivot_val != 0)
                {
                    break;//pivot found
                }
            }
            if(pivot_val == 0)
            {
                continue;//Go try with the next element in the current row (go to the next column)
            }
            swap_rows(matrix,cur_row,row_with_newpivot);//We find it, now swap the rows so we have a pivot to use
        }

        //Eliminate elements below the pivot using the pivot itself
        for(size_t i = cur_row+1, maxi = matrix->nrows-1; i < maxi; i++)
        {
            eliminate_column(&matrix_get_row((*matrix),i),cur_col,&matrix_get_row((*matrix),cur_row));
        }

        //Go to the next row
        cur_row++;
        if(cur_row == max_cur_row)
        {
            break;
        }
    }

    //Delete all-zero rows
    size_t original_nrows = matrix->nrows;
    for(size_t i = 0, maxi = original_nrows; i < maxi; i++)
    {
        row_t* cur_row = &matrix_get_row((*matrix),i);
        bool is_all_zero = is_row_all_zero(cur_row);

        if(is_all_zero)
        {
            delete_row(cur_row);
            for(size_t j = i, maxj = maxi-1; j < maxj; j++)
            {
                matrix_get_row((*matrix),j) = matrix_get_row((*matrix),j+1);
            }
            matrix->nrows--;
            maxi--;
        }
    }
    if(original_nrows != matrix->nrows)
    {
        matrix->rows = realloc(matrix->rows,sizeof(row_t)*matrix->nrows);
    }
}

inline void make_matrix_RREF(matrix_t* const restrict matrix)
{
    make_matrix_REF(matrix);

    for(
        size_t cur_col = 1, max_cur_col = matrix->ncols, cur_row = 0, max_cur_row = matrix->nrows;
        cur_col < max_cur_col; 
        cur_col++
        )
    {
        integer_t pivot_val = matrix_get_elem((*matrix),cur_row,cur_col);
        if(pivot_val == 0)
        {
            continue;//go try with the next element (next column)
        }

        simplify_row(&matrix_get_row((*matrix),cur_row));//We need the pivot to be the closest to one

        //Eliminate elements above the current pivot
        for(size_t i = 0; i < cur_row; i++)
        {
            eliminate_column(&matrix_get_row((*matrix),i),cur_col,&matrix_get_row((*matrix),cur_row));
        }

        //Next row
        cur_row++;
        if(cur_row == max_cur_row)
        {
            break;//well, there isn't a "next row" UnU
        }
    }
}

inline void delete_matrix(matrix_t* const restrict matrix)
{
    for(size_t i = 0; i < matrix->nrows; i++)
    {
        delete_row(&matrix->rows[i]);
    }
}
