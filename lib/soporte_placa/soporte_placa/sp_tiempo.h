#ifndef SP_TIEMPO
#define SP_TIEMPO
#include <stdint.h> // uint32_t
#include <stdbool.h> // bool

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

typedef void (*SP_TimeoutHandler)(void volatile *param);

/**
 * @brief Programa un evento de timeout. Al cumplirse el tiempo 
 * especificado la rutina de servicio de interrupción de SysTick 
 * hará un llamado al handler especificado (EL LLAMADO OCURRE EN
 * MODO HANDLER)
 * 
 * @param tiempo Tiempo en milisegundos a partir del cual hacer
 * el llamado. Mayor que cero.
 * @param handler Puntero a función handler, que no retorna valor
 * y recibe un parámetro puntero sin tipo (void *)
 * @param param Parámetro puntero sin tipo que será utilizado en
 * el llamado al handler
 * @return true Evento programado
 * @return false Falló la programación del evento (posiblemente por
 * falta de recursos)
 */
bool SP_Tiempo_addTimeout(uint32_t tiempo,SP_TimeoutHandler handler,void volatile *param);

void SP_Tiempo_init(void);

/**
 * @brief Obtiene el valor actual del contador de milisegundos
 * 
 * @return uint32_t Valor actual del contador de milisegundos
 */
uint32_t SP_Tiempo_getMilisegundos(void);
#endif

