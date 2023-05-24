#ifndef ME_I_COLA_EVENTOS_H
#define ME_I_COLA_EVENTOS_H

#include <me_pub/me_evento.h>
#include <stdbool.h>

/**
 * @brief Permite almacenar eventos para su
 * posterior procesamiento asíncrono por una máquina 
 * de estado.
 * 
 */
typedef struct IColaEventos IColaEventos;
/**
 * @brief Tabla de funciones virtuales para una implementación
 * de la interfaz @ref IColaEventos
 * 
 */
typedef struct IColaEventos_VT IColaEventos_VT;

struct IColaEventos_VT{
    /**
     * @brief Implementación del método @ref IColaEventos_dispatch
     */
    bool (* const dispatch)(IColaEventos *self, Evento e);
    /**
     * @brief Implementación del método @ref IColaEventos_queryDisponible
     */
    bool (* const queryDisponible)(IColaEventos *self);
    /**
     * @brief Implementación del método @ref IColaEventos_getSiguiente
     * 
     */
    Evento (* const getSiguiente)(IColaEventos *self);
};

struct IColaEventos{
    IColaEventos_VT const * _vptr;
};

/**
 * @brief Método de despacho. Añade un nuevo evento a la cola si
 * hay espacio suficiente.
 * 
 * @param self Este objeto
 * @param e Evento a añadir (por valor)
 * @return true Evento añadido
 * @return false Cola llena
 */
inline bool IColaEventos_dispatch(IColaEventos *self, Evento e){
    return self->_vptr->dispatch(self,e);
}


/**
 * @brief Método de consulta. Indica si hay eventos disponibles en
 * la cola
 * 
 * @param self Este objeto
 * @return true Eventos disponibles
 * @return false Cola vacía
 */
inline bool IColaEventos_queryDisponible(IColaEventos *self){
    return self->_vptr->queryDisponible(self);
}

/**
 * @brief Método de descarga. Obtiene el próximo evento disponible
 * en la cola y libera la memoria utilizada por el mismo.
 * 
 * @param self Este objeto
 * @return Evento El primer elemento de la cola. Si la cola está vacía
 * retorna (Evento){.mensaje=MSG_NULO,.param.iVal=-1}
 */
inline Evento IColaEventos_getSiguiente(IColaEventos *self){
    return self->_vptr->getSiguiente(self);
}

#endif
