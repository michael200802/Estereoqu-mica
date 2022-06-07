#include "reaction.h"
#include "../../../Math/matrix/matrix.h"

reaction_err_t balance_reaction(reaction_t * restrict const react)
{
    size_t nreactants = react->reactants.nsubstances;
    size_t nproducts = react->products.nsubstances;
    size_t nvariables = nreactants+nproducts;//number of variables in the system(whose values are the coefficient of each substance in the reaction)
    size_t nequations;//number of equations in the system or number of different elements in the reaction 
    components_of_substance_t* comps_arr;//compounds of each product and reactant
    components_of_substance_t comps_arr_sum_reactants = {};//total elements of comps_arr for reactants
    components_of_substance_t comps_arr_sum_products = {};//total elements of comps_arr for products
    components_of_substance_t comps_arr_sum = {};//total elements of comps_arr
    atomic_num_t* atomic_nums_arr;
    matrix_t matrix;//system of equations

    //do we have to balance this?
    if(is_reaction_balanced(react) == REACTION_IS_BALANCED)
    {
        return REACTION_ERR_BALANCE_ALREADY_BALANCED;
    }

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

    //fill comps_arr_sum
    sum_components_of_substance(&comps_arr_sum_reactants,&comps_arr_sum_products,&comps_arr_sum);

    //get the number of equations
    nequations = comps_arr_sum.ncomponents;

    //fill atomic_nums_arr
    atomic_nums_arr = malloc(sizeof(atomic_num_t)*nequations);
    for(size_t comps_index = 1, i = 0; i < nequations; comps_index++)
    {
        if(comps_arr_sum.bucket[comps_index] != 0)
        {
            atomic_nums_arr[i] = comps_index;
            i++;
        }
    }

    //create matrix
    if(create_matrix(&matrix,nequations+1,nvariables+1) == false)
    {
        return REACTION_ERR_BALANCE_CANNOT_BALANCE_MALLOC_FAILED;
    }

    //fill matrix
    for(size_t j = 0; j < nvariables; j++)
    {
        for(size_t i = 0; i < nequations; i++)
        {
            matrix_get_elem(matrix,i,j) = comps_arr[j].bucket[atomic_nums_arr[i]];
            if(!(j < nreactants))
            {
                matrix_get_elem(matrix,i,j) *= -1;
            }
        }
    }
    //Last equation shall be: 1a = 1
    matrix_get_elem(matrix,matrix.nrows-1,0) = 1;
    matrix_get_elem(matrix,matrix.nrows-1,matrix.ncols-1) = 1;

    puts("Original equation:\n");

    for (size_t i = 0; i < nequations; i++)
    {
        printf("%s: ",get_elem_symbol(atomic_nums_arr[i]).symbol);
        if(get_elem_symbol(atomic_nums_arr[i]).symbol[1] == '\0')
        {
            putchar(' ');
        }
        if(matrix_get_elem(matrix,i,0) < 0)
        {
            putchar('-');
        }
        for(size_t j = 0; j < matrix.ncols; j++)
        {

            if(j+1 < matrix.ncols)
            {
                printf("%lli%c ",(matrix_get_elem(matrix,i,j) < 0 ? matrix_get_elem(matrix,i,j)*-1 : matrix_get_elem(matrix,i,j)),97+j);
                if(j+2 < matrix.ncols)
                {
                    printf((matrix_get_elem(matrix,i,j+1) < 0 ? "- " : "+ "));
                }
            }
            else
            {
                printf("= %lli \n",(matrix_get_elem(matrix,i,j) < 0 ? matrix_get_elem(matrix,i,j)*-1 : matrix_get_elem(matrix,i,j)));
            }
        }
    }
    putchar('\n');

    free(comps_arr);

    free(atomic_nums_arr);

    make_matrix_RREF(&matrix);
    //all elements of the last column shall be positives
    //If they are negatives, they shall be multiplied by -1 so they become positive
    char sign = matrix_get_elem(matrix,0,matrix.ncols-1) < 0 ? -1:1;
    if(sign == -1)
    {
        for(size_t i = 0; i < matrix.nrows; i++)
        {
            matrix_get_elem(matrix,i,matrix.ncols-1) *= sign;
        }
    }

    //See if the system was resolved correctly
    //First, the matrix shall be nxm where n+1==m
    if(matrix.nrows+1 != matrix.ncols)
    {
        delete_matrix(&matrix);
        return REACTION_ERR_BALANCE_CANNOT_BALANCE;
    }
    //Second, the element of the main diagonal in the current row shall be non-zero
    //and, after this element, there shall be just zero elements, except the last column
    for(size_t i = 0; i < matrix.nrows; i++)
    {
        if(matrix_get_elem(matrix,i,i) == 0)
        {
            delete_matrix(&matrix);
            return REACTION_ERR_BALANCE_CANNOT_BALANCE;
        }
        for(size_t j = i+1; j < matrix.ncols-1; j++)
        {
            if(matrix_get_elem(matrix,i,j) != 0)
            {
                delete_matrix(&matrix);
                return REACTION_ERR_BALANCE_CANNOT_BALANCE;
            }
        }
    }
    //Finally, all coefficients shall be positive and different from zero
    for(size_t i = 0; i < matrix.nrows; i++)
    {
        if(matrix_get_elem(matrix,i,matrix.ncols-1) <= 0)
        {
            delete_matrix(&matrix);
            return REACTION_ERR_BALANCE_CANNOT_BALANCE;
        }
    }

    //We shall change the values of the last column so then the values of the variables are integers
    for(size_t i = 0; i < matrix.nrows; i++)
    {
        integer_t pivot = matrix_get_elem(matrix,i,i);
        if(pivot != 1)
        {
            integer_t lcm = get_lcm(pivot,matrix_get_elem(matrix,i,matrix.ncols-1));
            for(size_t i = 0; i < matrix.nrows; i++)
            {
                matrix_get_elem(matrix,i,matrix.ncols-1) *= lcm;
            }
            matrix_get_elem(matrix,i,matrix.ncols-1) /= pivot;
            matrix_get_elem(matrix,i,i) = 1;
        }
    }
    //Simplify the values of the last column
    integer_t gcd = matrix_get_elem(matrix,0,matrix.ncols-1);
    for(size_t i = 1; i < matrix.nrows; i++)
    {
        integer_t val = matrix_get_elem(matrix,i,matrix.ncols-1);
        if(val != 0) gcd = get_gcd(gcd,val);
    }
    if(gcd != 1)
    {
        for(size_t i = 0; i < matrix.nrows; i++)
        {
            matrix_get_elem(matrix,i,matrix.ncols-1) /= gcd;
        }
    }

    //Get all new coefficients
    integer_t val;
    for(size_t i = 0; i < react->reactants.nsubstances; i++)
    {
        val = matrix_get_elem(matrix,i,matrix.ncols-1);
        react->reactants.substances[i].amount = val;
    }
    for (size_t i = 0; i < react->products.nsubstances; i++)
    {
        val = matrix_get_elem(matrix,react->reactants.nsubstances+i,matrix.ncols-1);
        react->products.substances[i].amount = val;
    }

    delete_matrix(&matrix);

    return REACTION_ERR_BALANCE_SUCESS;
}

