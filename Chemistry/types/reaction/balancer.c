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

//make all elements in the main diagonal non-zero by swaping rows
bool make_nonzero(ssize_t** matrix, const size_t nrows, const size_t ncols)
{
    {//See if we really have to make_nonzero()
        bool all_nonzero = true;
        for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
        {
            if(matrix[cur_row][cur_col] != 0)
            {
                all_nonzero = false;
                break;
            }
        }
        if(all_nonzero == true)
        {
            return true;
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
                    ssize_t* aux = matrix[i];
                    matrix[i] = matrix[cur_row];
                    matrix[cur_row] = aux;
                }
            }
        }
    }

    //check if all elements of the main diagonal are non-zero
    for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
    {
        //matrix[cur_row][cur_col] is the current element of the diagonal to make non-zero
        if(matrix[cur_row][cur_col] == 0)
        {
            return false;
        }
    }

    return true;
}

//make zero all elements which aren't from the main diagonal
bool make_zero(ssize_t** matrix, const size_t nrows, const size_t ncols)
{
    //if(make_nonzero(matrix,nrows,ncols)==false) return false;

    //make zero all elements which aren't from the main diagonal
    for(size_t cur_col = 0, cur_row = 0, max_col = ncols-2; cur_col < max_col; cur_col++, cur_row++)
    {
        for(size_t i = cur_row+1; i < nrows; i++)
        {
            ssize_t cur_elem_to_make_zero = matrix[i][cur_col];
            ssize_t cur_elem_of_diagonal = matrix[cur_row][cur_col];
            if(cur_elem_to_make_zero == 0)
            {
                continue;
            }
            if(cur_elem_of_diagonal == 0)
            {
                return false;
            }
            ssize_t lcm = get_lcm(cur_elem_to_make_zero,cur_elem_of_diagonal);
            ssize_t cur_elem_of_diagonal_multiplier = lcm/cur_elem_of_diagonal;
            ssize_t cur_elem_to_make_zero_multiplier = lcm/cur_elem_to_make_zero;

            for(size_t j = cur_col; j < ncols; j++)
            {
                matrix[i][j] = cur_elem_to_make_zero_multiplier*matrix[i][j] -  cur_elem_of_diagonal_multiplier*matrix[cur_row][j];
            }

            if(make_nonzero(matrix,nrows,ncols)==false) return false;
        }
    }
    show_matrix(matrix,nrows,ncols);
    for(size_t cur_col = 1, cur_row = 1, max_col = ncols-1; cur_col < max_col; cur_col++, cur_row++)
    {
        for(size_t i = 0; i < cur_row; i++)
        {
            ssize_t cur_elem_to_make_zero = matrix[i][cur_col];
            ssize_t cur_elem_of_diagonal = matrix[cur_row][cur_col];
            if(cur_elem_of_diagonal == 0)
            {
                return false;
            }
            if(cur_elem_to_make_zero == 0)
            {
                continue;
            }
            ssize_t lcm = get_lcm(cur_elem_to_make_zero,cur_elem_of_diagonal);
            ssize_t cur_elem_of_diagonal_multiplier = lcm/cur_elem_of_diagonal;
            ssize_t cur_elem_to_make_zero_multiplier = lcm/cur_elem_to_make_zero;

            for(size_t j = i; j < ncols; j++)
            {
                matrix[i][j] = cur_elem_to_make_zero_multiplier*matrix[i][j] -  cur_elem_of_diagonal_multiplier*matrix[cur_row][j];
            }

            if(make_nonzero(matrix,nrows,ncols)==false) return false;        
        }
    }
    show_matrix(matrix,nrows,ncols);

    return true;
}

//returns the solutions
bool solve_matrix(ssize_t** matrix, size_t nrows, size_t ncols)
{
    show_matrix(matrix,nrows,ncols);

    if(make_nonzero(matrix,nrows,ncols)==false) return false;

    show_matrix(matrix,nrows,ncols);

    if(make_zero(matrix,nrows,ncols) == false) return false;

    //check if all elements of the main diagonal and the last column are non-zero (again)
    for(size_t cur_row = 0, cur_col = 0; cur_row < nrows; cur_row++, cur_col = cur_row)
    {
        //matrix[cur_row][cur_col] is the current element of the diagonal to make non-zero
        if(matrix[cur_row][cur_col] == 0 || matrix[cur_row][ncols-1] == 0)
        {
            return false;
        }
    }

    //get each coefficient
    //example of how each row is treated:
    //ax + 0y + 0z = b ----> 1x + 0y + 0z = b/a
    //Then, if b/a isn't an integer:
    /*a/b is simplified by using the gdc returned by  get_gcd(a,b)*/
    //all elements of the last column are multiplied by a
    for(size_t i = 0, j = 0; i < nrows; i++, j=i)
    {
        if(matrix[i][ncols-1]%matrix[i][j] != 0)
        {
            /* a/b is simplified */
            ssize_t gcd = get_gcd(matrix[i][ncols-1],matrix[i][j]);
            matrix[i][ncols-1] /= gcd;
            matrix[i][j] /= gcd;

            //everyone is multiplied by a
            ssize_t multiplier = matrix[i][j];
            for(size_t i = 0; i < nrows; i++)
            {
                matrix[i][ncols-1] *= multiplier;
            }
        }
        matrix[i][ncols-1] /= matrix[i][j];
        matrix[i][j] = 1;
    }

    //simplify all values
    const size_t last_col = ncols-1;
    ssize_t gdc = get_gcd(matrix[0][last_col],matrix[1][last_col]);
    for(size_t i = 2; i < nrows; i++)
    {
        ssize_t gdc_aux = get_gcd(matrix[i][last_col],matrix[i-1][last_col]);
        gdc = get_gcd(gdc,gdc_aux);
    }
    for(size_t i = 0; i < nrows; i++)
    {
        matrix[i][last_col] /= gdc;
    }

    show_matrix(matrix,nrows,ncols);

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
    size_t max_nrows;//nvariables

    //fill comps_arr
    comps_arr = malloc(sizeof(components_of_substance_t)*(nreactants+nproducts));
    for(size_t i = 0; i < nreactants; i++)
    {
        size_t amount = react->reactants.substances[i].amount;
        react->reactants.substances[i].amount = 1;
        get_components_of_substance(&react->reactants.substances[i],&comps_arr[i]);
        react->reactants.substances[i].amount = amount;
    }
    for(size_t i = 0; i < nproducts; i++)
    {
        size_t amount = react->products.substances[i].amount;
        react->products.substances[i].amount = 1;
        get_components_of_substance(&react->products.substances[i],&comps_arr[nreactants+i]);
        react->products.substances[i].amount = amount;
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
    max_nrows = nvariables > nrows ? nvariables : nrows;
    matrix = malloc(sizeof(ssize_t*)*max_nrows);
    for(size_t i = 0, maxi = max_nrows; i < maxi; i++)
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

    show_matrix(matrix,nrows,ncols);

    //fix the matrix for gauss-jordan (if it is not possible, it fails)
    {
        //fix its size
        if(nrows > nvariables)
        {
            //Simplify the equations
            for(size_t i = 0; i < nrows; i++)
            {
                ssize_t gdc = 0;
                size_t j = 0;
                for(; j < ncols && gdc == 0; j++)
                {
                    if(matrix[i][j] != 0)
                    {
                        gdc = matrix[i][j];
                    }
                }
                for(; j < ncols; j++)
                {
                    if(matrix[i][j] == 0)
                    {
                        continue;
                    }
                    gdc = get_gcd(gdc,matrix[i][j]);
                }
                for(size_t j = 0; j < ncols; j++)
                {
                    matrix[i][j] /= gdc;
                }
            }
            
            show_matrix(matrix,nrows,ncols);

            //Look if we have repeated equations
            for(size_t i = 0; i < nrows; i++)
            {
                for(size_t j = 0; j < nrows;)
                {
                    if(i == j)
                    {
                        j++;
                        continue;
                    }

                    ssize_t* row1 = matrix[i], *row2 = matrix[j];
                    bool equal = true;
                    for(size_t i = 0; i < ncols && equal; i++)
                    {
                        equal = (row1[i] == row2[i]);
                    }

                    if(equal)
                    {
                        nrows--;
                        matrix[i] = matrix[nrows];
                        matrix[nrows] = row1;
                        j = 0;
                    }
                    else
                    {
                        j++;
                    }
                }
            }

            //Fail, it's impossible
            if(nrows > nvariables)
            {
                for(size_t i = 0; i < max_nrows; i++)
                {
                    free(matrix[i]);
                }
                free(matrix);

                free(comps_arr);

                free(atomic_nums_arr);

                return REACTION_ERR_BALANCE_CANNOT_BALANCE;
            }
        }
        else if(nrows+1 == nvariables)
        {
            size_t last_row = nrows;
            for(size_t i = 0; i < nrows; i++)
            {
                for (size_t j = 0; j < ncols; j++)
                {
                    matrix[last_row][j] += matrix[i][j];
                }
            }
            nrows++;
        }
        else
        {
            for(size_t i = 0; i < max_nrows; i++)
            {
                free(matrix[i]);
            }
            free(matrix);

            free(comps_arr);

            free(atomic_nums_arr);

            return REACTION_ERR_BALANCE_CANNOT_BALANCE;
        }
    
        show_matrix(matrix,nrows,ncols);

        //replace one of the variables for one
        ssize_t* last_row = matrix[nrows-1];
        size_t lr_index = 0;
        if(make_nonzero(matrix,nrows,ncols) == false)
        {
            for(size_t i = 0; i < max_nrows; i++)
            {
                free(matrix[i]);
            }
            free(matrix);

            free(comps_arr);

            free(atomic_nums_arr);
            
            return REACTION_ERR_BALANCE_CANNOT_BALANCE;
        }
        while(matrix[lr_index] != last_row)
        {
            lr_index++;
        }
        for(size_t i = 0; i < ncols-1; i++)
        {
            if(last_row[i] != 0 && i != lr_index)
            {
                //"well, since it is 1, its coefficient shall go to the right part of the equation"
                last_row[ncols-1] = last_row[i]*-1;
                last_row[i] = 0;
            }
        }
    }

    show_matrix(matrix,nrows,ncols);

    if(solve_matrix(matrix,nrows,ncols) == false)
    {
        for(size_t i = 0; i < max_nrows; i++)
        {
            free(matrix[i]);
        }
        free(matrix);

        free(comps_arr);

        free(atomic_nums_arr);
        
        return REACTION_ERR_BALANCE_CANNOT_BALANCE;
    }

    for(size_t i = 0; i < react->reactants.nsubstances; i++)
    {
        ssize_t val = matrix[i][ncols-1];
        val = val < 0 ? val*-1 : val;
        react->reactants.substances[i].amount = val;
    }
    for (size_t i = 0; i < react->products.nsubstances; i++)
    {
        ssize_t val = matrix[react->reactants.nsubstances+i][ncols-1];
        val = val < 0 ? val*-1 : val;
        react->products.substances[i].amount = val;
    }   

    for(size_t i = 0; i < max_nrows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);

    free(comps_arr);

    free(atomic_nums_arr);

    return REACTION_ERR_BALANCE_SUCESS;
}

