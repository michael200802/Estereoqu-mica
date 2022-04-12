#include "GUI.h"
#include <pthread.h>

//mutex and cond for input
pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t input_condv = PTHREAD_COND_INITIALIZER;

//wnd input
input_t cur_input = {};
bool is_cur_input_ready = false;

inline input_errcode_t send_new_input(const reaction_t * restrict const react, const var_arr_t * restrict const reactants, const var_arr_t * restrict const products)
{
    //send input

    if(pthread_mutex_lock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_lock() fallo.");
        return INPUT_ERROR_MUTEX;
    }

    cur_input.react = react;
    cur_input.var_arr_reactants = reactants;
    cur_input.var_arr_products = products;

    is_cur_input_ready = true;

    if(pthread_mutex_unlock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_unlock() fallo.");
        return INPUT_ERROR_MUTEX;
    }

    if(pthread_cond_signal(&input_condv) != 0)
    {
        msg_app("Error critico:","pthread_cond_signal() fallo.");
        return INPUT_ERROR_CONDV_SIGNAL;
    }

    //Now we wait until main() is ready for getting input again

    if(pthread_mutex_lock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_lock() fallo.");
        return INPUT_ERROR_MUTEX;
    }

    if(pthread_cond_wait(&input_condv,&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_cond_wait() fallo.");
        return INPUT_ERROR_CONDV_WAIT;
    }

    if(pthread_mutex_unlock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_unlock() fallo.");
        return INPUT_ERROR_MUTEX;
    }

    return INPUT_NOERRROR;
}

inline input_t get_cur_input(void)
{
    input_t error_return = {.error_code = INPUT_ERROR_READING};

    if(pthread_cond_signal(&input_condv) != 0)
    {
        msg_app("Error critico:","pthread_cond_signal() fallo.");
        return (input_t){.error_code = INPUT_ERROR_CONDV_SIGNAL};
    }

    if(pthread_mutex_lock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_lock() fallo.");
        return (input_t){INPUT_ERROR_MUTEX};
    }

    if(!is_cur_input_ready)
    {
        if(pthread_cond_wait(&input_condv,&input_mutex) != 0)
        {
            msg_app("Error critico:","pthread_cond_wait() fallo.");
            return (input_t){INPUT_ERROR_CONDV_WAIT};
        }
    }

    is_cur_input_ready = false;

    if(pthread_mutex_unlock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_unlock() fallo.");
        return (input_t){INPUT_ERROR_MUTEX};
    }

    return cur_input;
}

inline void send_endwnd_signal(void)
{
    if(pthread_mutex_lock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_lock() fallo.");
        exit(EXIT_FAILURE);
    }

    cur_input = (input_t){.error_code = INPUT_ERROR_WND_CLOSED};

    is_cur_input_ready = true;

    if(pthread_mutex_unlock(&input_mutex) != 0)
    {
        msg_app("Error critico:","pthread_mutex_unlock() fallo.");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_signal(&input_condv) != 0)
    {
        msg_app("Error critico:","pthread_cond_signal() fallo.");
        exit(EXIT_FAILURE);
    }
}

inline int end_input(void)
{
    if(pthread_mutex_destroy(&input_mutex) == -1 || pthread_cond_destroy(&input_condv) == -1)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

