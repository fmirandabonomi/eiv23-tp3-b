#ifndef ME_I_RECEPTOR_EVENTO_H
#define ME_I_RECEPTOR_EVENTO_H
#include <me_pub/me_evento.h>
/**
 * @brief Objeto capaz de recibir eventos
 * 
 */
typedef struct IReceptorEvento IReceptorEvento;

struct IReceptorEvento{
    bool (*dispatch)(IReceptorEvento *self,Evento e);
};

inline bool IReceptorEvento_dispatch(IReceptorEvento *self,Evento e){
    return self->dispatch(self,e);
}

#endif
