#ifndef PRUEBA1_H
#define PRUEBA1_H
#include <maquina_estado.h>

typedef struct Prueba1 Prueba1;

/**
 * @brief Crea máquina de prueba 1, consiste en un solo estado "A" que acepta tres eventos (T1,T2,T3)
 * 
 * @return Prueba1* Maquina de prueba 1
 */
Prueba1 *Prueba1_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

Maquina *Prueba1_asMaquina(Prueba1 *self);

void Prueba1_delete(Prueba1 *self);

#endif
