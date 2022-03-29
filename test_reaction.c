#include <stdio.h>
#include "Chemistry/types/reaction/reaction.h"

int main(int argc, char * argv[])
{
    argc--;
    argv++;

    if(argc == 0 || argc%2 != 0)
    {
        puts("Ponga el numero correcto de parametros");
        return 0;
    }
    
    reaction_t react;
    char buffer[100];

    for(size_t i = 1; i < argc; i++)
    {
        if(init_reaction(argv[i-1],argv[i],&react))
        {
            printf("%s ---> %s\n",argv[i-1],argv[i]);
            for(size_t j = 0; j < react.reactives.nsubstances; j++)
            {
                print_substance(&react.reactives.substances[j],buffer,100);
                printf("%s",buffer);
                if(j+1 != react.reactives.nsubstances)
                {
                    printf(" + ");
                }
            }
            printf(" ---> ");
            for(size_t j = 0; j < react.products.nsubstances; j++)
            {
                print_substance(&react.products.substances[j],buffer,100);
                printf("%s",buffer);
                if(j+1 != react.products.nsubstances)
                {
                    printf(" + ");
                }
            }
            putchar('\n');
        }
        else
        {
            puts("Boludo, pon bien");
            return 0;
        }
    }

    return 0;
}