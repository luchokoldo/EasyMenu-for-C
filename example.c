#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);
void test6(void);
void test7(void);
void test8(void);
void test9(void);
void test10(void);
void test11(void);
void test12(void);
void test13(void);
void test14(void);
void test15(void);
void test16(void);
void test16_config(int menu_id);

int main()
{
    int menu = menu_register("Test");

    menu_add(menu, "Prueba 1", test1);
    menu_add(menu, "Prueba 2", test2);
    menu_add(menu, "Prueba 3", test3);
    menu_add(menu, "Prueba 4", test4);
    menu_add(menu, "Prueba 5", test5);
    menu_add(menu, "Prueba 6", test6);
    menu_add(menu, "Prueba 7", test7);
    menu_add(menu, "Prueba 8", test8);
    menu_add(menu, "Prueba 9", test9);
    menu_add(menu, "Prueba 10", test10);
    menu_add(menu, "Prueba 11", test11);
    menu_add(menu, "Prueba 12", test12);
    menu_add(menu, "Prueba 13", test13);
    menu_add(menu, "Prueba 14", test14);
    menu_add(menu, "Prueba 15", test15);

    test16_config(menu);

    int last_page = menu_display(menu, 1);

    printf("Pagina cuando se apreto 0 (salir) = %d\n", last_page);

    menu_destroy(&menu);

    printf("menu = %d\n", menu);

    return 0;
}

void test1(void)
{
    printf("Call Test1\n");
}

void test2(void)
{
    printf("Call Test2\n");
}

void test3(void)
{
    printf("Call Test3\n");
}

void test4(void)
{
    printf("Call Test4\n");
}

void test5(void)
{
    printf("Call Test5\n");
}

void test6(void)
{
    printf("Call Test6\n");
}

void test7(void)
{
    printf("Call Test7\n");
}

void test8(void)
{
    printf("Call Test8\n");
}

void test9(void)
{
    printf("Call Test9\n");
}

void test10(void)
{
    printf("Call Test10\n");
}

void test11(void)
{
    printf("Call Test11\n");
}

void test12(void)
{
    printf("Call Test12\n");
}

void test13(void)
{
    printf("Call Test13\n");
}

void test14(void)
{
    printf("Call Test14\n");
}

void test15(void)
{
    printf("Call Test15\n");
}

void test16(void)
{
    printf("Call Test16\n");
}
void test16_config(int menu_id)
{
    char buffer[128];

    printf("Ingresar texto: ");

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        size_t len = strlen(buffer);

        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        menu_add(menu_id, buffer, test16);
    }
}