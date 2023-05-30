#ifndef ACCION_CONTADOR_H
#define ACCION_CONTADOR_H

#include <i_accion.h>
#include <stdint.h>

typedef struct Contador{
    IAccion accion;
    uint32_t volatile cuenta;
}Contador; 


/**
 * @brief Inicializa contador y establece la cuenta en cero
 * 
 * @param self Este contador
 */
void Contador_init(Contador *self);
/**
 * @brief Obtiene la acción de incremento de cuenta para este contador
 * 
 * @param self Este contador
 * @return IAccion* Acción de incremento de cuenta
 */
IAccion * Contador_getAccionIncrementaCuenta(Contador *self);
/**
 * @brief Obtiene la cuenta actual
 * 
 * @param self Este contador
 * @return uint32_t Cuenta actual
 */
uint32_t Contador_getCuenta(Contador *self);
#endif
