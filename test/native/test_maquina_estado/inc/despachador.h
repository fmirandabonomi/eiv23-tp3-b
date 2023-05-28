#ifndef DESPACHADOR_H
#define DESPACHADOR_H
#include <me_pub/me_i_despachador_evento.h>
#include <stddef.h>

typedef struct Despachador Despachador;
Despachador * Despachador_new(size_t maxReceptores);
IDespachadorEvento * Despachador_asIDespachadorEvento(Despachador *self);
void Despachador_delete(Despachador *self);

#endif

