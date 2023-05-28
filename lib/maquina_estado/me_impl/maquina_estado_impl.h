#ifndef MAQUINA_ESTADO_IMPL_H
#define MAQUINA_ESTADO_IMPL_H
#include <maquina_estado.h>

struct Maquina{
    IReceptorEvento iReceptorEvento;
    ISoporteEstado iSoporteEstado;
    IColaEventos * colaEventos;
    IDespachadorEvento *despachador;
    IPilaEstados * pilaEstados;
    ITimer * timer;
    Estado * inicial;
    Estado * activo;
};

/**
 * @brief Inicializa una máquina de estado.
 * 
 * @param colaEventos Cola de eventos a utilizar
 * @param inicial Estado inicial (en el que inicia al recibir
 * un evento con el mensaje  @ref MSJ_INICIALIZA )
 * @return Maquina* Puntero a la nueva máquina a NULL si falló la
 * creación
 */
void Maquina_init(
    Maquina *self, 
    Estado *inicial, 
    IColaEventos *colaEventos, 
    IDespachadorEvento *despachador, 
    IPilaEstados *pilaEstados, ITimer *timer
);

/**
 * @brief Finaiza la operación de la máquina de estado, llamar
 * antes de liberar los recursos asociados
 * 
 * @param self Este objeto Maquina de estado
 */
void Maquina_finalize(Maquina *self);


#endif
