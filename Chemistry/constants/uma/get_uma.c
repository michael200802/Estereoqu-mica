#include "get_uma.h"
#include <string.h>

#include "table.h"

inline num_t get_uma(atomic_num_t atomic_num)
{
    if(atomic_num < 1 || atomic_num > NUMBER_OF_ELEMENTS)
    {
        return -1;
    }
    return uma_table[atomic_num-1].mass;
}