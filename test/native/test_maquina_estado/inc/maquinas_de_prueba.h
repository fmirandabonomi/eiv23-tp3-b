#ifndef MAQUINAS_DE_PRUEBA_H
#define MAQUINAS_DE_PRUEBA_H
#include <maquina_estado.h>

typedef struct Prueba1 Prueba1;
typedef struct Prueba2 Prueba2;


/**
 * @brief Crea máquina de prueba 1, consiste en un solo estado "A" que acepta tres eventos (T1,T2,T3)
  ```
    @startuml
    [*] --> A
    A : INCIALIZA/ACUSE(A,INICIALIZA)
    A : ENTRADA/ACUSE(A,ENTRADA)
    A : T1/ACUSE(A,T1)
    A : T2/ACUSE(A,T2)
    A : SALIDA/ACUSE(A,SALIDA)
    A --> A : T3/ACUSE(A,T3)
    @enduml
  ```
 * @return Prueba1* Maquina de prueba 1
 */
Prueba1 *Prueba1_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

Maquina *Prueba1_asMaquina(Prueba1 *self);

void Prueba1_delete(Prueba1 *self);

/**
 * @brief Crea máquina de prueba 2
  ```
    @startuml
    [*] --> A
    A : INCIALIZA/ACUSE(A,INICIALIZA)
    A : ENTRADA/ACUSE(A,ENTRADA)
    A : T1/ACUSE(A,T1)
    A : SALIDA/ACUSE(A,SALIDA)
    A --> A : T2/ACUSE(A,T2)
    A --> B : T3/ACUSE(A,T3)
    B : INCIALIZA/ACUSE(B,INICIALIZA)
    B : ENTRADA/ACUSE(B,ENTRADA)
    B : T4/ACUSE(B,T4)
    B : SALIDA/ACUSE(B,SALIDA)
    B --> B : T5/ACUSE(B,T5)
    B --> A : T6/ACUSE(B,T6)
    @enduml
  ```
 * @return Prueba2* Maquina de prueba 2
 */
Prueba2 *Prueba2_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

Maquina *Prueba2_asMaquina(Prueba2 *self);

void Prueba2_delete(Prueba2 *self);


#endif
