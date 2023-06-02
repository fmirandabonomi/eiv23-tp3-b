#ifndef CONTROLADOR_LUZ_H
#define CONTROLADOR_LUZ_H
#include <maquina_estado.h>
#include <stdint.h>
#include "soporte_controlador.h"

enum EventoControladorLuz{
    /**
     * @brief El botón de comando del controlador fue pulsado
     * 
     */
    EV_BOTON_PULSADO = EV_USUARIO,
    /**
     * @brief Transcurrió el tiempo programado
     *  
     */
    EV_TIMEOUT
};

/**
 * @brief Controlador de luz de escalera
 * 
 */
typedef struct ControladorLuz{
    Maquina maquina;
    uint32_t tiempoOn;
    SoporteControlador *soporte;
}ControladorLuz;


/**
 * @brief Controlador de luz de escalera
 * 
 *  [*] --> APAGADO       : EV_RESET/ luzOff();
 *  APAGADO --> ENCENDIDO : EV_BOTON_PULSADO / luzOn(); setTimeout(tiempoOn);
 *  ENCENDIDO --> APAGADO : EV_TIMEOUT / luzOff()
 * 
 * @param self Nueva instancia de controlador
 * @param tiempoOn Tiempo, en milisegundos, que permanece la luz encendida luego de la pulsación
 */
void ControladorLuz_init(ControladorLuz *self,uint32_t tiempoOn,SoporteControlador *soporte);

/**
 * @brief Controlador de luz de escalera como máquina de estado
 * 
 * @param self Este objeto
 * @return Maquina* Este objeto como máquina de estado
 */
Maquina * ControladorLuz_asMaquina(ControladorLuz *self);
 



#endif