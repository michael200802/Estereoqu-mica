#include "GUI.h"
#include <windows.h>
#include <pthread.h>

//global--------------------------------------

//class name of mainwnd
#define MAIN_WND_CLASSNAME "MAINWND"

//child-window identifiers
#define EDIT_EQUATION_ID 1
#define STATIC_EQUATION_TITLE_ID 2
#define EDIT_RESULT_ID 3
#define EDT_RESULT_TITLE_ID 4

//mutex and cond for input
pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t input_condv = PTHREAD_COND_INITIALIZER;

//global--------------------------------------

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

void * app_loop(void * hwnd);

int start_app(void)
{
    HWND hMainWnd;
    WNDCLASSEX MainWndClass;
    MSG Msg;

    pthread_t thread_apploop;

    MainWndClass = (WNDCLASSEX){
        .cbSize = sizeof(MainWndClass),
        .style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
        .lpfnWndProc = MainWndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = NULL,
        .hIcon = LoadIcon(NULL,IDI_APPLICATION),
        .hCursor = LoadIcon(NULL,IDC_ARROW),
        .hbrBackground = COLOR_APPWORKSPACE,
        .lpszMenuName = NULL,
        .lpszClassName = MAIN_WND_CLASSNAME,
        .hIconSm = NULL
    };
    if(RegisterClassEx(&MainWndClass) == 0)
    {
        exit_app("Error critico:","RegisterClassEx() fallo.");
        return EXIT_FAILURE;
    }

    hMainWnd = CreateWindowEx
        (
            0,
            MAIN_WND_CLASSNAME,
            "COCA",
            CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_DROPSHADOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            HWND_DESKTOP,
            NULL,
            NULL,
            NULL
        );
    if(hMainWnd == NULL)
    {
        exit_app("Error critico:","CreateWindowEx() fallo.");
        return EXIT_FAILURE;
    }

    ShowWindow(hMainWnd,SW_MAXIMIZE);

    if(pthread_create(&thread_apploop,NULL,app_loop,hMainWnd) != 0)
    {
        exit_app("Error critico:","pthread_create() fallo.");
        return EXIT_FAILURE;
    }
    if(pthread_detach(thread_apploop) != 0)
    {
        exit_app("Error critico:","pthread_detach() fallo.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void exit_app(const char * MBTitle, const char * MBMsg)
{
    MessageBox(HWND_DESKTOP,MBMsg,MBTitle,MB_OK|MB_ICONINFORMATION|MB_DEFBUTTON1|MB_SYSTEMMODAL);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hedit_equation;
    static HWND hedit_result;
    static HWND hstatic_equation_title;
    static HWND hstatic_result_title;

    switch (Msg)
    {
    case WM_COMMAND:
        break;
    case WM_CREATE:
        break;
    case WM_DESTROY:
        break;
    default:
        return DefWindowProc(hWnd,Msg,wParam,lParam);
    }
    return 0;
}

void * app_loop(void * hwnd)
{
    MSG Msg;
    HWND hMainWnd = hwnd;

    while(GetMessage(&Msg,NULL,0,0) == TRUE)
    {
        if(TranslateMessage(&Msg) == false)
        {

            return NULL;
        }
        DispatchMessage(&Msg);
    }
}

