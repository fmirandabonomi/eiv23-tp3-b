#ifndef MAQUINA_ESTADO_H
#define MAQUINA_ESTADO_H
#include <me_pub/me_i_cola_eventos.h>
#include <me_pub/me_i_receptor_evento.h>
#include <me_pub/me_estado.h>
#include <me_pub/me_evento.h>
#include <me_pub/me_i_despachador_evento.h>
#include <me_pub/me_i_pila_estados.h>
#include <stdint.h>
#include <stdbool.h>



/**
 * @brief Objeto máquina de estado. Mantiene un estado, acepta
 * y emite eventos.
 * 
 */
typedef struct Maquina Maquina;

/**
 * @brief Despacha un nuevo evento en forma asincrónica
 * 
 * @param self Este objeto Máquina de estado
 * @param e Evento (por valor)
 * @return true Evento despachado
 * @return false Falla al despachar (cola llena)
 */
bool Maquina_dispatch(Maquina *self,Evento e);

/**
 * @brief Procesa el primer evento pendiente
 * 
 * @param self Este objeto Maquina de estado
 * @return ResultadoEvento Resultado del procesamiento
 */
ResultadoEvento Maquina_procesa(Maquina *self);

/**
 * @brief Añade un receptor a la lista de receptores de eventos
 * generados por esta máquina de estado.
 * 
 * @param self Este objeto Maquina de estado
 * @param rx Receptor de evento
 * @return true Receptor añadido
 * @return false No pudo añadirse el receptor
 */
bool Maquina_addReceptorEvento(Maquina *self, IReceptorEvento *rx);

/**
 * @brief Quita un receptor de la lista de receptores de eventos
 * generados por esta máquina de estado.
 * 
 * @param self Este objeto Maquina de estado
 * @param rx Receptor de evento
 * @return true Receptor quitado
 * @return false Receptor no estaba en la lista
 */
bool Maquina_remReceptorEvento(Maquina *self, IReceptorEvento *rx);

/**
 * @brief Presenta la interfaz abstracta que representa esta máquina
 * de estado en su rol de receptor de eventos
 * 
 * @param self Este objeto Maquina de estado
 * @return IReceptorEvento* Este objeto representado como receptor de
 * eventos
 */
IReceptorEvento *Maquina_asIReceptorEvento(Maquina *self);


#endif
