#include "get_uma.h"
#include <string.h>

#include "table.h"

num_t get_uma(const char * str)
{
    for (size_t i = 0; i < NUMBER_OF_ELEMENTS; i++)
    {
        if(strcmp(uma_table[i].symbol,str) == 0)
        {
            return uma_table[i].mass;
        }
    }
    return -1;
}