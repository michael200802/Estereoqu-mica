#include "Parse.h"
#include "../Data/convert.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <stdio.h>

compound_array_t parse_compoundsum(const char * str)
{
    compound_array_t comarr;//compound array that shall be returned

    //get the number of compounds
    comarr.size = 1;
    for(size_t i = 0; str[i] != '\0'; i++)
    {
        if(str[i] == '+')
        {
            comarr.size++;
        }
    }

    //allocate memory
    comarr.array = (compound_t*)calloc(comarr.size,sizeof(compound_t));

    for(size_t i = 0, len = 0; i < comarr.size; i++)
    {
        //go to the next compound
        str += len;
        while(isspace(*str))
        {
            str++;
        }
        if(*str == '+')
        {
            str++;
            while(isspace(*str))
            {
                str++;
            }
        }

        //get the len
        for(len = 0; str[len] != ' ' && str[len] != '+' && str[len] != '\0'; len++);

        //copy
        comarr.array[i].name = (char*)calloc(len+1,sizeof(char));
        memcpy(comarr.array[i].name,str,len);
    }

    for (size_t i = 0; i < comarr.size; i++)
    {
        comarr.array[i] = convert_strto_compound(comarr.array[i].name);
    }

    return comarr;
}