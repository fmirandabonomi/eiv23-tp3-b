#ifndef ME_DESPACHADOR_EVENTO_H
#define ME_DESPACHADOR_EVENTO_H

#include <me_pub/me_i_receptor_evento.h>

/**
 * @brief Mantiene una lista de receptores de evento. Despacha eventos
 * a los receptores de la lista. Permite añadir y quitar receptores
 * 
 */
typedef struct IDespachadorEvento IDespachadorEvento;
/**
 * @brief Tabla de funciones que implementan la interfaz abstracta
 * @ref IDespachadorEvento
 * 
 */
typedef struct IDespachadorEvento_VT IDespachadorEvento_VT; 


struct IDespachadorEvento_VT{
    bool (* const addReceptor)(IDespachadorEvento *self, IReceptorEvento *rx);
    bool (* const remReceptor)(IDespachadorEvento *self, IReceptorEvento *rx);
    bool (* const dispatch   )(IDespachadorEvento *self,Evento e);
};

struct IDespachadorEvento{
    /**
     * @brief Puntero a tabla de funciones que implementan la interfaz abstracta
     * 
     */
    IDespachadorEvento_VT const *_vptr;
};

/**
 * @brief Añade un receptor a la lista de notificación
 * 
 * @param self Este objeto Notificador
 * @param rx Receptor de evento
 * @return true Receptor añadido
 * @return false No pudo añadirse el receptor
 */
inline bool IDespachadorEvento_addReceptor(IDespachadorEvento *self, IReceptorEvento *rx){
    return self->_vptr->addReceptor(self,rx);
}
/**
 * @brief Remueve un receptor de la lista de notificación
 * 
 * @param self Este objeto Notificador
 * @param rx Receptor de evento
 * @return true Receptor eliminado
 * @return false Receptor no hayado en el listado
 */
inline bool IDespachadorEvento_remReceptor(IDespachadorEvento *self, IReceptorEvento *rx){
    return self->_vptr->remReceptor(self,rx);
}
/**
 * @brief Despacha un evento a todos los receptores registrados
 * 
 * @param self Este objeto Notificador
 * @param e Evento a despachar
 * @return true Al menos un receptor aceptó el evento
 * @return false Ningún receptor aceptó el evento
 */
inline bool IDespachadorEvento_dispatch(IDespachadorEvento *self, Evento e){
    return self->_vptr->dispatch(self,e);
}

#endif
