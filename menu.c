#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

#define _MENU_PREV 8
#define _MENU_NEXT 9
#define _MENU_EXIT 0

const int _MAX_MENU_ITEMS = 7;

typedef struct 
{
    char *text;
    FuncPtr func;
} MenuData;

typedef struct 
{
    int id;
    char *name;
    MenuData data[_MAX_MENU_DATA];
    int valid_data;
} Menu;

Menu _main_menu[_MAX_MENU];
int _main_menu_ids = 0;

/*------------------------------ FUNCIONES INTERNAS ------------------------------*/

int _menu_create(int menu_index, int page);
void _menu_init(void) __attribute__((constructor));

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

void _menu_add_prev(int page)
{
    if(page <= 1)
    {
        return;
    }

    printf("%d. Volver.\n", _MENU_PREV);
}

void _menu_add_next(int valid_data, int page)
{
    if((valid_data - _MAX_MENU_ITEMS * page) < 1)
    {
        return;
    }

    printf("%d. Siguiente.\n", _MENU_NEXT);
}

void _menu_add_exit(void)
{
    printf("\n%d. Salir.\n\n", _MENU_EXIT);
}

int _get_options_in_page(int valid_data, int page)
{
    int total_page = valid_data / _MAX_MENU_ITEMS + 1;
    int last_page_options = valid_data % _MAX_MENU_ITEMS;

    return total_page - page > 0 ? _MAX_MENU_ITEMS : last_page_options;
}

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

int _menu_create(int menu_index, int page)
{
    system("cls");
    
    printf("\t%s\n\n", _main_menu[menu_index].name);
    
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

void _menu_init(void)
{
    for(int i = 0; i < _MAX_MENU; i++)
    {
        _main_menu[i].id = -1;
        _main_menu[i].valid_data = 0;
    }
}

/*------------------------------ FUNCIONES EXTERNAS ------------------------------*/

int menu_register(char *name)
{
    int id = 0;

    for(int i = 0; i < _MAX_MENU; i++)
    {
        if(_main_menu[i].id == -1)
        {
            id = _main_menu_ids++;

            _main_menu[i].id = id;
            _main_menu[i].name = name;
            _main_menu[i].valid_data = 0;

            break;
        }
    }

    return id;
}

void menu_add(int menu_id, char *text, FuncPtr func)
{
    int menu_index = _get_menu_index(menu_id);

    if(menu_index == _MENU_NON_VALID_ID)
    {
        printf("[MENU] EL IDENTIFICADOR NO EXISTE\n\n");

        return;
    }

    _main_menu[menu_index].data[_main_menu[menu_index].valid_data].text = text;
    _main_menu[menu_index].data[_main_menu[menu_index].valid_data].func = func;

    _main_menu[menu_index].valid_data++;
}

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

void menu_destroy(int *menu_id)
{
    int menu_index = _get_menu_index(*menu_id);

    if(menu_index == _MENU_NON_VALID_ID)
    {
        printf("[MENU] EL IDENTIFICADOR NO EXISTE\n\n");

        return;
    }

    _main_menu[menu_index].id = _MENU_NON_VALID_ID;
    _main_menu[menu_index].name = NULL;

    for(int i = 0; i < _main_menu[menu_index].valid_data; i++)
    {
        _main_menu[menu_index].data[i].text = NULL;
        _main_menu[menu_index].data[i].func = NULL;
    }

    _main_menu[menu_index].valid_data = 0;

    *menu_id = _MENU_NON_VALID_ID;
}