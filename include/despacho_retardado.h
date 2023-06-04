#ifndef DESPACHO_RETARDADO_H
#define DESPACHO_RETARDADO_H
#include <maquina_estado.h>
#include <stdint.h>
#include <stddef.h>

#ifndef MAX_DESPACHOS_RETARDADOS_ACTIVOS
#define MAX_DESPACHOS_RETARDADOS_ACTIVOS 4
#endif

/**
 * @brief Máquina de estado de control que maneja
 * una lista de despacho retardado. Permite programar
 * el envío de un evento luego de un retardo dado
 * 
 */
typedef struct DespachoRetardado{
    uint32_t t0;
    struct DespachoEnEspera{
        Maquina *destino;
        Evento evento;
        uint32_t cuenta;
    }despachosEnEspera[MAX_DESPACHOS_RETARDADOS_ACTIVOS];
    size_t numDespachosEnEspera;
}DespachoRetardado;

/**
 * @brief Inicializa una instancia de controlador de despacho
 * retardado.
 * 
 * @param self 
 */
void DespachoRetardado_init(DespachoRetardado *self);

/**
 * @brief Programa el despacho retardado de un evento luego
 * de transcurrido el tiempo indicado.
 * Si los recursos están agotados despacha el evento
 * inmediatamente. Esta función no bloquea.
 * 
 * @param self Este objeto
 * @param destino Destinatario del evento
 * @param evento Evento
 * @param tiempoMilisegundos Tiempo tras el cual efectuar el
 * despacho 
 */
void DespachoRetardado_programa(DespachoRetardado *self, Maquina *const destino, Evento const evento, uint32_t const tiempoMilisegundos);

/**
 * @brief Actualiza el estado de los despachos retardados en
 * curso. Despacha los eventos cuyo tiempo ha caducado.
 * Esta función no bloquea.
 * 
 * @param self Este objeto
 */
void DespachoRetardado_procesa(DespachoRetardado *self);

#endif
