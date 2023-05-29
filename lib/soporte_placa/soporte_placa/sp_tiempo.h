#ifndef SP_TIEMPO
#define SP_TIEMPO
#include <stdint.h> // uint32_t
#include <stdbool.h> // bool
#include <i_accion.h>

/**
 * @brief Rutina de servicio de interrupción de timer SysTick
 * 
 */
void SysTick_Handler(void);

/**
 * @brief Retardo con bloqueo durante un tiempo dado en milisegundos
 * @note Llamar a SP_init antes de usar.
 * @param tiempo Tiempo en milisegundos
 */
void SP_Tiempo_delay(uint32_t tiempo);

/**
 * @brief Programa un evento de timeout. Al cumplirse el tiempo 
 * especificado la rutina de servicio de interrupción de SysTick 
 * hará ejecutará la accuón especificada (la ejecución OCURRE EN
 * MODO HANDLER). Cada acción solo puede tener un timeout, sucesivos
 * llamados con la misma acción modifican su timeout y no generan
 * timeout adicionales
 * 
 * @param tiempo Tiempo en milisegundos a partir del cual hacer
 * el llamado. Mayor que cero.
 * @param handler Implementación de la interfaz IAccion, accion
 * ejecutada al cumplirse el tiempo. Nota: la acción es ejecutada
 * en modo HANDLER
 * @return true Evento programado
 * @return false Falló la programación del evento (posiblemente por
 * falta de recursos)
 */
bool SP_Tiempo_addTimeout(uint32_t tiempo,IAccion * accion);

void SP_Tiempo_init(void);

/**
 * @brief Obtiene el valor actual del contador de milisegundos
 * 
 * @return uint32_t Valor actual del contador de milisegundos
 */
uint32_t SP_Tiempo_getMilisegundos(void);
#endif

