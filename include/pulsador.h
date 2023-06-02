#ifndef PULSADOR_H
#define PULSADOR_H

#include <soporte_placa.h>
#include <stdint.h>
#include <stdbool.h>
#include <i_accion.h>
/**
 * @brief Mecanismo antirrebote y ejecución de 
 * acción cuando detecta pulsado
 * 
 */
typedef struct Pulsador{
    IAccion *accionPulsado;
    uint32_t t0;
    SP_HPin pin;
    uint8_t histeresis;
    uint8_t acumulador;
    bool estadoPresionado;
    bool nivelPresionado;
}Pulsador;

/**
 * @brief Inicializa el mecanismo antirrebote
 * 
 * @param self Memoria reservada para el nuevo objeto
 * @param pin Pin donde está conectado el pulsador. Es configurado en esta rutina
 * de inicialización con un resistor pull al nivel opuesto de nivelPresionado 
 * @param nivelPresionado Nivel que toma el pin cuando el pulsador es presionado
 * @param histeresis Cantidad de lecturas seguidas en valor opuesto necesarias 
 * para cambiar el estado del antirrebote
 * @param accionPulsado  Accion a ejecutar cuando se detecta la liberación del
 * pulsador
 */
void Pulsador_init(Pulsador *self, SP_HPin pin,bool nivelPresionado,uint8_t histeresis, IAccion *accionPulsado);


/**
 * @brief Rutina a ser llamada en el lazo principal para efecutar el proceso
 * de antirrebote.
 * 
 */
void Pulsador_procesa(Pulsador *self);

/**
 * @brief Retorna verdadero si el pulsador se encuentra presionado
 * 
 * @param self Este objeto
 * @return true Pulsador presionado
 * @return false Pulsador no presionado
 */
bool Pulsador_qPresionado(Pulsador *self);
#endif
