#ifndef I_SOPORTE_CONTROLADOR
#define I_SOPORTE_CONTROLADOR
#include <maquina_estado.h>
#include <stdint.h>

typedef struct SoporteControlador SoporteControlador;

/**
 * @brief Asocia este objeto con la máquina de estado de un controlador 
 * específico, llamar como parte de la inicialización del controlador.
 * 
 * @param self Este objeto
 * @param controlador El controlador a asociar
 */
void SoporteControlador_asociaControlador(SoporteControlador *self, Maquina* controlador);

/**
 * @brief Enciende la luz
 * 
 * @param self Este objeto 
 */
void SoporteControlador_luzOn(SoporteControlador *self);
/**
 * @brief Apaga la luz
 * 
 * @param self Este objeto 
 */
void SoporteControlador_luzOff(SoporteControlador *self);

/**
 * @brief Envía un evento luego de transcurrido el tiempo indicado. Para
 * usar esta funcionalidad la máquina de estado debe haber sido asociada
 * anteriormente a este objeto. Si se repite el llamado con el mismo
 * evento se reemplaza el timeout. Si se llama con eventos distintos 
 * programa diferentes timeouts.
 * 
 * @param self Este objeto
 * @param tiempo Tiempo en milisegundos tras el cual enviar el evento
 * @param evento Evento a enviar
 * @return true Timeout programado
 * @return false No pudo programarse el timeout
 */
bool SoporteControlador_setTimeout(SoporteControlador *self, uint32_t tiempo, Evento evento);


#endif