#ifndef MAQUINA_PRUEBA_H
#define MAQUINA_PRUEBA_H

#include <maquina_estado.h>

enum EventoMaquinaPrueba{
    EV_E1 = EV_USUARIO,
    EV_E2,
    EV_E3,
    EV_E4,
    EV_E5,
    EV_E6,
    EV_E7,
    EV_E8,
    EV_E9,
};

/**
 * @brief Máquina de estado para pruebas. Todas las
 * transiciones posibles entre tres estados
 * 
 * [*] --> A : RESET/accion0
 * A : E1/accion1
 * A --> B : E2/accion2
 * B : E3/accion3
 * B --> C : E4/accion4
 * C : E5/accion5
 * C --> A : E6/accion6
 * A --> C : E7/accion7
 * C --> B : E8/accion8
 * B --> A : E9/accion9
 * 
 * @param self 
 */
void MaquinaPrueba_init(Maquina *self);


/* Acciones -- deben ser implementadas */

void accion0(void);
void accion1(void);
void accion2(void);
void accion3(void);
void accion4(void);
void accion5(Maquina *maquina);
void accion6(void);
void accion7(void);
void accion8(void);
void accion9(void);


#endif
