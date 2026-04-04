#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

//Comentar la linea "#define MENU_AUTO_PAUSE" para evitar que se pause el programa despues de ejecutar la funcion de la opcion seleccionada en el menu.
#define MENU_AUTO_PAUSE

//Comentar la linea "#define MENU_AUTO_CLEAN" para evitar que se limpie la pantalla antes de mostrar el menu.
#define MENU_AUTO_CLEAN

//Aumantar el valor en caso de que necesitas almacenar mas menu.
#define _MAX_MENU 64

//Aumantar el valor en caso de que necesitas almacenar mas elementos en el menu.
#define _MAX_MENU_DATA 64

//Aumantar el valor en caso de que necesitas almacenar mas caracteres en el nombre del menu.
#define _MAX_MENU_NAME_LEN 128

//Aumantar el valor en caso de que necesitas almacenar mas caracteres en el texto de los elementos del menu.
#define _MAX_MENU_TEXT_LEN 128

//Texto de la opcion para volver a la pagina anterior.
#define _MENU_PREV_TEXT "Volver"

//Texto de la opcion para ir a la pagina siguiente.
#define _MENU_NEXT_TEXT "Siguiente"

//Texto de la opcion para ir salir.
#define _MENU_EXIT_TEXT "Salir"

/********************* NO MODIFICAR *********************/

#define _MENU_NON_VALID_ID -1
#define _MENU_PAGE_ERROR -2

typedef void (*FuncPtr)(void);

/**
 * @brief Registra el menu.
 * @param name nombre del menu.
 * @return el id del menu.
 */
int menu_register(char *name);

/**
 * @brief Agrega una elemento al menu.
 * @param menu_id identificador del menu.
 * @param text texto a mostrar.
 * @param func funcion (void) que se llama cuando se elige el elemento.
 */
void menu_add(int menu_id, char *text, FuncPtr func);

/**
 * @brief Muestra el menu.
 * @param menu_id identificador del menu.
 * @param page pagina a mostrar (empieza en 1).
 * @return ultima pagina mostrada.
 */
int menu_display(int menu_id, int page);

/**
 * @brief Borra los datos del menu.
 * @param menu_id identificador del menu.
 */
void menu_destroy(int *menu_id);

/********************* NO MODIFICAR *********************/

#endif