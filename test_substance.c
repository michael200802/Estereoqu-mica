#include <stdio.h>
#include "Chemistry/types/substance/substance.h"

extern size_t _print_substance(const substance_t * const restrict sub, char * buffer, size_t buffer_len); 

void foo(substance_t sub)
{
    char buffer[100];
    _print_substance(&sub,buffer,100);
    puts(buffer);
    if(!sub.is_simple_substance)
    {
        for (size_t i = 0; i < sub.substance.compound.nsubstances; i++)
        {
            foo(sub.substance.compound.substances[i]);
        }
    }
}

int main(int argc, char * argv[])
{
    argc--;
    argv++;

    if(*argv == NULL)
    {
        puts("Pon al menos un compuesto/sustancia, imbecil.");
    }

    for(size_t i = 0; i < argc; i++)
    {
        substance_t sub;
        if(init_substance(argv[i],&sub))
        {
            char buffer[100] = {};
            printf("%s: strlen: %ld, str: %s, molar mass: %lf, equivalent number: %lf\n",
                argv[i],
                print_substance(&sub,buffer,100),
                buffer,
                sub.molar_mass*sub.amount,
                get_equivalent_number_of_substance(&sub)
            );
            foo(sub);
            destroy_substance(&sub);
        }
        else
        {
            printf("%s no es un compuesto/sustancia\n",argv[i]);
        }
    }

    return 0;
}