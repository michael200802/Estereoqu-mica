#include "GUI.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

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

inline void print_num_in_output_buffer(num_t num)
{
    char buffer[100];
    if((long long)(num/1000) == 0)
    {
        sprintf(buffer,"%lf",num);
    }
    else
    {
        sprintf(buffer,"%.3lf",num);
    }
}

inline void flush_output_buffer(void)
{
    pthread_t thread;
    sem_t sem;

    sem_init(&sem,0,0);

    struct
    {
        sem_t * sem;
        const char * str;
    }thread_arg = {.sem = &sem, .str = buffer};

    if(pthread_create(&thread,NULL,show_output,&thread_arg) != 0)
    {
        msg_app("Error critico:","pthread_create() ha fallado.");
        exit(EXIT_FAILURE);
    }
    pthread_detach(thread);

    sem_wait(&sem);

    free(buffer);
    buffer = NULL;
    buffer_size = 0;
    buffer_str_len = 1;

    sem_destroy(&sem);
}

inline void end_output(void)
{
    if(buffer_size != 0)
    {
        free(buffer);
    }
}

