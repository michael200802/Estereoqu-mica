#include "row.h"

inline bool create_row(row_t* restrict const row, size_t ncols)
{
    *row = (row_t){.arr = malloc(sizeof(integer_t)*ncols), .len = ncols};

    return (row->arr == NULL ? false : true);
}

inline void subtract_row(row_t*restrict const row1, const row_t* restrict const row2)
{
    for(size_t i = 0; i < row1->len; i++)
    {
        row1->arr[i] -= row2->arr[i];
    }
}

inline void multiply_row(row_t*restrict const row, integer_t num)
{
    for(size_t i = 0; i < row->len; i++)
    {
        row->arr[i] *= num;
    }
}

inline void divide_row(row_t* restrict const row, integer_t dividend)
{
    if(dividend == 0)
    {
        return;
    }
    for(size_t i = 0; i < row->len; i++)
    {
        row->arr[i] /= dividend;
    }
}

inline void simplify_row(row_t*restrict const row)
{
    size_t i = 0;
    for(; row->arr[i] == 0 && i < row->len; i++);
    if(i == row->len)
    {
        return;
    }

    integer_t gcd = row->arr[i];
    for(i += 1; i < row->len; i++)
    {
        if(row->arr[i] == 0)
        {
            continue;
        }
        gcd = get_gcd(gcd,row->arr[i]);
    }

    divide_row(row,gcd);
}

inline bool is_row_all_zero(row_t* restrict const row)
{
    for(size_t i = 0, maxi = row->len; i < maxi; i++)
    {
        if(row->arr[i] != 0)
        {
            return false;
        }
    }
    return true;
}

inline void eliminate_column(row_t* restrict const minuend_row, size_t col, row_t* restrict const subtrahend_row)
{
    integer_t minuend = minuend_row->arr[col];
    integer_t subtrahend = subtrahend_row->arr[col];
    if(minuend == 0 || subtrahend == 0)
    {
        return;
    }

    integer_t lcm = get_lcm(minuend,subtrahend);

    multiply_row(minuend_row,lcm/minuend);
    multiply_row(subtrahend_row,lcm/subtrahend);

    subtract_row(minuend_row,subtrahend_row);

    simplify_row(minuend_row);
    divide_row(subtrahend_row,lcm/subtrahend);
}

inline void delete_row(row_t* restrict const row)
{
    free(row->arr);
}