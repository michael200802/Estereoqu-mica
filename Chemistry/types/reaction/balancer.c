#include "reaction.h"

void show_matrix(ssize_t** matrix, size_t nrows, size_t ncols)
{
    static size_t counter = 0;

    printf("Show matrix for the %zu time\n\n",++counter);
    for(size_t i = 0; i < nrows; i++)
    {
        for(size_t j = 0; j < ncols; j++)
        {
            printf("  %lli[%zu][%zu]  ",matrix[i][j],i,j);
        }
        putchar('\n');
    }
    putchar('\n');

}

ssize_t get_gcd(ssize_t num1, ssize_t num2)
{
    ssize_t remainder;
    do
    {
        remainder = num1%num2;
        num1 = num2;
        num2 = remainder;
    } while (remainder != 0);

    return num1;
}

ssize_t get_lcm(ssize_t num1, ssize_t num2)
{
    ssize_t product = num1*num2;
    if(product < 0) product*=-1;

    return product/get_gcd(num1,num2);
}

//returns the solutions
bool solve_matrix(ssize_t** matrix, size_t nrows, size_t ncols)
{
    //make all elements in the main diagonal non-zero
    for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
    {
        //matrix[cur_row][cur_col] is the current element of the diagonal to make non-zero

        if(matrix[cur_row][cur_col] != 0)
        {
            continue;
        }

        for(size_t i = 0, j = 0; i < nrows; i++, j = i)
        {
            if(i != cur_row || j != cur_col)
            {
                if(matrix[i][cur_col] != 0)
                {
                    for(size_t j = 0; j < ncols; j++)
                    {
                        ssize_t aux = matrix[i][j];
                        matrix[i][j] = matrix[cur_row][j];
                        matrix[cur_row][j] = aux;
                    }
                    break;
                }
            }
        }
    }
    for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
    {
        //matrix[cur_row][cur_col] is the current element of the diagonal to make non-zero

        for(size_t i = 0, j = 0; i < nrows; i++, j = i)
        {
            if(i != cur_row || j != cur_col)
            {
                if(matrix[i][cur_col] != 0 && matrix[cur_row][j] != 0)
                {
                    for(size_t j = 0; j < ncols; j++)
                    {
                        ssize_t aux = matrix[i][j];
                        matrix[i][j] = matrix[cur_row][j];
                        matrix[cur_row][j] = aux;
                    }
                }
            }
        }
    }

    //replace one of the variables for two
    {
        bool found_one = false;
        for(size_t i = 0; i < nrows; i++)
        {
            for(size_t j = 0, maxj = ncols-1; j < maxj; j++)
            {
                if(i != j && matrix[i][j] != 0)
                {
                    matrix[i][maxj] = -1*(matrix[i][j]*1);
                    matrix[i][j] = 0;
                    found_one = true;
                    break;
                }
            }
            if(found_one)
            {
                break;
            }
        }
        if(found_one == false)
        {
            return false;
        }
    }

    show_matrix(matrix,nrows,ncols);

    //check if all elements of the main diagonal are non-zero
    for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
    {
        //matrix[cur_row][cur_col] is the current element of the diagonal to make non-zero
        if(matrix[cur_row][cur_col] == 0)
        {
            return false;
        }
    }

    for(size_t cur_col = 0, cur_row = 0, max_col = ncols-2; cur_col < max_col; cur_col++, cur_row++)
    {
        for(size_t i = cur_row+1; i < nrows; i++)
        {
            ssize_t cur_elem_to_make_zero = matrix[i][cur_col];
            if(cur_elem_to_make_zero == 0)
            {
                continue;
            }
            ssize_t cur_elem_of_diagonal = matrix[cur_row][cur_col];
            ssize_t lcm = get_lcm(cur_elem_to_make_zero,cur_elem_of_diagonal);
            ssize_t cur_elem_of_diagonal_multiplier = lcm/cur_elem_of_diagonal;
            ssize_t cur_elem_to_make_zero_multiplier = lcm/cur_elem_to_make_zero;

            for(size_t j = cur_col; j < ncols; j++)
            {
                matrix[i][j] = cur_elem_to_make_zero_multiplier*matrix[i][j] -  cur_elem_of_diagonal_multiplier*matrix[cur_row][j];
            }
        }
    }
    show_matrix(matrix,nrows,ncols);
    for(size_t cur_col = 1, cur_row = 1, max_col = ncols-1; cur_col < max_col; cur_col++, cur_row++)
    {
        for(size_t i = 0; i < cur_row; i++)
        {
            ssize_t cur_elem_to_make_zero = matrix[i][cur_col];
            if(cur_elem_to_make_zero == 0)
            {
                continue;
            }
            ssize_t cur_elem_of_diagonal = matrix[cur_row][cur_col];
            ssize_t lcm = get_lcm(cur_elem_to_make_zero,cur_elem_of_diagonal);
            ssize_t cur_elem_of_diagonal_multiplier = lcm/cur_elem_of_diagonal;
            ssize_t cur_elem_to_make_zero_multiplier = lcm/cur_elem_to_make_zero;

            for(size_t j = i; j < ncols; j++)
            {
                matrix[i][j] = cur_elem_to_make_zero_multiplier*matrix[i][j] -  cur_elem_of_diagonal_multiplier*matrix[cur_row][j];
            }        
        }
    }

    //check if all elements of the main diagonal and the last column are non-zero (again)
    for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
    {
        //matrix[cur_row][cur_col] is the current element of the diagonal to make non-zero
        if(matrix[cur_row][cur_col] == 0 || matrix[cur_row][ncols-1] == 0)
        {
            return false;
        }
    }

    for(size_t i = 0, j = 0; i < nrows; i++, j=i)
    {
        if(matrix[i][ncols-1]%matrix[i][j] != 0)
        {
            ssize_t gcd = get_gcd(matrix[i][ncols-1],matrix[i][j]);
            matrix[i][ncols-1] /= gcd;
            matrix[i][j] /= gcd;

            ssize_t multiplier = matrix[i][j];
            for(size_t i = 0; i < nrows; i++)
            {
                matrix[i][ncols-1] *= multiplier;
            }
        }
        matrix[i][ncols-1] /= matrix[i][j];
        matrix[i][j] = 1;
    }

    return true;
}

reaction_err_t balance_reaction(reaction_t * restrict const react)
{
    size_t nreactants = react->reactants.nsubstances;
    size_t nproducts = react->products.nsubstances;
    size_t nvariables = nreactants+nproducts;//number of variables (whose values are the coefficient of each substance in the reaction)
    size_t nrows;//number of different elements in the reaction 
    size_t ncols = nvariables+1;//number of substances in the reaction
    components_of_substance_t* comps_arr;//compounds of each product and reactant
    components_of_substance_t comps_arr_sum_reactants = {};//total elements of comps_arr for reactants
    components_of_substance_t comps_arr_sum_products = {};//total elements of comps_arr for products
    components_of_substance_t comps_arr_sum = {};//total elements of comps_arr
    atomic_num_t* atomic_nums_arr;
    ssize_t ** matrix;//system of equations

    //fill comps_arr
    comps_arr = malloc(sizeof(components_of_substance_t)*(nreactants+nproducts));
    for(size_t i = 0; i < nreactants; i++)
    {
        get_components_of_substance(&react->reactants.substances[i],&comps_arr[i]);
    }
    for(size_t i = 0; i < nproducts; i++)
    {
        get_components_of_substance(&react->products.substances[i],&comps_arr[nreactants+i]);
    }

    //fill comps_arr_sum_reactants
    for(size_t i = 0; i < nreactants; i++)
    {
        sum_components_of_substance(&comps_arr[i],&comps_arr_sum_reactants,&comps_arr_sum_reactants);
    }
    //fill comps_arr_sum_products
    for(size_t i = 0; i < nproducts; i++)
    {
        sum_components_of_substance(&comps_arr[nreactants+i],&comps_arr_sum_products,&comps_arr_sum_products);
    }

    //do we have to balance this?
    if(compare_components_of_substance(&comps_arr_sum_reactants,&comps_arr_sum_products) == true)
    {
        free(comps_arr);
        return REACTION_ERR_BALANCE_ALREADY_BALANCED;
    }

    //fill comps_arr_sum
    sum_components_of_substance(&comps_arr_sum_reactants,&comps_arr_sum_products,&comps_arr_sum);

    //get the number of rows (equations in the system)
    nrows = comps_arr_sum.ncomponents;

    //fill atomic_nums_arr
    atomic_nums_arr = malloc(sizeof(atomic_num_t)*nrows);
    for(size_t comps_index = 1, i = 0; i < nrows; comps_index++)
    {
        if(comps_arr_sum.bucket[comps_index] != 0)
        {
            atomic_nums_arr[i] = comps_index;
            i++;
        }
    }

    //create matrix
    matrix = malloc(sizeof(ssize_t*)*(nvariables > nrows ? nvariables : nrows));
    for(size_t i = 0, maxi = (nvariables > nrows ? nvariables : nrows); i < maxi; i++)
    {
        //create the row
        matrix[i] = calloc(ncols,sizeof(ssize_t));
    }

    //fill matrix
    for(size_t j = 0, maxj = nreactants+nproducts; j < maxj; j++)
    {
        for(size_t i = 0; i < nrows; i++)
        {
            matrix[i][j] = comps_arr[j].bucket[atomic_nums_arr[i]];
            if(!(j < nreactants))
            {
                matrix[i][j] *= -1;
            }
        }
    }

    puts("Original equation:\n");

    for (size_t i = 0; i < nrows; i++)
    {
        printf("%s: ",get_elem_symbol(atomic_nums_arr[i]).symbol);
        if(get_elem_symbol(atomic_nums_arr[i]).symbol[1] == '\0')
        {
            putchar(' ');
        }
        if(matrix[i][0] < 0)
        {
            putchar('-');
        }
        for(size_t j = 0; j < ncols; j++)
        {

            if(j+1 < ncols)
            {
                printf("%lli%c ",(matrix[i][j] < 0 ? matrix[i][j]*-1 : matrix[i][j]),97+j);
                if(j+2 < ncols)
                {
                    printf((matrix[i][j+1] < 0 ? "- " : "+ "));
                }
            }
            else
            {
                printf("= %lli \n",(matrix[i][j] < 0 ? matrix[i][j]*-1 : matrix[i][j]));
            }
        }
    }
    putchar('\n');

    //fix the matrix for gauss-jordan
    {
        //fix its size
        if(nrows > nvariables)
        {
            //the last row is the total sum of the rows whose indexes are [nvariables;nrows)
            size_t last_row = nvariables-1;
            for(size_t i = nvariables; i < nrows; i++)
            {
                for(size_t j = 0; j < ncols; j++)
                {
                    matrix[last_row][j] += matrix[i][j];
                }
                free(matrix[i]);//this row will be no longer used for anything
            }
            nrows = nvariables;
        }
        else if(nrows < nvariables)
        {
            if(nrows+1 != nvariables)
            {
                return REACTION_ERR_BALANCE_CANNOT_BALANCE;
            }

            size_t last_row = nrows;
            for(size_t i = 0; i < nrows; i++)
            {
                for (size_t j = 0; j < ncols; j++)
                {
                    matrix[last_row][j] += matrix[i][j];
                }
            }
            nrows = nvariables;
        }
    }

    show_matrix(matrix,nrows,ncols);

    if(solve_matrix(matrix,nrows,ncols) == false) return REACTION_ERR_BALANCE_CANNOT_BALANCE;

    show_matrix(matrix,nrows,ncols);

    for(size_t i = 0; i < react->reactants.nsubstances; i++)
    {
        react->reactants.substances[i].amount = matrix[i][ncols-1];
    }
    for (size_t i = 0; i < react->products.nsubstances; i++)
    {
        react->products.substances[i].amount = matrix[react->reactants.nsubstances+i][ncols-1];
    }   

    for(size_t i = 0; i < nrows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);

    free(comps_arr);

    free(atomic_nums_arr);

    return REACTION_ERR_BALANCE_SUCESS;
}

