#ifndef ME_ESTADO_H
#define ME_ESTADO_H
#include <me_pub/me_evento.h>
#include <me_pub/me_i_soporte_estado.h>

/**
 * @brief Objeto que determina el comportamiento de una máquina
 * de estado ante un evento.
 * 
 */
typedef struct Estado Estado;
/**
 * @brief Nivel de anidamiento del estado. Número de estados ancestros
 * de un estado dado.
 * 
 */
typedef unsigned NivelEstado;
/**
 * @brief Información sobre el resultado del proceso de un
 * evento. Puede consistir en una solicitud de cambio de estado
 * 
 */
typedef struct ResultadoEvento ResultadoEvento;
/**
 * @brief Indica el resultado del proceso de un evento.
 * 
 */
typedef enum CodigoResultado{
    /**
     * @brief No había un evento pendiente. Este código solo puede
     * ser retornado por la máquina de estado.
     * 
     */
    RES_NULO,
    /**
     * @brief El evento fue ignorado
     * 
     */
    RES_EVENTO_IGNORADO,
    /**
     * @brief El evento fue procesado dando origen a alguna acción
     * 
     */
    RES_EVENTO_PROCESADO,
    /**
     * @brief El evento disparó una transición interna. El estado
     * destino está en el parámetro de estado. Este código no puede
     * ser retornado por la máquina de estado sino que debe dar lugar
     * a la transición indicada.
     * 
     */
    RES_TRANSICION_INTERNA,
    /**
     * @brief El evento disparó una transición externa. El estado
     * destino está en el parámetro de estado. Este código no puede
     * ser retornado por la máquina de estado sino que debe dar lugar
     * a la transición indicada.
     * 
     */
    RES_TRANSICION_EXTERNA,
    NUM_CODIGOS_RES
}CodigoResultado;

struct ResultadoEvento{
    /**
     * @brief Indica el resultado de procesar el evento. Puede incluir
     * una solicitud de cambio de estado
     * 
     */
    CodigoResultado codigo;
    /**
     * @brief Estado destino cuando se solicita una transición. Estado 
     * actual en otro caso.
     * 
     */
    Estado *param;
};

/**
 * @brief Procesa un evento, realizando las acciones necesarias y posiblemente
 * solicitando un cambio de estado
 * 
 * @param self Este objeto Estado
 * @param ctx Objeto que acepta los eventos despachados como parte de las acciones
 * disparadas por un evento procesado
 * @param e Evento a procesar
 * @return ResultadoEvento Resultado del proceso de evento, puede ser
 * RES_IGNORADO, RES_PROCESADO, RES_TRANSICION_INTERNA o RES_TRANSICION_EXTERNA
 */
ResultadoEvento Estado_procesaEvento(Estado *self, ISoporteEstado *ctx,Evento e);
/**
 * @brief Nivel de anidamiento del estado. Es el número de ancestros de este estado.
 * 
 * @param self Este objeto Estado
 * @return NivelEstado Número de ancestros de este estado
 */
NivelEstado Estado_getNivel(Estado *self);
/**
 * @brief Ancestro del cual este estado es descendiente inmediato
 * 
 * @param self Este objeto Estado
 * @return Estado* Estado padre.
 * @return NULL si no hay un estado padre.
 */
Estado *Estado_getPadre(Estado *self);


#endif
