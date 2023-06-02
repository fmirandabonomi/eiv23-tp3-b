#ifndef ACCION_DESPACHA_EVENTO_H
#define ACCION_DESPACHA_EVENTO_H
#include <i_accion.h>
#include <maquina_estado.h>

typedef enum EstadoDespacho{
    ED_NO_INICIALIZADO,
    ED_NO_DESPACHADO,
    ED_DESPACHADO,
    ED_FALLA
}EstadoDespacho;

typedef struct AccionDespachaEvento{
    IAccion accion;
    Evento evento;
    Maquina *destino;
    EstadoDespacho estadoDespacho; 
}AccionDespachaEvento;

/**
 * @brief Inicializa el objeto para su uso
 * 
 * @param self Objeto a inicializar
 * @param evento Evento a despachar al ejecutar
 * @param destino Máquina de estado de destino
 */
void AccionDespachaEvento_init(AccionDespachaEvento *self,Evento evento, Maquina *destino);

/**
 * @brief Acción que despacha un evento al ser ejecutada. Puede
 * ser ejecutada múltiples veces para despachar múltiples eventos,
 * el estado de despacho contiene solo el resultado de la última
 * ejecución
 * 
 * @param self Este objeto
 * @return IAccion* Acción
 */
IAccion * AccionDespachaEvento_asIAccion(AccionDespachaEvento *self);

/**
 * @brief Consulta el estado actual del proceso de despacho
 * 
 * @param self 
 * @return ED_NO_INICIALIZADO El objeto no fue inicializado
 * @return ED_NO_DESPACHADO Objeto inicializado pero aún no ha
 * despachado ningún evento
 * @return ED_DESPACHADO El objeto ha despachado un evento
 * @return ED_FALLA El último intento de despacho de evento ha fallado
 */
EstadoDespacho AccionDespachaEvento_getEstadoDespacho(AccionDespachaEvento *self);

/**
 * @brief Evento que será despachado
 * 
 * @param self Este objeto
 * @return EV_NULO No inicializado
 * @return Evento Evento a despachar
 */
Evento AccionDespachaEvento_getEvento(AccionDespachaEvento *self);
/**
 * @brief Establece el estado del objeto en no inicializado,
 * removiendo la configuración realizada al inicializar.
 * 
 * @param self 
 */
void AccionDespachaEvento_deInit(AccionDespachaEvento *self);

#endif
