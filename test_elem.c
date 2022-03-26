#include "Chemistry/types/elem/elem.h"

#include <stdio.h>

int main(int argc, char * argv[])
{
    argc--;
    argv++;

    if(argc == 0)
    {
        puts("Pon un elemento al menos");
        return -1;
    }

    for(size_t i = 0; i < argc; i++)
    {
        elem_symbol_t sym;
        elem_t elem;

        if(init_elem_symbol(argv[i],&sym) == false)
        {
            printf("%s no es un simbolo valido.\n",argv[i]);
        }
        else
        {
            if(init_elem(sym,&elem) == false)
            {
                printf("%s no es un elemento.\n",argv[i]);
            }
            else
            {
                printf("%zu: atomic num: %hhu, symbol: %s, mass: %f, {",i,elem.atomic_num,elem.symbol.symbol,elem.molar_mass);
                for(char i = 0; i < elem.possible_oxistates.n; i++)
                {
                    printf(" %hhd ",elem.possible_oxistates.states[i]);
                }
                puts("}\n");
            }
        }
        
    }

    return 0;
}