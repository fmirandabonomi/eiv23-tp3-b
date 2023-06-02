#ifndef MAQUINA_ESTADO_IMPL_H
#define MAQUINA_ESTADO_IMPL_H

#include <maquina_estado.h>

typedef enum CodResultado{
    RES_IGNORADO,
    RES_PROCESADO,
    RES_TRANSICION
}CodResultado;

struct Resultado{
    CodResultado codigo;
    Estado nuevoEstado; // Valido cuando resultado = RES_TRANSICION
};

/**
 * @brief Inicializa la máquina de estado
 * 
 * @param self Memoria reservada para este objeto
 */
void Maquina_init(Maquina *self, Estado estadoInicial);

#endif
