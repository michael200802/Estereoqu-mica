#include "GUI.h"
#include <stdlib.h>

char * buffer = NULL;
size_t buffer_size = 0;
size_t buffer_str_len = 1;

inline void catstr_to_output_buffer(const char * str, size_t str_len)
{
    if(buffer_size < buffer_str_len+str_len)
    {
        buffer_size = buffer_str_len+str_len+100;

        buffer = realloc(buffer,buffer_size);
    }

    char * buf;
    for(buf = buffer+buffer_str_len-1; *str != '\0'; str++, buf++, buffer_str_len++)
    {
        *buf = *str;
    }
    *buf = '\0';
}

inline void flush_output_buffer(void)
{
    show_output(buffer);

    free(buffer);
    buffer = NULL;
    buffer_size = 0;
    buffer_str_len = 1;
}

inline void end_output(void)
{
    if(buffer_size != 0)
    {
        free(buffer);
    }
}

