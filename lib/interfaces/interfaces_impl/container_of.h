#ifndef CONTAINER_OF_H
#define CONTAINER_OF_H
#include <stddef.h>

/**
 @brief Obtiene el puntero a una estructura en base al puntero a uno de sus miembros

 Ejemplo:

    #include <stdio.h>
    #include <assert.h>
    #include <interfaces_impl/container_of.h>
    typedef struct A{
        int a1;
        int a2;
        struct B{
            int b1;
        }a3;
    }A;
    int main(void){
        A mi_a = {0};
        int *pB1 = &mi_a.a3.b1;
        A *const p1 = container_of(pB1,A,a3.b1);
        assert(p1 == &mi_a);
        puts("Prueba pasada \n");
        return 0;
    }

*/
#define container_of(puntero,TipoContenedor,rutaMiembro) (__extension__({\
    __typeof__(((TipoContenedor*)0)->rutaMiembro) *const pMiembro = (puntero);\
    TipoContenedor *const pContenedor = ((void*)pMiembro) - offsetof(TipoContenedor,rutaMiembro);\
    pContenedor;}))

#endif
