#ifndef ME_I_SOPORTE_ESTADO_H
#define ME_I_SOPORTE_ESTADO_H
#include <i_timer.h>
/**
 * @brief Objeto que proporciona servicios necesarios para despacho de mensajes
 * y programación de acciones temporizadas. Provistos por la máquina de estado
 * 
 */
typedef struct ISoporteEstado ISoporteEstado;
/**
 * @brief abla de funciones virtuales para una implementación
 * de la interfaz @ref ISoporteEstado
 * 
 */
typedef struct ISoporteEstado_VT ISoporteEstado_VT;

struct ISoporteEstado_VT{
    bool (* const dispatchInterno)(ISoporteEstado *self,Evento e);
    bool (* const dispatchExterno)(ISoporteEstado *self,Evento e);
    ITimer * (* const getTimer)(ISoporteEstado *self);
};
struct ISoporteEstado{
    ISoporteEstado_VT const *_vptr;
};

/**
 * @brief Despacha un evento a la propia máquina de estado
 * 
 * @param self Este objeto ISoporteEstado
 * @param e Evento a despachar
 * @return true Evento despachado
 * @return false No pudo despacharse el evento
 */
inline bool ISoporteEstado_dispatchInterno(ISoporteEstado *self,Evento e){
    return self->_vptr->dispatchInterno(self,e);
}
/**
 * @brief Despacha un evento a los receptores registrados con la
 * máquina de estado
 * 
 * @param self Este objeto ISoporteEstado
 * @param e Evento a despachar
 * @return true Evento despachado
 * @return false No pudo despacharse el evento
 */
inline bool ISoporteEstado_dispatchExterno(ISoporteEstado *self,Evento e){
    return self->_vptr->dispatchExterno(self,e);
}
/**
 * @brief Obtiene el objeto `ITimer` que provee el servicio de temporización
 * para la máquina de estado
 * 
 * @param self Este objeto ISoporteEstado
 * @return ITimer*  Objeto ITimer que provee el servicio de temporización
 */
inline ITimer * ISoporteEstado_getTimer(ISoporteEstado *self){
    return self->_vptr->getTimer(self);
}

#endif
