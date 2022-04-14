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
                msg_app("Vuelva pronto,","Adios.");
                return end_app();           
        }

        {
            const reaction_t * const react = input.react;

            const var_arr_t * const var_arrs[2] = {input.var_arr_reactants, input.var_arr_products};

            num_t seed;
            char buffer[1000];

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
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf mol de ",var_arrs[j]->substances[i].mol));
                                break;
                            case 'g':
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf g de ",var_arrs[j]->substances[i].mol*var_arrs[j]->substances[i].molar_mass));
                                break;
                            case 'k':
                                catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf kg de ",(var_arrs[j]->substances[i].mol*var_arrs[j]->substances[i].molar_mass)/1000));
                                break;
                        }
                        catstr_to_output_buffer(buffer,print_substance(subs,buffer,1000));
                        catstr_to_output_buffer("\r\n\r\n",4);
                        subs->amount = amount;

                        //mol
                        for (size_t i = 0; i < react->reactants.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf mol ",react->reactants.substances[i].amount*seed));

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
                        catstr_to_output_buffer("--->",sizeof("--->")-1);
                        for (size_t i = 0; i < react->products.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf mol ",react->products.substances[i].amount*seed));

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
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf g ",(react->reactants.substances[i].amount*seed)*react->reactants.substances[i].molar_mass));

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
                        catstr_to_output_buffer("--->",sizeof("--->")-1);
                        for (size_t i = 0; i < react->products.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf g ",(react->products.substances[i].amount*seed)*react->products.substances[i].molar_mass));

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
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf kg ",(react->reactants.substances[i].amount*seed)*react->reactants.substances[i].molar_mass/100));

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
                        catstr_to_output_buffer("--->",sizeof("--->")-1);
                        for (size_t i = 0; i < react->products.nsubstances; i++)
                        {
                            catstr_to_output_buffer(buffer,sprintf(buffer,"%.2lf kg ",(react->products.substances[i].amount*seed)*react->products.substances[i].molar_mass/1000));

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

        flush_output_buffer();        
    }

    return 0;
}