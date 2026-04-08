#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

#define _MENU_PREV 8
#define _MENU_NEXT 9
#define _MENU_EXIT 0

const int _MAX_MENU_ITEMS = 7;

// MenuData almacena cada elemento del menu el cual contiene el texto a mostrar y el puntero de la funcion que se llama al seleccionarlo.
typedef struct 
{
    char text[_MAX_MENU_TEXT_LEN];
    FuncPtr func;
} MenuData;

// Estructura principal la cual contiene todo lo necesario para mostrar el menu.
// id contiene el identificador del menu (si contiene _MENU_NON_VALID_ID significa que no hay datos guardados y se puede utilizar para un menu nuevo).
// name almacena el titulo del menu.
// data almacena los elementos a mostrar en el menu.
// valid_data cuenta cuantos elementos hay en data.
typedef struct 
{
    int id;
    char name[_MAX_MENU_NAME_LEN];
    MenuData data[_MAX_MENU_DATA];
    int valid_data;
} Menu;

// Almacena toda la informacion de todos los menu.
Menu _main_menu[_MAX_MENU];

// Se utiliza para asignar id a los menu de forma incremental para que cada identificador sea unico.
int _main_menu_ids = 0;

/*------------------------------ FUNCIONES INTERNAS ------------------------------*/

int _menu_create(int menu_index, int page);

// Inicializa _main_menu, dado que tiene __attribute__((constructor)) esta funcion se llama automaticamente antes que main.
void _menu_init(void) __attribute__((constructor));

// Busca y devuelve la posicion de la celda que contiene identificador en _main_menu.
int _get_menu_index(int menu_id)
{
    int menu_index = _MENU_NON_VALID_ID;

    for(int i = 0; i < _MAX_MENU; i++)
    {
        if(_main_menu[i].id == menu_id)
        {
            menu_index = i;

            break;
        }
    }

    return menu_index;
}

// Agrega el boton de Volver (_MENU_PREV_TEXT) en caso de que la pagina sea mayor a 1.
void _menu_add_prev(int page)
{
    if(page <= 1)
    {
        return;
    }

    printf("%d. %s\n", _MENU_PREV, _MENU_PREV_TEXT);
}

// Agrega el boton de Siguiente (_MENU_NEXT_TEXT) en caso de que existan elementos en la siguiente pagina.
// valid_data contiene la cantidad de elementos que existen en el menu.
// _MAX_MENU_ITEMS * page devuelve el maximo de elementos posibles dada la pagina actual.
void _menu_add_next(int valid_data, int page)
{
    if((valid_data - _MAX_MENU_ITEMS * page) < 1)
    {
        return;
    }

    printf("%d. %s\n", _MENU_NEXT, _MENU_NEXT_TEXT);
}

// Agrega el boton para salir del menu.
void _menu_add_exit(void)
{
    printf("\n%d. %s\n\n", _MENU_EXIT, _MENU_EXIT_TEXT);
}

// Devuelve la cantidad de elementos que se muestran en la pagina actual.
// Se calcula obtieniendo la cantidad total de paginas que existen en el menu (en base a _MAX_MENU_ITEMS) menos la pagina actual, 
//     de ser mayor a 0 significa que hay mas paginas para mostrar, entonces hay _MAX_MENU_ITEMS en la pagina actual,
//     en cambio, si no es mayor a 0 significa que es la ultima pagina y hay _MAX_MENU_ITEMS o menos.
int _get_options_in_page(int valid_data, int page)
{
    int total_page = valid_data / _MAX_MENU_ITEMS + 1;
    int last_page_options = valid_data % _MAX_MENU_ITEMS;

    return total_page - page > 0 ? _MAX_MENU_ITEMS : last_page_options;
}

// Obtiene un caracter ingresado por el usuario y verifica que cumpla con las siguientes condiciones para validarlo:
// Esta en el rango de 0-9 y
//     El numero ingresado es menor o igual a la cantidad de elementos del menu que existen en la pagina actual (_get_options_in_page)
//         (En caso de que el usuario aprieta el boton de Salir (_MENU_EXIT_TEXT) entra en esta condicion)
//     o la pagina actual no es la primera y el numero ingresado es _MENU_PREV
//     o hay mas paginas y el numero ingresado es _MENU_PREV.
// Devuelve el numero ingresado despues de validar que entra en las condiciones previamente mencionadas.
int _get_menu_valid_option(int menu_index, int page)
{
    char option;

    printf("Elegir opcion: ");

    fflush(stdin);
    if(scanf(" %c", &option) == 1 || option >= 48 || option <= 57)
    {
        int i_option = option % 48;
        
        if(i_option <= _get_options_in_page(_main_menu[menu_index].valid_data, page) || 
            (page > 1 && i_option == _MENU_PREV) || 
            (((_main_menu[menu_index].valid_data - _MAX_MENU_ITEMS * page) >= 1) && i_option == _MENU_NEXT))
        {
            return i_option;
        }
    }

    printf("[MENU] Opcion incorrecta.\n");

    return _get_menu_valid_option(menu_index, page);
}

// Se encarga de:
//     obtener un numero que cumpla con las condiciones en _get_menu_valid_option,
//     crear un nuevo menu en la pagina anterior
//     o crear un nuevo menu en la pagina siguiente
//     o salir del menu
//     o llamar la funcion correspondiente al elemento del menu seleccionado
//         (vuelve a mostrar el menu despues de que se ejecuto la funcion).
// Devuelve la pagina que estaba mostrando al Salir (_MENU_EXIT_TEXT) del menu.
int _menu_handler(int menu_index, int page)
{
    int option = _get_menu_valid_option(menu_index, page);
    
    switch(option)
    {
        case _MENU_PREV: return _menu_create(menu_index, page - 1);
        case _MENU_NEXT: return _menu_create(menu_index, page + 1);
        case _MENU_EXIT: return page;
        default: _main_menu[menu_index].data[option + (_MAX_MENU_ITEMS * (page - 1)) - 1].func();
    }

    #ifdef MENU_AUTO_PAUSE
    system("pause");
    #endif

    return _menu_create(menu_index, page);
}

// Se encarga de:
//     Mostrar el titulo del menu,
//     mostrar elementos del menu segun la pagina,
//     mostrar o no la opcion Volver (_MENU_PREV_TEXT)
//     mostrar o no la opcion Siguiente (_MENU_NEXT_TEXT)
//     mostrar la opcion Salir (_MENU_EXIT_TEXT)
//     llamar a _menu_handler para obtener y ejecutar la opcion del usuario.
// Devuelve la pagina que se estaba mostrando cuando el usuario eligio la opcion Salir (_MENU_EXIT_TEXT).
int _menu_create(int menu_index, int page)
{
    #ifdef MENU_AUTO_CLEAN
    system("cls");
    #endif
    
    printf("\t%s\n\n", _main_menu[menu_index].name);

    // Muestra los elementos del menu.
    //     i inicia en 0 o el primer elemento de la pagina correspondiente.
    //     i es siempre menor a la cantidad de elementos del menu y
    //         es el primer elemento del menu
    //         o esta en el rango de elementos posibles a mostrar.
    for(int i = 0 + _MAX_MENU_ITEMS * (page - 1); i < _main_menu[menu_index].valid_data && (i == 0 + _MAX_MENU_ITEMS * (page - 1) || (i % _MAX_MENU_ITEMS) != 0); i++)
    {
        printf("%d. %s\n", (i % _MAX_MENU_ITEMS) + 1, _main_menu[menu_index].data[i].text);
    }

    printf("\n");

    _menu_add_prev(page);
    _menu_add_next(_main_menu[menu_index].valid_data, page);
    _menu_add_exit();

    return _menu_handler(menu_index, page);
}

// Inicializa _main_menu asignando a todas las celdas el identificador _MENU_NON_VALID_ID 
//     el cual sirve para saber si la celda esta disponible para usarse
//     o esta ya fue asignada y puede contener elementos.
void _menu_init(void)
{
    for(int i = 0; i < _MAX_MENU; i++)
    {
        _main_menu[i].id = _MENU_NON_VALID_ID;
        _main_menu[i].valid_data = 0;
    }
}

/*------------------------------ FUNCIONES EXTERNAS ------------------------------*/

// Registra un nuevo menu:
//     Busca que exista una celda en _main_menu cuyo id sea _MENU_NON_VALID_ID.
//     Le asigna un identificador usando _main_menu_id.
//     Guarda el nombre.
// Devuelve el identificador asignado al menu o _MENU_NON_VALID_ID en caso de que no exista
//     una celda disponible.
int menu_register(char *name)
{
    int id = _MENU_NON_VALID_ID;

    for(int i = 0; i < _MAX_MENU; i++)
    {
        if(_main_menu[i].id == -1)
        {
            id = _main_menu_ids++;

            _main_menu[i].id = id;
            strcpy(_main_menu[i].name, name);

            break;
        }
    }

    return id;
}

// Agrega un elemento al menu:
//     Obtiene la ubicacion del identificador en _main_menu.
//     Copia el texto del elemento para mostrar en el menu.
//     Guarda el puntero de la funcion a llamar cuando se seleccione el elemento.
//     Incrementa la cantidad de informacion valida almacenada.
void menu_add(int menu_id, char *text, FuncPtr func)
{
    int menu_index = _get_menu_index(menu_id);

    if(menu_index == _MENU_NON_VALID_ID)
    {
        printf("[MENU] EL IDENTIFICADOR NO EXISTE\n\n");

        return;
    }

    strcpy(_main_menu[menu_index].data[_main_menu[menu_index].valid_data].text, text);
    _main_menu[menu_index].data[_main_menu[menu_index].valid_data].func = func;

    _main_menu[menu_index].valid_data++;
}

// Muesta el menu en la pagina correspondiente:
//     Obtiene la ubicacion del identificador en _main_menu.
//     Muestra el menu.
// Devuelve la ultima pagina que se mostro.
int menu_display(int menu_id, int page)
{
    int menu_index = _get_menu_index(menu_id);

    if(menu_index == _MENU_NON_VALID_ID)
    {
        printf("[MENU] EL IDENTIFICADOR NO EXISTE\n\n");

        return _MENU_NON_VALID_ID;
    }

    if(page < 1)
    {
        printf("[MENU] LAS PAGINAS EMPIEZAN EN 1 Y INGRESASTE %d\n\n", page);

        return _MENU_PAGE_ERROR;
    }

    if(_main_menu[menu_index].valid_data - _MAX_MENU_ITEMS * (page - 1) < 1)
    {
        printf("[MENU] NO SE PUEDE MOSTRAR LA PAGINA PORQUE NO HAY SUFICIENTES ELEMENTOS\n\n");

        return _MENU_PAGE_ERROR;
    }

    int last_page_used = _menu_create(menu_index, page);

    return last_page_used;
}

// Borra todos los datos del menu:
//     Obtiene la ubicacion del identificador en _main_menu.
//     Asigna _MENU_NON_VALID_ID a id dejandolo libre para que se pueda crear un nuevo menu.
//     Borra el nombre del menu.
//     Borra los elementos del menu.
//     Asigna 0 a los datos validos.
//     Asigna _MENU_NON_VALID_ID a menu_id.
void menu_destroy(int *menu_id)
{
    int menu_index = _get_menu_index(*menu_id);

    if(menu_index == _MENU_NON_VALID_ID)
    {
        printf("[MENU] EL IDENTIFICADOR NO EXISTE\n\n");

        return;
    }

    _main_menu[menu_index].id = _MENU_NON_VALID_ID;

    size_t name_len = strlen(_main_menu[menu_index].name);

    for(size_t i = 0; i < name_len; i++)
    {
        _main_menu[menu_index].name[i] = 0;
    }

    for(int i = 0; i < _main_menu[menu_index].valid_data; i++)
    {
        size_t text_len = strlen(_main_menu[menu_index].data[i].text);

        for(size_t z = 0; z < text_len; z++)
        {
            _main_menu[menu_index].data[i].text[z] = 0;
        }

        _main_menu[menu_index].data[i].func = NULL;
    }

    _main_menu[menu_index].valid_data = 0;

    *menu_id = _MENU_NON_VALID_ID;
}
