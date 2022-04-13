#include "GUI.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <pthread.h>
#include <semaphore.h>

//global--------------------------------------

//class name of mainwnd
#define MAIN_WND_CLASSNAME "MAINWND"

//class name of pop-up (output)
#define POPUP_WND_CLASSNAME "OUTPUT_POPUP"

//child-window identifiers
#define EDIT_REACTANTS_ID ((HMENU)1)
#define STATIC_REACTANTS_TITLE_ID ((HMENU)2)
#define EDIT_PRODUCTS_ID ((HMENU)3)
#define STATIC_PRODUCTS_TITLE_ID ((HMENU)4)
#define STATIC_ERROR_ID ((HMENU)5)
#define BUTTON_GETOUTPUT_ID ((HMENU)6)
#define CTRLS_VARIABLE_ID(n) ((HMENU)(n))

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

#define STATIC_ERROR_WIDTH EDIT_PRODUCTS_WIDTH
#define STATIC_ERROR_HEIGHT LINE_HEIGHT

#define BUTTON_GETOUTPUT_WIDTH 70
#define BUTTON_GETOUTPUT_HEIGHT LINE_HEIGHT

#define CTRLS_VARIABLE_WIDTH 600
#define CTRLS_VARIABLE_HEIGHT LINE_HEIGHT*4

#define POPUP_OUTPUT_WIDTH 1000
#define POPUP_OUTPUT_HEIGHT 600

//pos of controls
#define EDIT_REACTANTS_X 20
#define EDIT_REACTANTS_Y 50

#define STATIC_REACTANTS_TITLE_X EDIT_REACTANTS_X
#define STATIC_REACTANTS_TITLE_Y (EDIT_REACTANTS_Y-LINE_HEIGHT)

#define EDIT_PRODUCTS_X EDIT_REACTANTS_X
#define EDIT_PRODUCTS_Y (EDIT_REACTANTS_Y+STATIC_REACTANTS_TITLE_HEIGHT+LINE_HEIGHT+10)

#define STATIC_PRODUCTS_TITLE_X EDIT_PRODUCTS_X
#define STATIC_PRODUCTS_TITLE_Y (EDIT_PRODUCTS_Y-LINE_HEIGHT)

#define STATIC_ERROR_X (EDIT_REACTANTS_X + (EDIT_REACTANTS_WIDTH-STATIC_ERROR_WIDTH)/2)
#define STATIC_ERROR_Y (STATIC_PRODUCTS_TITLE_Y + LINE_HEIGHT*3)

#define BUTTON_GETOUTPUT_X (EDIT_REACTANTS_X + (EDIT_REACTANTS_WIDTH-BUTTON_GETOUTPUT_WIDTH)/2)
#define BUTTON_GETOUTPUT_Y (STATIC_ERROR_Y + LINE_HEIGHT*2)

#define CTRLS_VARIABLES_REACTANTS_X (EDIT_REACTANTS_X + 30)
#define CTRLS_VARIABLES_PRODUCTS_X (CTRLS_VARIABLES_REACTANTS_X + CTRLS_VARIABLE_WIDTH + 40)

#define POPUP_OUTPUT_X 200
#define POPUP_OUTPUT_y 100

//GUI
typedef enum {
    //errors at initializing
    GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED = 1 << 0,
    GUI_STATE_ERROR_CANNOT_INIT_SEMWAIT_FAILED = 1 << 1,

    //success
    GUI_STATE_INITIALIZED = 1 << 2,

    GUI_STATE_BLOCKED = 1 << 3,

    //input errors
    GUI_STATE_ERROR_INPUT = 1 << 4

    }
    GUI_state_t;
GUI_state_t GUI_state;

//var arrays
typedef struct
{
    HWND static_title, edit, combobox, static_error;
    bool is_ready;
}var_ctrl_t;

typedef struct
{
    var_arr_t var_arr;
    
    size_t nctrls;
    var_ctrl_t * ctrls;

}var_arr_ctrls_t;

//global--------------------------------------

static inline void init_var_arr_ctrls(substance_arr_t subs, size_t x_pos, HWND hMainWnd, HINSTANCE hIns, var_arr_ctrls_t * restrict var_arr)
{
    init_var_arr(var_arr->var_arr,subs.nsubstances);
    
    var_arr->nctrls = subs.nsubstances;
    var_arr->ctrls = malloc(sizeof(var_ctrl_t)*var_arr->nctrls);
    if(var_arr->ctrls == NULL)
    {
        msg_app("Error critico:","malloc() ha fallado; no se puede obtener memoria.");
        exit(EXIT_FAILURE);
    }

    size_t cur_y_pos = BUTTON_GETOUTPUT_Y + LINE_HEIGHT*3;
    for(size_t i = 0; i < subs.nsubstances; i++, cur_y_pos+=CTRLS_VARIABLE_HEIGHT)
    {
        var_arr->var_arr.substances[i].molar_mass = subs.substances[i].molar_mass;

        char buffer[75];
        print_substance(&subs.substances[i],buffer,75);        
        var_arr->ctrls[i].static_title = CreateWindowEx(
            0,
            WC_STATIC,
            buffer,
            WS_CHILD|WS_VISIBLE|SS_SIMPLE,
            x_pos,
            cur_y_pos + 5,
            CTRLS_VARIABLE_WIDTH,
            LINE_HEIGHT,
            hMainWnd,
            CTRLS_VARIABLE_ID(x_pos+i*5+1),
            hIns,
            NULL
        );

        var_arr->ctrls[i].edit = CreateWindowEx(
            0,
            WC_EDIT,
            "?",
            WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
            x_pos,
            cur_y_pos + 5 + LINE_HEIGHT + 5,
            CTRLS_VARIABLE_WIDTH-CTRLS_VARIABLE_WIDTH*0.2,
            LINE_HEIGHT,
            hMainWnd,
            CTRLS_VARIABLE_ID(x_pos+i*5+2),
            hIns,
            NULL
        );

        var_arr->ctrls[i].combobox = CreateWindowEx(
            0,
            WC_COMBOBOX,
            "",
            WS_VISIBLE|WS_CHILD|WS_OVERLAPPED|CBS_HASSTRINGS|CBS_DROPDOWNLIST,
            x_pos + (CTRLS_VARIABLE_WIDTH-CTRLS_VARIABLE_WIDTH*0.2),
            cur_y_pos + 5 + LINE_HEIGHT + 5,
            CTRLS_VARIABLE_WIDTH*0.2,
            LINE_HEIGHT,
            hMainWnd,
            CTRLS_VARIABLE_ID(x_pos+i*5+3),
            hIns,
            NULL
        );
        ComboBox_AddString(var_arr->ctrls[i].combobox,"mol");
        ComboBox_AddString(var_arr->ctrls[i].combobox,"g");
        ComboBox_AddString(var_arr->ctrls[i].combobox,"kg");
        ComboBox_SetCurSel(var_arr->ctrls[i].combobox,0);

        var_arr->ctrls[i].static_error = CreateWindowEx(
            0,
            WC_STATIC,
            "",
            WS_CHILD|WS_VISIBLE|SS_SIMPLE,
            x_pos,
            cur_y_pos + 5 + LINE_HEIGHT + 5 + LINE_HEIGHT + 5,
            CTRLS_VARIABLE_WIDTH,
            LINE_HEIGHT,
            hMainWnd,
            CTRLS_VARIABLE_ID(x_pos+i*5+4),
            hIns,
            NULL
        );
        var_arr->ctrls[i].is_ready = true;
    }
}

static inline bool get_all_vars_from_ctrls(const var_arr_ctrls_t * const restrict var_arr)
{
    //Chech if there were any errors
    for(size_t i = 0; i < var_arr->nctrls; i++)
    {
        if(var_arr->ctrls[i].is_ready == false)
        {
            return false;
        }
    }

    char buffer[100];
    num_t num;
    bool all_zero = true;

    //get all variables
    for(size_t i = 0; i < var_arr->nctrls; i++)
    {
        Edit_GetText(var_arr->ctrls[i].edit,buffer,100);
        if(buffer[0] == '?' && buffer[1] == '\0')
        {
            var_arr->var_arr.substances[i].mol = 0;
        }
        else
        {
            all_zero = false;
            if(sscanf(buffer,"%lf",&num) != 1)
            {
                return false;
            }

            switch (ComboBox_GetCurSel(var_arr->ctrls[i].combobox))
            {
                case 0://mol
                    var_arr->var_arr.substances[i].mol = num;
                    break;
                case 2://kg
                    num /= 1000;
                case 1://g
                    var_arr->var_arr.substances[i].mol = num * var_arr->var_arr.substances[i].molar_mass;
                    break;
            }
        }
    }

    if(all_zero)
    {
        return true;
    }
    return true;
}

static inline void destroy_var_arr_ctrls(const var_arr_ctrls_t * const restrict var_arr)
{
    for(size_t i = 0; i < var_arr->nctrls; i++)
    {
        DestroyWindow(var_arr->ctrls[i].static_title);
        DestroyWindow(var_arr->ctrls[i].combobox);
        DestroyWindow(var_arr->ctrls[i].edit);
        DestroyWindow(var_arr->ctrls[i].static_error);
    }
    free(var_arr->ctrls);
    destroy_var_arr(var_arr->var_arr);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK PopUpProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

void * app_loop(void * hwnd);

inline int start_app(void)
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

    //creating the pop-up class
    WNDCLASSEX PopUpWndClass = MainWndClass;
    PopUpWndClass.lpszClassName = POPUP_WND_CLASSNAME;
    PopUpWndClass.lpfnWndProc = PopUpProc;
    if(RegisterClassEx(&PopUpWndClass) == 0)
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

//show window ---> using pop-up to show a string

LRESULT CALLBACK PopUpProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hpopup;
    switch(Msg)
    {
        case WM_SIZE:
            {
                RECT pos;
                GetWindowRect(hWnd,&pos);
                size_t width = pos.right-pos.left;
                size_t height = pos.bottom-pos.top;

                SetWindowPos(hpopup,HWND_TOP,0,0,width-10,height-30,SWP_SHOWWINDOW|SWP_NOMOVE);
            }
            break;
        case WM_CREATE:
            {
                char * str = ((CREATESTRUCT*)lParam)->lpCreateParams;
                hpopup = CreateWindowEx(
                    0,
                    WC_EDIT,
                    str,
                    WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|ES_LEFT|ES_MULTILINE|ES_READONLY,
                    0,
                    0,
                    POPUP_OUTPUT_WIDTH,
                    POPUP_OUTPUT_HEIGHT,
                    hWnd,
                    NULL,
                    NULL,
                    NULL
                );
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd,Msg,wParam,lParam);
    }
    return 0;
}

inline void show_output(const char * str)
{
    HWND hpopup = CreateWindowEx(
            0,
            POPUP_WND_CLASSNAME,
            "",
            WS_VISIBLE|WS_POPUPWINDOW|WS_MINIMIZEBOX|WS_CAPTION|WS_THICKFRAME,
            POPUP_OUTPUT_X,
            POPUP_OUTPUT_y,
            POPUP_OUTPUT_WIDTH + 10,
            POPUP_OUTPUT_HEIGHT + 30,
            HWND_DESKTOP,
            NULL,
            NULL,
            str
        );
    ShowWindow(hpopup,SW_NORMAL);

    MSG Msg;

    while(GetMessage(&Msg,NULL,0,0) == TRUE)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
}

//show window ---> using pop-up to show a string

inline void msg_app(const char * MBTitle, const char * MBMsg)
{
    MessageBox(HWND_DESKTOP,MBMsg,MBTitle,MB_OK|MB_ICONINFORMATION|MB_DEFBUTTON1|MB_SYSTEMMODAL);
}

inline int end_app(void)
{
    return end_input();
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;

    static HWND hedit_reactants;
    static HWND hedit_products;
    static HWND hstatic_reactants_title;
    static HWND hstatic_products_title;
    static HWND hbutton_getoutput;
    static HWND hstatic_error;

    static reaction_t react;
    static bool is_reaction_ready;

    static var_arr_ctrls_t var_arr_ctrls_reactants = {}, var_arr_ctrls_products = {};
    static bool is_var_arr_reactants_ready, is_var_arr_products_ready;

    switch (Msg)
    {
    case WM_COMMAND:
        {
            struct
            {
                char str[1000];
                size_t len;
                bool ready;
            } static edit_reactants_str = {}, edit_products_str = {};

            switch(LOWORD(wParam))
            {
                default:
                    {
                        intptr_t child_id = LOWORD(wParam);
                        if(child_id > CTRLS_VARIABLES_REACTANTS_X)
                        {
                            char ctrl_type = child_id % 5;
                            if(ctrl_type == 2)
                            {
                                size_t var_ctrl_index;
                                var_ctrl_t * var_ctrl;
                                char buffer[75];
                                num_t num;
                                
                                var_ctrl_index = child_id/5;
                                if(child_id > CTRLS_VARIABLES_PRODUCTS_X)
                                {
                                    var_ctrl = &var_arr_ctrls_products.ctrls[var_ctrl_index];
                                }
                                else
                                {
                                    var_ctrl = &var_arr_ctrls_reactants.ctrls[var_ctrl_index];
                                }

                                if(Edit_GetTextLength(var_ctrl->edit) < 75)
                                {
                                    Edit_GetText(var_ctrl->edit,buffer,75);
                                    if(!(buffer[0] == '?' && buffer[1] == '\0') && sscanf(buffer,"%lf",&num) != 1)
                                    {
                                        var_ctrl->is_ready = false;
                                        Static_SetText(var_ctrl->static_error,"Cadena no valida.");   
                                    }
                                    else
                                    {
                                        var_ctrl->is_ready = true;
                                    }
                                }
                                else
                                {
                                    var_ctrl->is_ready = false;
                                    Static_SetText(var_ctrl->static_error,"Cadena muy grande.");
                                }
                            }
                        }
                    }
                    break;
                case (intptr_t)BUTTON_GETOUTPUT_ID:
                    if(HIWORD(wParam) == BN_CLICKED)
                    {
                        if(is_reaction_ready)
                        {
                            if(get_all_vars_from_ctrls(&var_arr_ctrls_reactants))
                            {
                                if(get_all_vars_from_ctrls(&var_arr_ctrls_products))
                                {
                                    if(send_new_input(&react,&var_arr_ctrls_reactants.var_arr,&var_arr_ctrls_products.var_arr) != INPUT_NOERRROR)
                                    {
                                        exit(EXIT_FAILURE);
                                    }
                                }
                            }
                        }
                    }
                    break;
                case (intptr_t)EDIT_PRODUCTS_ID:

                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        if(is_var_arr_products_ready)
                        {
                            destroy_var_arr_ctrls(&var_arr_ctrls_products);
                            is_var_arr_products_ready = false;
                        }
                        
                        edit_products_str.len = Edit_GetTextLength(hedit_products)+1;
                        if(edit_products_str.len < 1000)
                        {
                            Edit_GetText(hedit_products,edit_products_str.str,edit_products_str.len);
                            edit_products_str.ready = true;
                            //puts(edit_products_str.str);

                            if(edit_reactants_str.ready)
                            {
                                if(is_reaction_ready)
                                {
                                    destroy_reaction(&react);
                                    is_reaction_ready = init_reaction(edit_reactants_str.str,edit_products_str.str,&react);
                                    if(is_reaction_ready == false)
                                    {
                                        Static_SetText(hstatic_error,"Productos incorrectos.");
                                    }
                                }
                                else
                                {
                                    is_reaction_ready = init_reaction(edit_reactants_str.str,edit_products_str.str,&react);
                                    if(is_reaction_ready == false)
                                    {
                                        Static_SetText(hstatic_error,"Reaccion incorrecta.");
                                    }
                                }
                                if(is_reaction_ready)
                                {
                                    is_var_arr_products_ready = true;
                                    Static_SetText(hstatic_error,"Todo en orden.");

                                    init_var_arr_ctrls(react.products,CTRLS_VARIABLES_PRODUCTS_X,hWnd,hInstance,&var_arr_ctrls_products);
                                    is_var_arr_products_ready = true;

                                    if(is_var_arr_reactants_ready == false)
                                    {
                                        init_var_arr_ctrls(react.reactants,CTRLS_VARIABLES_REACTANTS_X,hWnd,hInstance,&var_arr_ctrls_reactants);
                                        is_var_arr_reactants_ready = true;                                        
                                    }
                                }
                            }
                            else
                            {
                                Static_SetText(hstatic_error,"Faltan los reactivos.");
                            }
                        }
                        else
                        {
                            Static_SetText(hstatic_error,"Productos demasiado largos.");
                            edit_products_str.ready = false;
                        }
                    }
            
                    break;
                case (intptr_t)EDIT_REACTANTS_ID:

                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        if(is_var_arr_reactants_ready)
                        {
                            destroy_var_arr_ctrls(&var_arr_ctrls_reactants);
                            is_var_arr_reactants_ready = false;
                        }

                        edit_reactants_str.len = Edit_GetTextLength(hedit_reactants)+1;
                        if(edit_reactants_str.len < 1000)
                        {
                            Edit_GetText(hedit_reactants,edit_reactants_str.str,edit_reactants_str.len);
                            edit_reactants_str.ready = true;
                            //puts(edit_reactants_str.str);

                            if(edit_products_str.ready)
                            {
                                if(is_reaction_ready)
                                {
                                    destroy_reaction(&react);
                                    is_reaction_ready = init_reaction(edit_reactants_str.str,edit_products_str.str,&react);
                                    if(is_reaction_ready == false)
                                    {
                                        Static_SetText(hstatic_error,"Reactivos incorrectos.");
                                    }
                                }
                                else
                                {
                                    is_reaction_ready = init_reaction(edit_reactants_str.str,edit_products_str.str,&react);
                                    if(is_reaction_ready == false)
                                    {
                                        Static_SetText(hstatic_error,"Reaccion incorrecta.");
                                    }
                                }
                                if(is_reaction_ready)
                                {
                                    is_var_arr_reactants_ready = true;
                                    Static_SetText(hstatic_error,"Todo en orden.");

                                    init_var_arr_ctrls(react.reactants,CTRLS_VARIABLES_REACTANTS_X,hWnd,hInstance,&var_arr_ctrls_reactants);
                                    is_var_arr_reactants_ready = true;

                                    if(is_var_arr_products_ready == false)
                                    {
                                        init_var_arr_ctrls(react.products,CTRLS_VARIABLES_PRODUCTS_X,hWnd,hInstance,&var_arr_ctrls_products);
                                        is_var_arr_products_ready = true;
                                    }
                                }
                            }
                            else
                            {
                                Static_SetText(hstatic_error,"Te falta ingresar los productos.");
                            }
                        }
                        else
                        {
                            Static_SetText(hstatic_error,"Reactivos demasiado largos.");
                            edit_reactants_str.ready = false;
                        }
                    }
            
                    break;
            }
        }
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

            hbutton_getoutput = CreateWindowEx
                (
                    0,
                    WC_BUTTON,
                    "Resolver",
                    WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|BS_DEFPUSHBUTTON,
                    BUTTON_GETOUTPUT_X,
                    BUTTON_GETOUTPUT_Y,
                    BUTTON_GETOUTPUT_WIDTH,
                    BUTTON_GETOUTPUT_HEIGHT,
                    hWnd,
                    BUTTON_GETOUTPUT_ID,
                    hInstance,
                    NULL
                );
            if(hbutton_getoutput == NULL)
            {
                GUI_state = GUI_STATE_ERROR_CANNOT_INIT_CREATEWINDOW_FAILED;
                if(sem_post(app_loop_sem) == -1)
                {
                    msg_app("Error critico:","sem_post() fallo.");
                    exit(EXIT_FAILURE);
                }
                return 0;   
            }

            hstatic_error = CreateWindowEx
                (
                    0,
                    WC_STATIC,
                    "Ingrese la reaccion.",
                    WS_CHILD|WS_VISIBLE|SS_CENTER,
                    STATIC_ERROR_X,
                    STATIC_ERROR_Y,
                    STATIC_ERROR_WIDTH,
                    STATIC_ERROR_HEIGHT,
                    hWnd,
                    STATIC_ERROR_ID,
                    hInstance,
                    NULL
                );
            if(hstatic_error == NULL)
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
            if(sem_post(app_loop_sem) == -1)
            {
                msg_app("Error critico:","sem_post() fallo.");
                exit(EXIT_FAILURE);
            }
        }    
        break;
    case WM_DESTROY:
        if(is_reaction_ready)
        {
            destroy_reaction(&react);
        }
        if(is_var_arr_products_ready)
        {
            destroy_var_arr_ctrls(&var_arr_ctrls_products);
        }
        if(is_var_arr_reactants_ready)
        {
            destroy_var_arr_ctrls(&var_arr_ctrls_reactants);
        }
        send_endwnd_signal();
        PostQuitMessage(0);
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
