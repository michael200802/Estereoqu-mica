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

        catstr_to_output_buffer("Hola Mundo",(sizeof "Hola Mundo") - 1);
        /*
        for(size_t i = 0; i < 100; i++)
        {
            for(size_t i = 0; i < 1000; i++)
            {
                char buffer[100];
                catstr_to_output_buffer(buffer,sprintf(buffer,"%zu",i));
            }
            catstr_to_output_buffer("\r\n",2);
        }
        */

        flush_output_buffer();        
    }

    return 0;
}