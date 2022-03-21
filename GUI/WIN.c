#include "GUI.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <pthread.h>
#include <semaphore.h>

//global--------------------------------------

//class name of mainwnd
#define MAIN_WND_CLASSNAME "MAINWND"

//child-window identifiers
#define EDIT_REACTANTS_ID (HMENU)1
#define STATIC_REACTANTS_TITLE_ID (HMENU)2
#define EDIT_PRODUCTS_ID (HMENU)3
#define STATIC_PRODUCTS_TITLE_ID (HMENU)4

//size of controls
#define LINE_HEIGHT 20

#define EDIT_REACTANTS_WIDTH 1300
#define EDIT_REACTANTS_HEIGHT LINE_HEIGHT

#define STATIC_REACTANTS_TITLE_WIDTH 70
#define STATIC_REACTANTS_TITLE_HEIGHT LINE_HEIGHT

#define EDIT_PRODUCTS_WIDTH EDIT_REACTANTS_WIDTH
#define EDIT_PRODUCTS_HEIGHT LINE_HEIGHT

#define STATIC_PRODUCTS_TITLE_WIDTH 70
#define STATIC_PRODUCTS_TITLE_HEIGHT LINE_HEIGHT

//pos of controls
#define EDIT_REACTANTS_X 20
#define EDIT_REACTANTS_Y 50

#define STATIC_REACTANTS_TITLE_X EDIT_REACTANTS_X
#define STATIC_REACTANTS_TITLE_Y EDIT_REACTANTS_Y-LINE_HEIGHT

#define EDIT_PRODUCTS_X EDIT_REACTANTS_X
#define EDIT_PRODUCTS_Y EDIT_REACTANTS_Y+STATIC_REACTANTS_TITLE_HEIGHT+LINE_HEIGHT+10

#define STATIC_PRODUCTS_TITLE_X EDIT_PRODUCTS_X
#define STATIC_PRODUCTS_TITLE_Y EDIT_PRODUCTS_Y-LINE_HEIGHT


//mutex and cond for input
pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t input_condv = PTHREAD_COND_INITIALIZER;

//GUI
typedef enum {
    //errors at initializing
    GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED = -1,
    GUI_STATE_ERROR_CANNOT_INIT_SEMWAIT_FAILED = -2,

    //success
    GUI_STATE_INITIALIZED = 0
    }
    GUI_state_t;
GUI_state_t GUI_state;

//global--------------------------------------

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

void * app_loop(void * hwnd);

int start_app(void)
{
    WNDCLASSEX MainWndClass;

    pthread_t thread_apploop;

    sem_t sem;

    if(sem_init(&sem,0,0) == -1)
    {
        return EXIT_FAILURE;
    }

    MainWndClass = (WNDCLASSEX){
        .cbSize = sizeof(MainWndClass),
        .style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
        .lpfnWndProc = MainWndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = NULL,
        .hIcon = LoadIcon(NULL,IDI_APPLICATION),
        .hCursor = LoadCursor(NULL,IDC_ARROW),
        .hbrBackground = GetStockObject(GRAY_BRUSH),
        .lpszMenuName = NULL,
        .lpszClassName = MAIN_WND_CLASSNAME,
        .hIconSm = NULL
    };
    if(RegisterClassEx(&MainWndClass) == 0)
    {
        msg_app("Error critico:","RegisterClassEx() fallo.");
        return EXIT_FAILURE;
    }

    if(pthread_create(&thread_apploop,NULL,app_loop,&sem) != 0)
    {
        msg_app("Error critico:","pthread_create() fallo.");
        return EXIT_FAILURE;
    }
    if(pthread_detach(thread_apploop) != 0)
    {
        msg_app("Error critico:","pthread_detach() fallo.");
        return EXIT_FAILURE;
    }

    if(sem_wait(&sem) == -1)
    {
        sem_destroy(&sem);
        msg_app("Error critico:","sem_wait() fallo.");
        return EXIT_FAILURE;
    }
    sem_destroy(&sem);

    switch (GUI_state)
    {
    case GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED:
        msg_app("Error critico:","CreateWindow() fallo.");
        return EXIT_FAILURE;
    case GUI_STATE_ERROR_CANNOT_INIT_SEMWAIT_FAILED:
        msg_app("Error critico:","sem_wait() fallo.");
        return EXIT_FAILURE;

    case GUI_STATE_INITIALIZED:
        break;

    default:
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int end_app(void)
{
    if(
        pthread_mutex_destroy(&input_mutex) == -1
        ||
        pthread_cond_destroy(&input_condv) == -1
    )
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void msg_app(const char * MBTitle, const char * MBMsg)
{
    MessageBox(HWND_DESKTOP,MBMsg,MBTitle,MB_OK|MB_ICONINFORMATION|MB_DEFBUTTON1|MB_SYSTEMMODAL);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;

    static HWND hedit_reactants;
    static HWND hedit_products;
    static HWND hstatic_reactants_title;
    static HWND hstatic_products_title;

    switch (Msg)
    {
    case WM_COMMAND:
        break;
    case WM_CREATE:
        {
            sem_t * app_loop_sem = ((CREATESTRUCT*)lParam)->lpCreateParams;

            hInstance = ((CREATESTRUCT*)lParam)->hInstance;

            hedit_reactants = CreateWindowEx
                (
                    0,
                    WC_EDIT,
                    "",
                    WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_BORDER|ES_CENTER,
                    EDIT_REACTANTS_X,
                    EDIT_REACTANTS_Y,
                    EDIT_REACTANTS_WIDTH,
                    EDIT_REACTANTS_HEIGHT,
                    hWnd,
                    EDIT_REACTANTS_ID,
                    hInstance,
                    NULL
                );
            if(hedit_reactants == NULL)
            {
                GUI_state = GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED;
                if(sem_post(app_loop_sem) == -1)
                {
                    msg_app("Error critico:","sem_post() fallo.");
                    exit(EXIT_FAILURE);
                }
                return 0;
            }

            hstatic_reactants_title = CreateWindowEx
                (
                    0,
                    WC_STATIC,
                    "Reactivos",
                    WS_CHILD|WS_VISIBLE|WS_BORDER|WS_OVERLAPPED|SS_LEFT,
                    STATIC_REACTANTS_TITLE_X,
                    STATIC_REACTANTS_TITLE_Y,
                    STATIC_PRODUCTS_TITLE_WIDTH,
                    STATIC_PRODUCTS_TITLE_HEIGHT,
                    hWnd,
                    STATIC_REACTANTS_TITLE_ID,
                    hInstance,
                    NULL
                );
            if(hstatic_reactants_title == NULL)
            {
                GUI_state = GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED;
                if(sem_post(app_loop_sem) == -1)
                {
                    msg_app("Error critico:","sem_post() fallo.");
                    exit(EXIT_FAILURE);
                }
                return 0;
            }

            hedit_products = CreateWindowEx
                (
                    0,
                    WC_EDIT,
                    "",
                    WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_BORDER|ES_CENTER,
                    EDIT_PRODUCTS_X,
                    EDIT_PRODUCTS_Y,
                    EDIT_PRODUCTS_WIDTH,
                    EDIT_PRODUCTS_HEIGHT,
                    hWnd,
                    EDIT_PRODUCTS_ID,
                    hInstance,
                    NULL
                );
            if(hedit_products == NULL)
            {
                GUI_state = GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED;
                if(sem_post(app_loop_sem) == -1)
                {
                    msg_app("Error critico:","sem_post() fallo.");
                    exit(EXIT_FAILURE);
                }
                return 0;
            }

            hstatic_products_title = CreateWindowEx
                (
                    0,
                    WC_STATIC,
                    "Productos",
                    WS_CHILD|WS_VISIBLE|WS_BORDER|WS_OVERLAPPED|SS_LEFT,
                    STATIC_PRODUCTS_TITLE_X,
                    STATIC_PRODUCTS_TITLE_Y,
                    STATIC_PRODUCTS_TITLE_WIDTH,
                    STATIC_PRODUCTS_TITLE_HEIGHT,
                    hWnd,
                    STATIC_PRODUCTS_TITLE_ID,
                    hInstance,
                    NULL
                );
            if(hstatic_products_title == NULL)
            {
                GUI_state = GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED;
                if(sem_post(app_loop_sem) == -1)
                {
                    msg_app("Error critico:","sem_post() fallo.");
                    exit(EXIT_FAILURE);
                }
                return 0;
            }

            GUI_state = GUI_STATE_INITIALIZED;
            sem_post(app_loop_sem);
        }    
        break;
    default:
        return DefWindowProc(hWnd,Msg,wParam,lParam);
    }
    return 0;
}

void * app_loop(void * hwnd)
{
    sem_t * start_app_sem = hwnd, sem;
    MSG Msg;
    HWND hMainWnd;

    sem_init(&sem,0,0);

    hMainWnd = CreateWindowEx
        (
            0,
            MAIN_WND_CLASSNAME,
            "COCA",
            WS_OVERLAPPED,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            HWND_DESKTOP,
            NULL,
            NULL,
            &sem
        );
    if(hMainWnd == NULL)
    {
        GUI_state = GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED;
        sem_destroy(&sem);
        if(sem_post(start_app_sem) == -1)
        {
            msg_app("Error critico:","sem_post() fallo.");
            exit(EXIT_FAILURE);
        }
        return NULL;
    }
    ShowWindow(hMainWnd,SW_SHOWMAXIMIZED);

    if(sem_wait(&sem) == -1)
    {
        GUI_state = GUI_STATE_ERROR_CANNOT_INIT_SEMWAIT_FAILED;
        sem_destroy(&sem);    
        sem_post(start_app_sem);
        return NULL;
    }

    sem_destroy(&sem);

    if(sem_post(start_app_sem) == -1)
    {
        msg_app("Error critico:","sem_post() fallo.");
        exit(EXIT_FAILURE);
    }

    while(GetMessage(&Msg,NULL,0,0) == TRUE)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return NULL;
}

