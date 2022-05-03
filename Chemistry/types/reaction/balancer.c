#include "reaction.h"

reaction_err_t balance_reaction(reaction_t * restrict const react)
{
    size_t nreactants = react->reactants.nsubstances;
    size_t nproducts = react->products.nsubstances;
    size_t nrows;//number of different elements in the reaction 
    size_t ncols = nreactants+nproducts;//number of substances in the reaction
    components_of_substance_t* comps_arr;//compounds of each product and reactant
    components_of_substance_t comps_arr_sum_reactants = {};//total elements of comps_arr for reactants
    components_of_substance_t comps_arr_sum_products = {};//total elements of comps_arr for products
    components_of_substance_t comps_arr_sum = {};//total elements of comps_arr
    atomic_num_t* atomic_nums_arr;
    ssize_t ** matrix;//system of equations

    //fill comps_arr
    comps_arr = malloc(sizeof(components_of_substance_t)*ncols);
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
    matrix = malloc(sizeof(ssize_t*)*nrows);
    for(size_t i = 0; i < nrows; i++)
    {
        //create the row
        matrix[i] = calloc(ncols,sizeof(ssize_t));
    }

    //fill matrix
    for(size_t j = 0; j < ncols; j++)
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
            printf("%lli%c ",(matrix[i][j] < 0 ? matrix[i][j]*-1 : matrix[i][j]),97+j);

            if(j+1 != ncols)
            {
                printf((matrix[i][j+1] < 0 ? "- " : "+ "));
            }
        }
        puts("= 0");
    }

    //and Idk what to do anymore jsjsjjs (Mati, me cago en todo, ojala me muera)
    
    for(size_t i = 0; i < nrows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);

    free(comps_arr);

    free(atomic_nums_arr);

    return REACTION_ERR_BALANCE_SUCESS;
}

