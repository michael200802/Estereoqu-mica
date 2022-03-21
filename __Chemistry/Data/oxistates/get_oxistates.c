#include "get_oxistates.h"
#include <string.h>

#include "table.h"

oxistates_t get_oxistate(const char * str)
{
    for (size_t i = 0; i < NUMBER_OF_ELEMENTS; i++)
    {
        if(strcmp(str,oxistates_table[i].symbol) == 0)
        {
            return oxistates_table[i].states;
        }
    }
    oxistates_t err = {.n = -1};
    return err;
}