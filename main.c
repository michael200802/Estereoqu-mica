#include "GUI/GUI.h"
#include <stdlib.h>

int main(void)
{
    int exitv = start_app();
    if(exitv == EXIT_FAILURE)
    {
        msg_app("Error al empezar la aplicacion.","Se le comunicara el error al Michu.");
        //codigo para mandar el error xd
    }

    for(;;)
    {
        input_t input = get_cur_input();

        switch (input.error_code)
        {
            case INPUT_NOERRROR:
                break;
            default:
                msg_app("Error en el input.","Se le comunicara el error al Michu.");
            case INPUT_ERROR_WND_CLOSED:
                return end_app();
        }

        {
            const reaction_t * const react = input.react;

            const var_arr_t * const var_arrs[2] = {input.var_arr_reactants, input.var_arr_products};

            //a ---> x
            //b ---> y
            //Then, how do I get the value of y?
            //y = x * (b/a)
            //then seed is b/a
            //y = x * seed
            //this can work as a function:
            //f(x) = x*seed = x*m = m*x
            //f(x) = m*x
            num_t seed;

            char buffer[1000];

            const substance_arr_t * const theoretical_reactants = &react->reactants;
            const var_arr_t * const real_reactants = input.var_arr_reactants;
            size_t limiting_reagent_index;
            size_t excess_reagent_index;

            //print theoretical and real reaction
            {
                const substance_arr_t * cur_subs_arr;
                const substance_arr_t * const react[2] = {&input.react->reactants,&input.react->products};
                char units[] = {'m','g','k'};
                catstr_to_output_buffer("Reaccion teorica:\r\n\r\n",sizeof("Reaccion teorica:\r\n\r\n")-1);
                for(size_t units_index = 0; units_index < 3; units_index++)
                {
                    for(size_t i = 0; i < 2; i++)
                    {
                        cur_subs_arr = react[i];
                        for(size_t i = 0; i < cur_subs_arr->nsubstances; i++)
                        {
                            num_t num = cur_subs_arr->substances[i].amount;
                            const char * str;
                            size_t str_len;
                            switch(units[units_index])
                            {
                                case 'm':
                                    str = " mol de ";
                                    str_len = sizeof(" mol de ");
                                    break;
                                case 'g':
                                    num *= cur_subs_arr->substances[i].molar_mass;
                                    str = " g de ";
                                    str_len = sizeof(" g de ");
                                    break;
                                case 'k':
                                    num *= cur_subs_arr->substances[i].molar_mass;
                                    num /= 1000;
                                    str = " kg de ";
                                    str_len = sizeof(" kg de ");
                                    break;
                            }
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf",num));
                            catstr_to_output_buffer(str,str_len-1);

                            {
                                size_t amount = cur_subs_arr->substances[i].amount;
                                cur_subs_arr->substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&cur_subs_arr->substances[i],buffer,1000));
                                cur_subs_arr->substances[i].amount = amount;
                            }


                            if(i != cur_subs_arr->nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",sizeof(" + ")-1);
                            }
                        }

                        if(i == 0)
                        {
                            catstr_to_output_buffer(" ---> ",sizeof(" ---> ")-1);
                        }
                        else
                        {
                            catstr_to_output_buffer("\r\n\r\n",4);
                        }
                    }
                }

            }

            //print the real reaction and print which is the excess reagent and the limiting reagent
            while(1)
            {
                //see if at least two variables of the reactants are ready
                unsigned char counter = 0;
                for(size_t i = 0; i < real_reactants->nsubstances; i++)
                {
                    if(real_reactants->substances[i].mol != 0)
                    {
                        counter++;
                    }
                }

                if(counter == 0)
                {
                    //kill the while statement
                    break;
                }

                if(counter == 1)
                {
                    //we just have data about one reactant so...

                    //get limiting
                    for(size_t i = 0; i < real_reactants->nsubstances; i++)
                    {
                        if(real_reactants->substances[i].mol != 0)
                        {
                            limiting_reagent_index = i;
                            break;
                        }
                    }
                }
                else
                {
                    //get limiting
                    for(size_t i = 0; i < real_reactants->nsubstances; i++)
                    {
                        if(real_reactants->substances[i].mol != 0)
                        {
                            limiting_reagent_index = i;
                            break;
                        }
                    }
                    seed = real_reactants->substances[limiting_reagent_index].mol / theoretical_reactants->substances[limiting_reagent_index].amount;
                    for(size_t i = limiting_reagent_index+1; i < real_reactants->nsubstances; i++)
                    {
                        if(real_reactants->substances[i].mol == 0) continue;
                        if(theoretical_reactants->substances[i].amount*seed > real_reactants->substances[i].mol)
                        {
                            limiting_reagent_index = i;
                            seed = real_reactants->substances[limiting_reagent_index].mol / theoretical_reactants->substances[limiting_reagent_index].amount;
                        }
                    }
                    //get excess
                    if(counter == 2)
                    {
                        for(size_t i = 0; i < real_reactants->nsubstances; i++)
                        {
                            if(limiting_reagent_index != i && real_reactants->substances[i].mol != 0)
                            {
                                excess_reagent_index = i;
                                break;
                            }
                        }
                    }
                    else
                    {
                        //There are more than just two
                        //The given reactants may be incompatible
                        bool incompatible_reactants = false;
                        for(size_t i = 0; i < real_reactants->nsubstances; i++)
                        {
                            if(real_reactants->substances[i].mol == 0 || i == limiting_reagent_index) continue;
                            if(theoretical_reactants->substances[i].amount*seed > real_reactants->substances[i].mol)
                            {
                                //What we require is greater than what we have
                                //Which means, it is incompatible
                                incompatible_reactants = true;
                                break;
                            }
                        }
                        if(incompatible_reactants)
                        {
                            break;//They are incompatibe, there isn't a real reaction
                        }
                    }
                }

                {
                    const substance_arr_t * cur_subs_arr;
                    const substance_arr_t * const react[2] = {&input.react->reactants,&input.react->products};
                    num_t seed = real_reactants->substances[limiting_reagent_index].mol / input.react->reactants.substances[limiting_reagent_index].amount;
                    char units[] = {'m','g','k'};
                    catstr_to_output_buffer("\r\n\r\nReaccion real:\r\n\r\n",sizeof("\r\n\r\nReaccion real:\r\n\r\n")-1);
                    for(size_t units_index = 0; units_index < 3; units_index++)
                    {
                        for(size_t i = 0; i < 2; i++)
                        {
                            cur_subs_arr = react[i];
                            for(size_t i = 0; i < cur_subs_arr->nsubstances; i++)
                            {
                                num_t num = cur_subs_arr->substances[i].amount*seed;
                                const char * str;
                                size_t str_len;
                                switch(units[units_index])
                                {
                                    case 'm':
                                        str = " mol de ";
                                        str_len = sizeof(" mol de ");
                                        break;
                                    case 'g':
                                        num *= cur_subs_arr->substances[i].molar_mass;
                                        str = " g de ";
                                        str_len = sizeof(" g de ");
                                        break;
                                    case 'k':
                                        num *= cur_subs_arr->substances[i].molar_mass;
                                        num /= 1000;
                                        str = " kg de ";
                                        str_len = sizeof(" kg de ");
                                        break;
                                }
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%lf",num));
                                catstr_to_output_buffer(str,str_len-1);

                                {
                                    size_t amount = cur_subs_arr->substances[i].amount;
                                    cur_subs_arr->substances[i].amount = 1;
                                    catstr_to_output_buffer(buffer,print_substance(&cur_subs_arr->substances[i],buffer,1000));
                                    cur_subs_arr->substances[i].amount = amount;
                                }

                                if(i != cur_subs_arr->nsubstances-1)
                                {
                                    catstr_to_output_buffer(" + ",sizeof(" + ")-1);
                                }
                            }

                            if(i == 0)
                            {
                                catstr_to_output_buffer(" ---> ",sizeof(" ---> ")-1);
                            }
                            else
                            {
                                catstr_to_output_buffer("\r\n\r\n",4);
                            }
                        }
                    }
                }              

                if(counter == 1)
                {
                    //kill the while statement
                    break;
                }

                //print real reaction
                if(counter == 2)
                {
                    if(theoretical_reactants->substances[excess_reagent_index].amount*seed == real_reactants->substances[excess_reagent_index].mol)
                    {
                        break;//There are not limiting and excess
                    }
                    catstr_to_output_buffer("\r\nReactivo limitante: ",sizeof("\r\nReactivo limitante: ")-1);
                    {
                        size_t amount = theoretical_reactants->substances[limiting_reagent_index].amount;
                        theoretical_reactants->substances[limiting_reagent_index].amount = 1;
                        catstr_to_output_buffer(buffer,print_substance(&theoretical_reactants->substances[limiting_reagent_index],buffer,1000));
                        theoretical_reactants->substances[limiting_reagent_index].amount = amount;
                    }
                    catstr_to_output_buffer("\r\nReactivo en exceso: ",sizeof("\r\nReactivo en exceso: ")-1);
                    {
                        size_t amount = theoretical_reactants->substances[excess_reagent_index].amount;
                        theoretical_reactants->substances[excess_reagent_index].amount = 1;
                        catstr_to_output_buffer(buffer,print_substance(&theoretical_reactants->substances[excess_reagent_index],buffer,1000));                
                        theoretical_reactants->substances[excess_reagent_index].amount = amount;
                    }
                }
                else
                {
                    catstr_to_output_buffer("\r\nReactivo limitante: ",sizeof("\r\nReactivo limitante: ")-1);
                    {
                        size_t amount = theoretical_reactants->substances[limiting_reagent_index].amount;
                        theoretical_reactants->substances[limiting_reagent_index].amount = 1;
                        catstr_to_output_buffer(buffer,print_substance(&theoretical_reactants->substances[limiting_reagent_index],buffer,1000));
                        theoretical_reactants->substances[limiting_reagent_index].amount = amount;
                    }
                }

                //kill the while statement
                break;
            }
            catstr_to_output_buffer("\r\n\r\n",4);
            
            for(size_t j = 0; j < 2; j++)
            {
                for(size_t i = 0; i < var_arrs[j]->nsubstances; i++)
                {
                    if(var_arrs[j]->substances[i].mol != 0)
                    {
                        substance_t * subs = (j == 0 ? &react->reactants.substances[i] : &react->products.substances[i]);
                        size_t amount;

                        seed = var_arrs[j]->substances[i].mol / subs->amount;

                        amount = subs->amount;
                        subs->amount = 1;
                        if(j == 0)
                        {
                            catstr_to_output_buffer("\r\nSi hay ",sizeof("\r\nSi hay ")-1);
                        }
                        else
                        {
                            catstr_to_output_buffer("\r\nSi se necesita producir ",sizeof("\r\nSi se necesita producir ")-1);
                        }
                        switch(var_arrs[j]->substances[i].unit)
                        {
                            case 'm':
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%lf mol de ",var_arrs[j]->substances[i].mol));
                                break;
                            case 'g':
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%lf g de ",var_arrs[j]->substances[i].mol*var_arrs[j]->substances[i].molar_mass));
                                break;
                            case 'k':
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%lf kg de ",(var_arrs[j]->substances[i].mol*var_arrs[j]->substances[i].molar_mass)/1000));
                                break;
                        }
                        {
                            catstr_to_output_buffer(buffer,print_substance(subs,buffer,1000));
                        }
                        catstr_to_output_buffer("\r\n\r\n",4);
                        subs->amount = amount;

                        //mol
                        for (size_t i = 0; i < react->reactants.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf mol ",react->reactants.substances[i].amount*seed));

                            {
                                size_t amount = react->reactants.substances[i].amount;
                                react->reactants.substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&react->reactants.substances[i],buffer,1000));
                                react->reactants.substances[i].amount = amount;
                            }

                            if(i != react->reactants.nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",3);
                            }
                        }
                        catstr_to_output_buffer(" ---> ",sizeof(" ---> ")-1);
                        for (size_t i = 0; i < react->products.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf mol ",react->products.substances[i].amount*seed));

                            {
                                size_t amount = react->products.substances[i].amount;
                                react->products.substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&react->products.substances[i],buffer,1000));
                                react->products.substances[i].amount = amount;
                            }

                            if(i != react->products.nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",3);
                            }
                        }
                        catstr_to_output_buffer("\r\n\r\n",4);

                        //g
                        for (size_t i = 0; i < react->reactants.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf g ",(react->reactants.substances[i].amount*seed)*react->reactants.substances[i].molar_mass));

                            {
                                size_t amount = react->reactants.substances[i].amount;
                                react->reactants.substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&react->reactants.substances[i],buffer,1000));
                                react->reactants.substances[i].amount = amount;
                            }

                            if(i != react->reactants.nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",3);
                            }
                        }
                        catstr_to_output_buffer(" ---> ",sizeof(" ---> ")-1);
                        for (size_t i = 0; i < react->products.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf g ",(react->products.substances[i].amount*seed)*react->products.substances[i].molar_mass));

                            {
                                size_t amount = react->products.substances[i].amount;
                                react->products.substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&react->products.substances[i],buffer,1000));
                                react->products.substances[i].amount = amount;
                            }

                            if(i != react->products.nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",3);
                            }
                        }
                        catstr_to_output_buffer("\r\n\r\n",4);

                        //kg
                        for (size_t i = 0; i < react->reactants.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf kg ",(react->reactants.substances[i].amount*seed)*react->reactants.substances[i].molar_mass/100));

                            {
                                size_t amount = react->reactants.substances[i].amount;
                                react->reactants.substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&react->reactants.substances[i],buffer,1000));
                                react->reactants.substances[i].amount = amount;
                            }

                            if(i != react->reactants.nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",3);
                            }
                        }
                        catstr_to_output_buffer(" ---> ",sizeof(" ---> ")-1);
                        for (size_t i = 0; i < react->products.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%lf kg ",(react->products.substances[i].amount*seed)*react->products.substances[i].molar_mass/1000));

                            {
                                size_t amount = react->products.substances[i].amount;
                                react->products.substances[i].amount = 1;
                                catstr_to_output_buffer(buffer,print_substance(&react->products.substances[i],buffer,1000));
                                react->products.substances[i].amount = amount;
                            }

                            if(i != react->products.nsubstances-1)
                            {
                                catstr_to_output_buffer(" + ",3);
                            }
                        }
                        catstr_to_output_buffer("\r\n\r\n",4);

                    }
            
                }
            }

        }

        for(size_t i = 0; i < input.react->reactants.nsubstances; i++)
        {
            char buffer[100];
            print_substance(&input.react->reactants.substances[i],buffer,100);
            printf(" %s ",buffer);
        }
        putchar('\n');

        flush_output_buffer();        
    }

    return 0;
}
