#include "get_oxistates.h"

#include <string.h>

#include "table.h"

inline oxistates_t get_oxistates(atomic_num_t atomic_num)
{
    if(atomic_num < 1 || atomic_num > NUMBER_OF_ELEMENTS)
    {
        oxistates_t err = {.n = -1};
        return err;
    }
    return oxistates_table[atomic_num-1].states;
}
