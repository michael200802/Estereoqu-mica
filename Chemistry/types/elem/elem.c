#include "elem.h"

inline bool init_elem(const elem_symbol_t symbol, elem_t * const restrict elem)
{
    *elem = (elem_t){.atomic_num = get_atomicnum(symbol)};
    return (elem->atomic_num == -1 ? false : true);
}