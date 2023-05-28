#ifndef PILA_ESTADOS_H
#define PILA_ESTADOS_H
#include <me_pub/me_i_pila_estados.h>
#include <stddef.h>

typedef struct PilaEstados PilaEstados;

PilaEstados * PilaEstados_new(size_t maxEstados);
IPilaEstados * PilaEstados_asIPilaEstados(PilaEstados *self);
size_t PilaEstados_getNivelesUsados(PilaEstados *self);
void PilaEstados_delete(PilaEstados *self);

#endif
