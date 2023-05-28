#ifndef ME_I_RECEPTOR_EVENTO_H
#define ME_I_RECEPTOR_EVENTO_H
#include <me_pub/me_evento.h>
#include <stdbool.h>

/**
 * @brief Objeto capaz de recibir eventos en forma asincrónica
 * 
 */
typedef struct IReceptorEvento IReceptorEvento;

struct IReceptorEvento{
    bool (*dispatch)(IReceptorEvento *self,Evento e);
};

/**
 * @brief Despacha un evento para posterior procesamiento
 * 
 * @param self Este objeto IReceptorEvento
 * @param e Evento
 * @return true Evento despachado
 * @return false Evento no aceptado
 */
inline bool IReceptorEvento_dispatch(IReceptorEvento *self,Evento e){
    return self->dispatch(self,e);
}

#endif
