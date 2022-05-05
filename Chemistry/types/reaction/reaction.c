#include "reaction.h"

reaction_err_t init_reaction(const char * restrict _reactants, const char * restrict _products, reaction_t * restrict const react, bool balance)
{
    size_t nreactants = 0;
    size_t nproducts = 0;
    {
        size_t i;

        i = 0;
        while(isspace(*_reactants))
        {
            _reactants++;
        }
        while(_reactants[i] != '\0')
        {
            {
                size_t substance_index = i;

                while(isspace(_reactants[i]) == 0 && _reactants[i] != '+' && _reactants[i] != '\0')
                {
                    i++;
                }

                if(is_str_substance(&_reactants[substance_index],i-substance_index) == false)
                {
                    return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
                }

                nreactants++;
            }

            switch(_reactants[i])
            {
                case ' ':
                    while(isspace(_reactants[i]))
                    {
                        i++;
                    }
                    if(_reactants[i] != '+')
                    {
                        if(_reactants[i] == '\0')
                        {
                            continue;
                        }
                        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
                    }

                case '+':
                    i++;
                    while(isspace(_reactants[i]))
                    {
                        i++;
                    }
                    if(_reactants[i] == '+' || _reactants[i] == '\0')
                    {
                        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
                    }

            }
        }

        i = 0;
        while(isspace(*_products))
        {
            _products++;
        }
        while(_products[i] != '\0')
        {
            {
                size_t substance_index = i;

                while(_products[i] != '+' && isspace(_products[i]) == 0 && _products[i] != '\0')
                {
                    i++;
                }

                if(is_str_substance(&_products[substance_index],i-substance_index) == false)
                {
                    return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
                }
                nproducts++;
            }
            switch (_products[i])
            {
                case ' ':
                    while(isspace(_products[i]))
                    {
                        i++;
                    }
                    if(_products[i] != '+')
                    {
                        if(_products[i] == '\0')
                        {
                            continue;
                        }
                        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
                    }

                case '+':
                    do
                    {
                        i++;
                    }while(isspace(_products[i]));
                    if(_products[i] == '+' || _products[i] == '\0')
                    {
                        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
                    }
            }
        }
    }
    if(nreactants == 0 || nproducts == 0)
    {
        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
    }
    char * reactants;
    char * products;
    reactants = strdup(_reactants);
    if(reactants == NULL)
    {
        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
    }
    products = strdup(_products);
    if(products == NULL)
    {
        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
    }

    char * strtok_buffer;
    char ** tokens;
    tokens = malloc(sizeof(char*)*(nreactants > nproducts ? nreactants : nproducts));
    if(tokens == NULL)
    {
        //free input strings
        free(products);
        free(reactants);
        return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
    }

    strtok_buffer = reactants;
    for(size_t i = 0; i < nreactants; i++)
    {
        tokens[i] = strtok_r(NULL," +",&strtok_buffer);

        if(tokens[i] == NULL)
        {
            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactants);
            return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
        }
    }
    react->reactants.nsubstances = nreactants;
    react->reactants.substances = malloc(sizeof(substance_t)*nreactants);
    for(size_t i = 0; i < nreactants; i++)
    {
        if(init_substance(tokens[i],&react->reactants.substances[i]) == REACTION_ERR_INIT_UNKNOWN_SYMBOL)//just in case... I know it isn't useful now but in the future it may be so
        {
            for(size_t j = 0; j < i; j++)//free reactants already created
            {
                destroy_substance(&react->reactants.substances[j]);
            }
            free(react->reactants.substances);//free memory for reactants' array

            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactants);
            return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
        } 
    }

    strtok_buffer = products;
    for(size_t i = 0; i < nproducts; i++)
    {
        tokens[i] = strtok_r(NULL,"+ ",&strtok_buffer);

        if(tokens[i] == NULL || is_str_substance(tokens[i],IS_STR_SUBSTANCE_UNKNOWN_LEN) == false)
        {
            for(size_t i = 0; i < nreactants; i++)//destroy all reactants
            {
                destroy_substance(&react->reactants.substances[i]);
            }
            free(react->reactants.substances);//free memory for reactants' array

            free(tokens);//free tokens

            //free input strings
            free(products);
            free(reactants);

            return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
        }
    }
    react->products.nsubstances = nproducts;
    react->products.substances = malloc(sizeof(substance_t)*nproducts);
    for(size_t i = 0; i < nproducts; i++)
    {
        if(init_substance(tokens[i],&react->products.substances[i]) == REACTION_ERR_INIT_UNKNOWN_SYMBOL)
        {
            free(tokens);//free tokens

            for(size_t j = 0; j < i; j++)//destroy all products already created
            {
                destroy_substance(&react->products.substances[j]);
            }
            free(react->products.substances);//goodbye products' array

            for(size_t i = 0; i < nreactants; i++)//destroy all reactants
            {
                destroy_substance(&react->reactants.substances[i]);
            }
            free(react->reactants.substances);//free memory for reactants' array

            //free input strings
            free(products);
            free(reactants);

            return REACTION_ERR_INIT_UNKNOWN_SYMBOL;
        }
    }

    {//see whether they have the same components or we have to balance the reaction
        if(balance)
        {
            switch(balance_reaction(react))
            {
                case REACTION_ERR_BALANCE_SUCESS:
                    break;
                case REACTION_ERR_BALANCE_ALREADY_BALANCED:
                    break;
                case REACTION_ERR_BALANCE_CANNOT_BALANCE:
                    {
                        //destroy the just created reaction
                        destroy_reaction(react);

                        free(tokens);//free tokens

                        //free input strings
                        free(products);
                        free(reactants);
                        return REACTION_ERR_INIT_CANNOT_BALANCE;
                    }
            }
        }
        else 
        {
            is_reaction_balanced_returnv_t is_balanced = is_reaction_balanced(react);
            switch(is_balanced)
            {
                case REACTION_IS_NOT_BALANCED:
                    {
                        //destroy the just created reaction
                        destroy_reaction(react);

                        free(tokens);//free tokens

                        //free input strings
                        free(products);
                        free(reactants);
                        return REACTION_ERR_INIT_UNBALANCED;
                    }
                case REACTION_CANNOT_BE_BALANCED:
                    {
                        //destroy the just created reaction
                        destroy_reaction(react);

                        free(tokens);//free tokens

                        //free input strings
                        free(products);
                        free(reactants);
                        return REACTION_ERR_INIT_CANNOT_BALANCE;
                    }
            }
        }
    }

    free(tokens);//free tokens

    //free input strings
    free(products);
    free(reactants);

    return REACTION_ERR_INIT_SUCCESS;
}

inline is_reaction_balanced_returnv_t is_reaction_balanced(const reaction_t * restrict const react)
{
    components_of_substance_t reactants_comp = {};
    components_of_substance_t products_comp = {};
    components_of_substance_t aux;

    for(size_t i = 0; i < react->reactants.nsubstances; i++)
    {
        get_components_of_substance(&react->reactants.substances[i],&aux);
        sum_components_of_substance(&reactants_comp,&aux,&reactants_comp);
    }
    for(size_t i = 0; i < react->products.nsubstances; i++)
    {
        get_components_of_substance(&react->products.substances[i],&aux);
        sum_components_of_substance(&products_comp,&aux,&products_comp);
    }

    if(compare_components_of_substance(&reactants_comp,&products_comp) == false)
    {
        return (reactants_comp.ncomponents != products_comp.ncomponents ? REACTION_CANNOT_BE_BALANCED : REACTION_IS_NOT_BALANCED);
    }
    return REACTION_IS_BALANCED;
}

inline reaction_err_t init_balanced_reaction(const char * restrict _reactants, const char * restrict _products, reaction_t * restrict const react)
{
    return init_reaction(_reactants,_products,react,false);
}

inline void destroy_reaction(reaction_t * restrict const react)
{
    for(size_t i = 0; i < react->reactants.nsubstances; i++)
    {
        destroy_substance(&react->reactants.substances[i]);
    }
    free(react->reactants.substances);

    for(size_t i = 0; i < react->products.nsubstances; i++)
    {
        destroy_substance(&react->products.substances[i]);
    }
    free(react->products.substances);
}