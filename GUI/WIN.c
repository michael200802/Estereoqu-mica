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
#define CTRLS_VARIABLE_DIVISOR ((intptr_t)10)

//size of controls
#define LINE_HEIGHT 20

#define EDIT_REACTANTS_WIDTH 1300
#define EDIT_REACTANTS_HEIGHT (LINE_HEIGHT)

#define STATIC_REACTANTS_TITLE_WIDTH 70
#define STATIC_REACTANTS_TITLE_HEIGHT LINE_HEIGHT

#define EDIT_PRODUCTS_WIDTH EDIT_REACTANTS_WIDTH
#define EDIT_PRODUCTS_HEIGHT (LINE_HEIGHT)

#define STATIC_PRODUCTS_TITLE_WIDTH 70
#define STATIC_PRODUCTS_TITLE_HEIGHT LINE_HEIGHT

#define STATIC_ERROR_WIDTH EDIT_PRODUCTS_WIDTH
#define STATIC_ERROR_HEIGHT LINE_HEIGHT

#define BUTTON_GETOUTPUT_WIDTH 70
#define BUTTON_GETOUTPUT_HEIGHT LINE_HEIGHT

#define CTRLS_VARIABLE_WIDTH 600
#define CTRLS_VARIABLE_HEIGHT LINE_HEIGHT*5

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

#define CTRLS_VARIABLES_MIN_Y (BUTTON_GETOUTPUT_Y + LINE_HEIGHT*3)

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

//var ctrl arrays
typedef enum {CTRl_TYPE_STATIC_TITLE, CTRL_TYPE_EDIT, CTRL_TYPE_CB, CTRL_TYPE_OPTIONAL_EDIT, CTRL_TYPE_OPTIONAL_CB, CTRL_TYPE_STATIC_ERROR} var_arr_ctrl_id_type_t;
typedef struct
{
    HWND static_title, edit, combobox, op_edit, op_combobox, static_error;
    bool is_ready;
}var_ctrl_t;//a bunch of ctrls treated as one ctrl that is used to handle the input of the var

typedef struct
{
    //ctrls
    size_t nctrls;
    var_ctrl_t * ctrls;

    //where the input of the ctrls will be stored after a call to get_all_vars_from_ctrls
    var_arr_t var_arr;

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

    size_t cur_y_pos = CTRLS_VARIABLES_MIN_Y;
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
            CTRLS_VARIABLE_ID(x_pos+i*CTRLS_VARIABLE_DIVISOR + CTRl_TYPE_STATIC_TITLE),
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
            CTRLS_VARIABLE_ID(x_pos+i*CTRLS_VARIABLE_DIVISOR + CTRL_TYPE_EDIT),
            hIns,
            NULL
        );

        var_arr->ctrls[i].combobox = CreateWindowEx(
            0,
            WC_COMBOBOX,
            "",
            WS_VISIBLE|WS_CHILD|CBS_HASSTRINGS|CBS_DROPDOWNLIST,
            x_pos + (CTRLS_VARIABLE_WIDTH-CTRLS_VARIABLE_WIDTH*0.2),
            cur_y_pos + 5 + LINE_HEIGHT + 5,
            CTRLS_VARIABLE_WIDTH*0.2,
            LINE_HEIGHT*6,
            hMainWnd,
            CTRLS_VARIABLE_ID(x_pos+i*CTRLS_VARIABLE_DIVISOR + CTRL_TYPE_CB),
            hIns,
            NULL
        );
        ComboBox_AddString(var_arr->ctrls[i].combobox,"mol");
        ComboBox_AddString(var_arr->ctrls[i].combobox,"g");
        ComboBox_AddString(var_arr->ctrls[i].combobox,"kg");
        ComboBox_AddString(var_arr->ctrls[i].combobox,"M");
        ComboBox_AddString(var_arr->ctrls[i].combobox,"m");
        ComboBox_SetCurSel(var_arr->ctrls[i].combobox,0);

        var_arr->ctrls[i].op_edit = var_arr->ctrls[i].op_combobox = NULL;

        var_arr->ctrls[i].static_error = CreateWindowEx(
            0,
            WC_STATIC,
            "Desconocido.",
            WS_CHILD|WS_VISIBLE|SS_SIMPLE,
            x_pos,
            cur_y_pos + 5 + LINE_HEIGHT + 5 + LINE_HEIGHT + 5 + LINE_HEIGHT + 5,
            CTRLS_VARIABLE_WIDTH,
            LINE_HEIGHT,
            hMainWnd,
            CTRLS_VARIABLE_ID(x_pos+i*CTRLS_VARIABLE_DIVISOR + CTRL_TYPE_STATIC_ERROR),
            hIns,
            NULL
        );
        var_arr->ctrls[i].is_ready = true;
    }
}

//returns 0 if one of the ctrls has an error
//returns 1 if sucess
//returns -2 if all inputs are '?'
static inline char get_all_vars_from_ctrls(const var_arr_ctrls_t * const restrict var_arr)
{
    //Chech if there were any errors
    for(size_t i = 0; i < var_arr->nctrls; i++)
    {
        if(var_arr->ctrls[i].is_ready == false)
        {
            return 0;
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
            num = strtod(buffer,NULL);

            switch (ComboBox_GetCurSel(var_arr->ctrls[i].combobox))
            {
                case 0://mol
                    var_arr->var_arr.substances[i].mol = num;
                    var_arr->var_arr.substances[i].unit = 'm';
                    break;
                case 1://g
                    var_arr->var_arr.substances[i].mol = num / var_arr->var_arr.substances[i].molar_mass;
                    var_arr->var_arr.substances[i].unit = 'g';
                    break;
                case 2://kg
                    num /= 1000;
                    var_arr->var_arr.substances[i].mol = num / var_arr->var_arr.substances[i].molar_mass;
                    var_arr->var_arr.substances[i].unit = 'k';
                    break;
                case 3://M
                case 4://m
                    var_arr->var_arr.substances[i].mol = num;
                    var_arr->var_arr.substances[i].unit = 'm';

                    Edit_GetText(var_arr->ctrls[i].op_edit,buffer,100);
                    num = strtod(buffer,NULL);
                    num /= (ComboBox_GetCurSel(var_arr->ctrls[i].op_combobox) == 0 ? 1 : 1000);

                    var_arr->var_arr.substances[i].mol *= num;
                    break;
            }
        }
    }

    if(all_zero)
    {
        return -2;
    }
    return 1;
}

static inline void destroy_var_arr_ctrls(const var_arr_ctrls_t * const restrict var_arr)
{
    for(size_t i = 0; i < var_arr->nctrls; i++)
    {
        DestroyWindow(var_arr->ctrls[i].static_title);
        DestroyWindow(var_arr->ctrls[i].combobox);
        DestroyWindow(var_arr->ctrls[i].edit);
        if(var_arr->ctrls[i].op_edit != NULL)
        {
            DestroyWindow(var_arr->ctrls[i].op_edit);
            DestroyWindow(var_arr->ctrls[i].op_combobox);
        }
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

void * show_output(void * args)
{
    struct
    {
        sem_t * sem;
        const char * str;
    }*thread_arg = args;

    const char * str = thread_arg->str;
    sem_t * sem = thread_arg->sem;

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

    sem_post(sem);

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
    end_output();
    return end_input();
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;//handle of the instance of the program

    //ctrls for the input of the reaction
    static HWND hedit_reactants;
    static HWND hedit_products;
    static HWND hstatic_reactants_title;
    static HWND hstatic_products_title;
    static HWND hbutton_getoutput;
    static HWND hstatic_error;

    static reaction_t react;//it stores the reaction
    static enum {reaction_nonready = false, reaction_ready = true, reaction_unbalanced} is_reaction_ready = reaction_nonready;//is the reaction ready? (the user has finally put the reaction in the reactants and products edits)

    //ctrls for the input of the data of each substance
    static var_arr_ctrls_t var_arr_ctrls_reactants = {}, var_arr_ctrls_products = {};
    static bool is_var_arr_reactants_ready = false, is_var_arr_products_ready = false;

    switch (Msg)
    {
    case WM_COMMAND:
        {
            struct
            {
                char str[1000];//buffer
                size_t len;//buffer len
                bool ready;//is there something stored in the buffer?
            } static edit_reactants_str = {}, edit_products_str = {};//buffers for the edits of the reaction

            switch(LOWORD(wParam)/*child window id*/)
            {
                default://handles Edits and comboboxes of the variables' ctrls
                    {
                        intptr_t ctrl_var_id = LOWORD(wParam);
                        char ctrl_type = ctrl_var_id % CTRLS_VARIABLE_DIVISOR;
                        if(
                            ctrl_var_id > CTRLS_VARIABLES_REACTANTS_X
                            &&
                            (
                                (ctrl_type == CTRL_TYPE_EDIT || ctrl_type == CTRL_TYPE_OPTIONAL_EDIT) && HIWORD(wParam) == EN_CHANGE
                                ||
                                ctrl_type == CTRL_TYPE_CB && HIWORD(wParam) == CBN_SELCHANGE
                            )
                        )
                        {
                            size_t var_ctrl_index;//index in var_arr_ctrls_products or var_arr_ctrls_reactants
                            var_ctrl_t * var_ctrl;//ptr to a var_ctrl structure

                            //get values for var_ctrl_index and var_ctrl
                            var_ctrl_index = ctrl_var_id - (ctrl_var_id < CTRLS_VARIABLES_PRODUCTS_X ? CTRLS_VARIABLES_REACTANTS_X : CTRLS_VARIABLES_PRODUCTS_X);
                            var_ctrl_index /= CTRLS_VARIABLE_DIVISOR;
                            if(ctrl_var_id > CTRLS_VARIABLES_PRODUCTS_X)
                            {
                                var_ctrl = &var_arr_ctrls_products.ctrls[var_ctrl_index];
                            }
                            else
                            {
                                var_ctrl = &var_arr_ctrls_reactants.ctrls[var_ctrl_index];
                            }

                            if(ctrl_type == CTRL_TYPE_CB)
                            {
                                HWND hcombobox = (HWND)lParam;
                                size_t cur_y_pos = CTRLS_VARIABLE_HEIGHT*var_ctrl_index + (CTRLS_VARIABLES_MIN_Y + 5 + LINE_HEIGHT + 5 + LINE_HEIGHT + 5);
                                intptr_t min_ctrl_var_id = ctrl_var_id-ctrl_type;
                                size_t combobox_cursel = ComboBox_GetCurSel(hcombobox);
                                if(var_ctrl->op_combobox != NULL || var_ctrl->op_edit != NULL)
                                {
                                    DestroyWindow(var_ctrl->op_combobox);
                                    DestroyWindow(var_ctrl->op_edit);
                                    var_ctrl->op_combobox = var_ctrl->op_edit = NULL;
                                    var_ctrl->is_ready = true;
                                    if(combobox_cursel != 3 && combobox_cursel != 4)
                                    {
                                        SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(min_ctrl_var_id+CTRL_TYPE_EDIT,EN_CHANGE),(LPARAM)var_ctrl->edit);
                                    }
                                }
                                if(combobox_cursel == 3 || combobox_cursel == 4)
                                {
                                    size_t cur_x_pos = (min_ctrl_var_id < CTRLS_VARIABLES_PRODUCTS_X ? CTRLS_VARIABLES_REACTANTS_X : CTRLS_VARIABLES_PRODUCTS_X);
                                    var_ctrl->op_edit = CreateWindowEx(
                                        0,
                                        WC_EDIT,
                                        "?",
                                        WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
                                        cur_x_pos,
                                        cur_y_pos,
                                        CTRLS_VARIABLE_WIDTH-CTRLS_VARIABLE_WIDTH*0.2,
                                        LINE_HEIGHT,
                                        hWnd,
                                        CTRLS_VARIABLE_ID(min_ctrl_var_id+CTRL_TYPE_OPTIONAL_EDIT),
                                        hInstance,
                                        NULL
                                    );
                                    var_ctrl->op_combobox = CreateWindowEx(
                                        0,
                                        WC_COMBOBOX,
                                        "",
                                        WS_CHILD|WS_VISIBLE|WS_BORDER|CBS_HASSTRINGS|CBS_DROPDOWNLIST,
                                        cur_x_pos + CTRLS_VARIABLE_WIDTH-CTRLS_VARIABLE_WIDTH*0.2,
                                        cur_y_pos,
                                        CTRLS_VARIABLE_WIDTH*0.2,
                                        LINE_HEIGHT*3,
                                        hWnd,
                                        CTRLS_VARIABLE_ID(min_ctrl_var_id+CTRL_TYPE_OPTIONAL_CB),
                                        hInstance,
                                        NULL
                                    );
                                    switch (combobox_cursel)
                                    {
                                        case 3://L
                                            ComboBox_AddString(var_ctrl->op_combobox,"L");
                                            ComboBox_AddString(var_ctrl->op_combobox,"mL");
                                            Static_SetText(var_ctrl->static_error,"Ingrese el numero de Litros.       ");
                                            break;
                                        case 4://kg
                                            ComboBox_AddString(var_ctrl->op_combobox,"Kg");
                                            ComboBox_AddString(var_ctrl->op_combobox,"g");
                                            Static_SetText(var_ctrl->static_error,"Ingrese el numero de Kg.           ");
                                            break;
                                    }
                                    ComboBox_SetCurSel(var_ctrl->op_combobox,0);
                                    var_ctrl->is_ready = false;
                                }
                            }
                            else
                            {
                                HWND hedit = (HWND)lParam;

                                char buffer[75];
                                num_t num;

                                if(Edit_GetTextLength(hedit) < 75)
                                {
                                    Edit_GetText(hedit,buffer,75);
                                    char * ptrtoend;
                                    num = strtod(buffer,&ptrtoend);
                                    if(var_ctrl->op_edit != NULL)
                                    {
                                        Edit_GetText(var_ctrl->op_edit,buffer,75);
                                        if(buffer[0] == '?' && buffer[1] == '\0')
                                        {
                                            var_ctrl->is_ready = false;
                                            Static_SetText(
                                                var_ctrl->static_error,
                                                "No puede ser desconocido."
                                            );
                                            break;
                                        }
                                    }
                                    if(*ptrtoend != '\0')
                                    {
                                        if(buffer[0] == '?' && buffer[1] == '\0')
                                        {
                                            if(var_ctrl->op_edit == NULL)
                                            {
                                                var_ctrl->is_ready = true;
                                                Static_SetText(var_ctrl->static_error,"Desconocido.                             ");
                                            }
                                            else
                                            {
                                                var_ctrl->is_ready = false;
                                                Static_SetText(var_ctrl->static_error,"No puede ser desconocido.");
                                            }
                                        }
                                        else
                                        {
                                            var_ctrl->is_ready = false;
                                            Static_SetText(var_ctrl->static_error,"Cadena no valida.                      ");
                                        }
                                    }
                                    else if(num == 0 || num < 0)
                                    {
                                        var_ctrl->is_ready = false;
                                        Static_SetText(var_ctrl->static_error,"Numero no valido.                     ");
                                    }
                                    else
                                    {
                                        Static_SetText(var_ctrl->static_error,"Valido.                                        ");
                                        var_ctrl->is_ready = true;
                                    }
                                }
                                else
                                {
                                    var_ctrl->is_ready = false;
                                    Static_SetText(var_ctrl->static_error,"Cadena muy grande.                 ");
                                }
                            }
                        }
                    }
                    break;

                //handles the clicks of the main button
                case (intptr_t)BUTTON_GETOUTPUT_ID:
                    if(HIWORD(wParam) == BN_CLICKED)
                    {
                        if(is_reaction_ready == reaction_ready)
                        {
                            char vars_reactants = get_all_vars_from_ctrls(&var_arr_ctrls_reactants);
                            char vars_products = get_all_vars_from_ctrls(&var_arr_ctrls_products);
                            if(vars_reactants + vars_products == 2 || vars_reactants + vars_products == -1)
                            {
                                if(send_new_input(&react,&var_arr_ctrls_reactants.var_arr,&var_arr_ctrls_products.var_arr) != INPUT_NOERRROR)
                                {
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                        else if(is_reaction_ready == reaction_unbalanced)
                        {

                        }
                    }
                    break;

                //handles the input of the reaction
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

                            if(edit_products_str.ready)
                            {
                                if(is_reaction_ready == reaction_ready)
                                {
                                    destroy_reaction(&react);
                                    switch(init_balanced_reaction(edit_reactants_str.str,edit_products_str.str,&react))
                                    {
                                        case REACTION_ERR_INIT_UNBALANCED:
                                            is_reaction_ready = reaction_unbalanced;
                                            Static_SetText(hstatic_error,"Reaccion no balanceada.");
                                            break;
                                        case REACTION_ERR_INIT_UNKNOWN_SYMBOL:
                                            is_reaction_ready = reaction_nonready;
                                            Static_SetText(hstatic_error,"Reactivos incorrectos.");
                                            break;
                                    }
                                }
                                else
                                {
                                    switch(init_balanced_reaction(edit_reactants_str.str,edit_products_str.str,&react))
                                    {
                                        case REACTION_ERR_INIT_UNBALANCED:
                                            is_reaction_ready = reaction_unbalanced;
                                            Static_SetText(hstatic_error,"Reaccion no balanceada.");
                                            break;
                                        case REACTION_ERR_INIT_UNKNOWN_SYMBOL:
                                            is_reaction_ready = reaction_nonready;
                                            Static_SetText(hstatic_error,"Reaccion incorrecta.");
                                            break;
                                        case REACTION_ERR_INIT_SUCCESS:
                                            is_reaction_ready = reaction_ready;
                                            break;
                                    }
                                }
                                if(is_reaction_ready == reaction_ready)
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

                            if(edit_reactants_str.ready)
                            {
                                if(is_reaction_ready == reaction_ready)
                                {
                                    destroy_reaction(&react);
                                    switch(init_balanced_reaction(edit_reactants_str.str,edit_products_str.str,&react))
                                    {
                                        case REACTION_ERR_INIT_UNBALANCED:
                                            is_reaction_ready = reaction_unbalanced;
                                            Static_SetText(hstatic_error,"Reaccion no balanceada.");
                                            break;
                                        case REACTION_ERR_INIT_UNKNOWN_SYMBOL:
                                            is_reaction_ready = reaction_nonready;
                                            Static_SetText(hstatic_error,"Productos incorrectos.");
                                            break;
                                    }
                                }
                                else
                                {
                                    switch(init_balanced_reaction(edit_reactants_str.str,edit_products_str.str,&react))
                                    {
                                        case REACTION_ERR_INIT_UNBALANCED:
                                            is_reaction_ready = reaction_unbalanced;
                                            Static_SetText(hstatic_error,"Reaccion no balanceada.");
                                            break;
                                        case REACTION_ERR_INIT_UNKNOWN_SYMBOL:
                                            is_reaction_ready = reaction_nonready;
                                            Static_SetText(hstatic_error,"Reaccion incorrecta.");
                                            break;
                                        case REACTION_ERR_INIT_SUCCESS:
                                            is_reaction_ready = reaction_ready;
                                            break;
                                    }
                                }
                                if(is_reaction_ready == reaction_ready)
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
                //handles the input of the reaction
            }
        }
        break;
    case WM_CREATE://inits the main window
        {
            sem_t * app_loop_sem = ((CREATESTRUCT*)lParam)->lpCreateParams;

            hInstance = ((CREATESTRUCT*)lParam)->hInstance;

            hedit_reactants = CreateWindowEx
                (
                    0,
                    WC_EDIT,
                    "",
                    WS_CHILD|WS_VISIBLE|WS_BORDER|ES_CENTER,
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
                    WS_CHILD|WS_VISIBLE|WS_BORDER|ES_CENTER,
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
    case WM_DESTROY://free resources and the quit
        {
            if(is_reaction_ready == reaction_ready)
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
            send_endwnd_signal();//we are done here
            PostQuitMessage(0);//bye bye
        }
    default:
        return DefWindowProc(hWnd,Msg,wParam,lParam);//I don't know how to handle this, lets handle it by default
    }
    return 0;//everything is fine here
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
            "Mochi",
            WS_OVERLAPPEDWINDOW,
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
