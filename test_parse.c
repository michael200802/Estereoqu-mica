#include <stdio.h>
#include "Chemistry/ParseStr/is_correct.h"
#include "Chemistry/ParseStr/Parse.h"

int main(int argc, char ** argv)
{
    argv++;
    if(*argv == NULL)
    {
        return 0;
    }

    if(is_compoundsum_correct(*argv))
    {
        puts("Correct");
    }
    else
    {
        puts("Wrong");
        return 0;
    }

    compound_array_t comarr = parse_compoundsum(*argv);

    for (size_t i = 0; i < comarr.size; i++)
    {
        printf("%zu/%zu: %s\n",i,comarr.size,comarr.array[i].name);
        printf("Mol: %lf Mass: %lf\n",comarr.array[i].mol,comarr.array[i].pm);
        puts("Elements: ");
        for (size_t j = 0; j < comarr.array[i].elemsarr.size; j++)
        {
            printf("%zu/%zu: %s\n",j,comarr.array[i].elemsarr.size,comarr.array[i].elemsarr.array[j].symbol);
            printf("Mol: %lf Mass: %lf\n");
            puts("Oxistates: ");
            for (size_t k = 0; k < comarr.array[i].elemsarr.array[j].oxistates.n; k++)
            {
                printf("%d, ",comarr.array[i].elemsarr.array[j].oxistates.states[k]);
            }
            putchar('\n');
        }
        
    }
    

    return 0;
}