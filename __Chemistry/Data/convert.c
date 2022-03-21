#include "convert.h"
#include "uma/get_uma.h"
#include "oxistates/get_oxistates.h"
#include <stdlib.h>
#include <ctype.h>

num_t Pow(num_t x, size_t y)
{
    num_t result = 1;
    while(y != 0)
    {
        result *= x;
        y--;
    }
    return result;
}

element_t convert_strto_elem(const char * str)
{
    element_t elem;
    //get the symbol
    elem.symbol[0] = *str;
    str++;
    if(isalpha(*str))
    {
        elem.symbol[1] = *str;
        elem.symbol[2] = '\0';
    }
    else
    {
        elem.symbol[1] = '\0';
    }

    //get the mol
    size_t len;
    for(len = 0; isdigit(str[len]); len++);
    if(len != 0)
    {
        elem.mol = 0;
        do
        {
            len--;
            elem.mol += Pow(10,len)*(*str-48);
            str++;
        } while (len != 0);
    }
    else
    {
        elem.mol = 1;
    }

    //get mass
    elem.mass = get_uma(elem.symbol);
    elem.mass *= elem.mol;

    //get oxistate
    elem.oxistates = get_oxistate(elem.symbol);

    return elem;
}

compound_t convert_strto_compound(char * str)
{
    compound_t comp = {.name = str};
    size_t len;

    //get the coefficient ---> get mol
    for(len = 0; isdigit(str[len]); len++);
    if(len != 0)
    {
        comp.mol = 0;
        do
        {
            len--;
            comp.mol += Pow(10,len)*(*str-48);
            str++;
        } while (len != 0);
    }
    else
    {
        comp.mol = 1;
    }

    //get the number of elements
    comp.elemsarr.size = 0;
    for(size_t i = 0; str[i] != '\0'; i++)
    {
        if(isalpha(str[i]) && isupper(str[i]))
        {
            comp.elemsarr.size++;
        }
    }

    //allocate memory
    comp.elemsarr.array = (element_t*)calloc(comp.elemsarr.size,sizeof(element_t));

    //fill the elements array
    for(size_t i = 0; i < comp.elemsarr.size; i++)
    {
        //go to the next element
        while(isalpha(*str) == 0 || isupper(*str) == 0)
        {
            str++;
        }

        //get element
        comp.elemsarr.array[i] = convert_strto_elem(str);

        //update mol
        for (size_t j = 0; str[j] != '\0' && str[j] != '(';)
        {
            if(str[j] == ')')
            {
                j++;
                len = 0;
                for(len = 0; isdigit(str[j+len]); len++);
                if(len != 0)
                {
                    size_t mol = 0;
                    do
                    {
                        len--;
                        mol += Pow(10,len)*(str[j]-48);
                        j++;
                    } while (len != 0);
                    comp.elemsarr.array[i].mol *= mol;
                    comp.elemsarr.array[i].mass *= mol;
                }
            }
            else
            {
                j++;
            }
        }
        
        str++;
    }

    //get the mass
    comp.pm = 0;
    for (size_t i = 0; i < comp.elemsarr.size; i++)
    {
        comp.pm += comp.elemsarr.array[i].mass;
    }
    comp.pm *= comp.mol;
    
    return comp;
}