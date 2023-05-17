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

typedef void (*SP_TimeoutHandler)(void *param);

/**
 * @brief Llama a la función handler desde la rutina de servicio
 * de interrupción de SysTick luego de pasado el tiempo en milisegundos,
 * con el parámetro especificado.
 * 
 * @param tiempo Tiempo en milisegundos
 * @param handler Puntero a función handler
 * @param param Parámetro a pasar como argumento de la función handler (puntero void)
 * @return true Timeout programado
 * @return false Recursos insuficientes
 */
bool SP_Tiempo_registraTimeout(uint32_t tiempo,SP_TimeoutHandler handler,void *param);

void SP_Tiempo_init(void);

/**
 * @brief Obtiene el valor actual del contador de milisegundos
 * 
 * @return uint32_t Valor actual del contador de milisegundos
 */
uint32_t SP_Tiempo_getMilisegundos(void);
#endif

