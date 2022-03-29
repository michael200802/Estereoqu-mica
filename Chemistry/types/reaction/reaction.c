#include "reaction.h"

bool init_reaction(const char * restrict _reactives, const char * restrict _products, reaction_t * restrict const react)
{
    char * reactives;
    char * products;
    char * strtok_buffer;
    size_t nreactives = 1;
    size_t nproducts = 1;
    char ** tokens;

    for(size_t i = 0; _reactives[i] != '\0'; i++)
    {
        if(_reactives[i] == '+')
        {
            if(isspace(_reactives[i+1]))
            {
                do
                {
                    i++;
                } while (isspace(_reactives[i]));
                if(_reactives[i] == '+')
                {
                    return false;
                }
                i--;  
            }
            nreactives++;
        }
    }
    for(size_t i = 0; _products[i] != '\0'; i++)
    {
        if(_products[i] == '+')
        {
            if(isspace(_products[i+1]))
            {
                do
                {
                    i++;
                } while (isspace(_products[i]));
                if(_products[i] == '+')
                {
                    return false;
                }
                i--;
            }
            nproducts++;
        }
    }

    reactives = strdup(_reactives);
    if(reactives == NULL)
    {
        return false;
    }
    products = strdup(_products);
    if(products == NULL)
    {
        return false;
    }

    tokens = malloc(sizeof(char*)*(nreactives > nproducts ? nreactives : nproducts));
    if(tokens == NULL)
    {
        //free input strings
        free(products);
        free(reactives);
        return false;
    }

    strtok_buffer = reactives;
    for(size_t i = 0; i < nreactives; i++)
    {
        tokens[i] = strtok_r(NULL," +",&strtok_buffer);
        if(tokens[i] == NULL || is_str_substance(tokens[i]) == false)
        {
            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactives);
            return false;
        }
    }
    react->reactives.nsubstances = nreactives;
    react->reactives.substances = malloc(sizeof(substance_t)*nreactives);
    for(size_t i = 0; i < nreactives; i++)
    {
        if(init_substance(tokens[i],&react->reactives.substances[i]) == false)//just in case... I know it isn't useful now but in the future it may be so
        {
            for(size_t j = 0; j < i; j++)//free reactives already created
            {
                destroy_substance(&react->reactives.substances[j]);
            }
            free(react->reactives.substances);//free memory for reactives' array

            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactives);
            return false;
        } 
    }

    strtok_buffer = products;
    for(size_t i = 0; i < nproducts; i++)
    {
        tokens[i] = strtok_r(NULL," +",&strtok_buffer);
        if(tokens[i] == NULL || is_str_substance(tokens[i]) == false)
        {
            for(size_t i = 0; i < nreactives; i++)//destroy all reactives
            {
                destroy_substance(&react->reactives.substances[i]);
            }
            free(react->reactives.substances);//free memory for reactives' array

            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactives);

            return false;
        }
    }
    react->products.nsubstances = nproducts;
    react->products.substances = malloc(sizeof(substance_t)*nproducts);
    for(size_t i = 0; i < nproducts; i++)
    {
        if(init_substance(tokens[i],&react->products.substances[i]) == false)
        {
            free(tokens);//free tokens

            for(size_t j = 0; j < i; j++)//destroy all products already created
            {
                destroy_substance(&react->products.substances[j]);
            }
            free(react->products.substances);//goodbye products' array

            for(size_t i = 0; i < nreactives; i++)//destroy all reactives
            {
                destroy_substance(&react->reactives.substances[i]);
            }
            free(react->reactives.substances);//free memory for reactives' array

            //free input strings
            free(products);
            free(reactives);

            return false;
        }
    }

    {//see whether they have the same components
        components_of_substance_t reactives_comp = {};
        components_of_substance_t products_comp = {};
        components_of_substance_t aux;

        for(size_t i = 0; i < react->reactives.nsubstances; i++)
        {
            get_components_of_substance(&react->reactives.substances[i],&aux);
            sum_components_of_substance(&aux,&reactives_comp,&reactives_comp);
        }

        for(size_t i = 0; i < react->products.nsubstances; i++)
        {
            get_components_of_substance(&react->products.substances[i],&aux);
            sum_components_of_substance(&aux,&products_comp,&products_comp);
        }

        if(compare_components_of_substance(&reactives_comp,&products_comp) == false)
        {
            //destroy the just created reaction
            destroy_reaction(react);

            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactives);
            return false;
        }
    }
    

    free(tokens);//free tokens

    //free input strings
    free(products);
    free(reactives);

    return true;
}

inline void destroy_reaction(reaction_t * restrict const react)
{
    for(size_t i = 0; i < react->reactives.nsubstances; i++)
    {
        destroy_substance(&react->reactives.substances[i]);
    }
    free(react->reactives.substances);

    for(size_t i = 0; i < react->products.nsubstances; i++)
    {
        destroy_substance(&react->products.substances[i]);
    }
    free(react->products.substances);
}