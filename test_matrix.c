#include <stdio.h>
#include "Math/matrix/matrix.h"

int main(void)
{
    FILE* stream = fopen("matrix.txt","r");
    matrix_t matrix;

    //Create matrix
    {
        size_t nrows, ncols;

        if(fscanf(stream,"nrows %zu",&nrows) + fscanf(stream,"\nncols %zu",&ncols) < 2)
        {
            puts("No se pudo obtener la longitud de la matriz.");
            return -1;
        }


        if(create_matrix(&matrix,nrows,ncols) == false)
        {
            puts("create_matrix() fallo.");
            return -1;
        }
    }
    fgetc(stream);

    //Fill matrix
    for(size_t i = 0; i < matrix.nrows; i++)
    {
        for(size_t j = 0; j < matrix.ncols; j++)
        {
            if(fscanf(stream,"%zu",&matrix_get_elem(matrix,i,j)) != 1)
            {
                printf("No se pudo obtener el elemento[%zu][%zu]\n",i,j);
                return -1;
            }
        }
        fgetc(stream);
    }

    fclose(stream);

    show_matrix(&matrix);

    make_matrix_REF(&matrix);
    show_matrix(&matrix);

    make_matrix_RREF(&matrix);
    show_matrix(&matrix);

    delete_matrix(&matrix);

    return 0;
}