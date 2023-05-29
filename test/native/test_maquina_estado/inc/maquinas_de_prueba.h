#ifndef MAQUINAS_DE_PRUEBA_H
#define MAQUINAS_DE_PRUEBA_H
#include <maquina_estado.h>


typedef Maquina * (*MaquinaNew)(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

/**
  @brief Para ejercitar las transiciones en una máquina de un solo estado
  @startuml
  [*] --> A
  A : INCIALIZA/ACUSE(A,INICIALIZA)
  A : ENTRADA/ACUSE(A,ENTRADA)
  A : T1/ACUSE(A,T1)
  A : T2/ACUSE(A,T2)
  A : SALIDA/ACUSE(A,SALIDA)
  A --> A : T3/ACUSE(A,T3)
  @enduml
  @param colaEventos
  @param despachador  
  @param pilaEstados 
  @param timer 
  @return Maquina* 
*/
Maquina *Prueba1_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

/**
  @brief Para ejercitar transiciones entre dos estados nivel 0
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
  @param colaEventos
  @param despachador  
  @param pilaEstados 
  @param timer 
  @return Maquina* 
*/
Maquina *Prueba2_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

/**
  @brief Para ejercitar las transiciones en una máquina con un estado anidado iniciando en el estado superior
  [*] --> A.B
  A{
    [*] --> B
    B : T2
    B --> B : T3
    B --> A : T4
    A --> B : T5
  } C
  A.B --> A : T6
  A --> A : T7
  A --> A.B : T8
  @param colaEventos 
  @param despachador 
  @param pilaEstados 
  @param timer 
  @return Maquina* 
*/
Maquina *Prueba3_new_a(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);
/**
  @brief Para ejercitar las transiciones en una máquina con un estado anidado iniciando en el estado interno  
  [*] --> A
  A : T1
  A{
    [*] --> B
    B : T2
    B --> B : T3
    B --> A : T4
    A --> B : T5
  } C
  A.B --> A : T6
  A --> A : T7
  A --> A.B : T8
  @param colaEventos 
  @param despachador 
  @param pilaEstados 
  @param timer 
  @return Maquina* 
*/
Maquina *Prueba3_new_b(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);


/**
  @brief Para ejercitar las transiciones en una máquina con un estado anidado y otro no anidado
  [*] --> A
  A{
    [*] --> B
  }
  A --> A.B : T1
  A.B --> C : T2
  C --> A : T3
  A --> C : T5
  C --> A.B : T6
  @param colaEventos 
  @param despachador 
  @param pilaEstados 
  @param timer 
  @return Maquina* 
*/
Maquina *Prueba4_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer);

#endif
