#ifndef CONTAINER_OF_H
#define CONTAINER_OF_H
#include <stddef.h>
/**
 * @brief Obtiene un puntero a una estructura en base al puntero a uno de sus miembros.
 * 
 * Idea tomada del kernel de linux (linux/container_of)
 * Usa extensiones de C propias de GCC: 
 * 
 * - Expresiones con bloque de código ({})
 * - __typeof__(<expresión>) -> tipo de <expresión>
 */
#define container_of(ptr, type, member) (__extension__({\
    __typeof__(((type*)0)->member) *const ptr_miembro = (ptr);\
    (type*)(((void*)ptr_miembro) - offsetof(type,member)); }))

#endif
