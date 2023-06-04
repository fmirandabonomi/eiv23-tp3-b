#ifndef CONTROLADOR_LUZ_H
#define CONTROLADOR_LUZ_H
#include <stdint.h>
#include <soporte_placa.h>
#include <maquina_estado.h>
#include "despacho_retardado.h"
#include "eventos_aplicacion.h"



/**
 * @brief Controlador de luz de escalera
 * 
 */
typedef struct ControladorLuz{
    Maquina maquina;
    uint32_t tiempoOn;
    DespachoRetardado *despachoRetardado;
    struct{
        SP_HPin pin;
        bool nivelOn;
    }interfazLuz;
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
 * @param pinLuz Pin donde está conectado el control de encendido de la luz
 * @param nivelLuzOn Nivel que debe tener el pinLuz para que la luz esté encendida
 * @param despachoRetardado Controlador de despacho de eventos retardados
 */
void ControladorLuz_init(ControladorLuz *self,uint32_t tiempoOn,SP_HPin pinLuz,bool nivelLuzOn,DespachoRetardado *despachoRetardado);

/**
 * @brief Controlador de luz de escalera como máquina de estado
 * 
 * @param self Este objeto
 * @return Maquina* Este objeto como máquina de estado
 */
Maquina * ControladorLuz_asMaquina(ControladorLuz *self);
 



#endif