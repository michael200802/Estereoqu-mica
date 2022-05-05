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
        reaction_err_t returnerr = init_reaction(argv[i-1],argv[i],&react,true);
        switch(returnerr)
        {
            case REACTION_ERR_INIT_UNBALANCED:
                puts("Sin balancear.");
                break; 
            case REACTION_ERR_INIT_CANNOT_BALANCE:
                puts("No es posible balancear.");
                break;    
            case REACTION_ERR_INIT_SUCCESS:
                printf("Given:   %s ---> %s\n",argv[i-1],argv[i]);

                printf("Program: ");
                for(size_t j = 0; j < react.reactants.nsubstances; j++)
                {
                    print_substance(&react.reactants.substances[j],buffer,100);
                    printf("%s",buffer);
                    if(j+1 != react.reactants.nsubstances)
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
                break;
            case REACTION_ERR_INIT_UNKNOWN_SYMBOL:
                puts("Boludo, pon bien");
                return 0;
        }
    }

    return 0;
}