#ifndef COLA_EVENTOS_H
#define COLA_EVENTOS_H
#include <stddef.h>
#include <me_pub/me_i_cola_eventos.h>
#include <me_pub/me_i_receptor_evento.h>

typedef struct ColaEventos ColaEventos;
ColaEventos * ColaEventos_new(size_t maxEventos);
IColaEventos * ColaEventos_asIColaEventos(ColaEventos *self);
IReceptorEvento * ColaEventos_asIReceptorEvento(ColaEventos *self);
size_t ColaEventos_numEventos(ColaEventos *self);
void ColaEventos_clear(ColaEventos *self);
size_t ColaEventos_getEventos(ColaEventos *self,size_t maxEventos,Evento *eventos);
void ColaEventos_delete(ColaEventos *self);

#endif
