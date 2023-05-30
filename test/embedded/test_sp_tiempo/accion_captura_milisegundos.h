#ifndef ACCION_CAPTURA_MILISEGUNDOS
#define ACCION_CAPTURA_MILISEGUNDOS

#include <i_accion.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct CapturaMilisegundos CapturaMilisegundos;

struct CapturaMilisegundos
{
    IAccion accion;
    uint32_t volatile captura;
    bool volatile capturado;
};

/**
 * @brief Inicializa un objeto CapturaMilisegundos con el valor de tiempo inicial
 * 
 * @param self Puntero a este objeto
 * @param valorInicial Valor de tiempo inicial a usar (una captura en este tiempo sería 0)
 */
void CapturaMilisegundos_init(CapturaMilisegundos *self, uint32_t valorInicial);
/**
 * @brief Obtiene una acción que al ejecutarse efectúa la captura del valor actual
 * del contador de milisegundos
 * 
 * @param self Puntero a este objeto
 * @return IAccion* Puntero a acción que efectúa la captura al ser ejecutada
 */
IAccion *CapturaMilisegundos_getAccionCaptura(CapturaMilisegundos *self);
/**
 * @brief Determina si ya fue ejecutada la acción de captura
 * 
 * @param self Puntero a este objeto
 * @return true Captura efecutada, valor disponible
 * @return false Captura no efectuada, valor lee 0
 */
bool CapturaMilisegundos_qCapturado(CapturaMilisegundos *self);
/**
 * @brief Obtiene el valor de milisegundos capturado
 * 
 * @param self Puntero a este objeto
 * @return uint32_t Valor capturado, 0 si no hubo captura aún
 */
uint32_t CapturaMilisegundos_getValor(CapturaMilisegundos *self);

#endif
