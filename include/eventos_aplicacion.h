#ifndef EVENTOS_APLICACION_H
#define EVENTOS_APLICACION_H
#include <maquina_estado.h>

enum EventoAplicacion{
    /**
     * @brief El botón de comando del controlador fue pulsado
     * 
     */
    EV_BOTON_PULSADO = EV_USUARIO,
    /**
     * @brief Triple pulsación del botón 
     * 
     */
    EV_TOQUE_TRIPLE,
    /**
     * @brief Transcurrió el tiempo programado
     *  
     */
    EV_TIMEOUT,

};

#endif
