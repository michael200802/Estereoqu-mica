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
            case INPUT_ERROR_READING:
                msg_app("Error en el input.","Se le comunicara el error al Michu.");
                return end_app();
            case INPUT_ERROR_WND_CLOSED:
                msg_app("Vuelva pronto,","Adios.");
                return end_app();           
        }

    }

    return 0;
}