#ifndef ME_I_PILA_ESTADOS_H
#define ME_I_PILA_ESTADOS_H

#include <me_pub/me_estado.h>

/**
 * @brief Mantiene una pila de estados que soporta las
 * operaciones push y pop.
 * 
 */
typedef struct IPilaEstados IPilaEstados;
/**
 * @brief Tabla de funciones que implementan la interfaz
 * abstracta @ref IPilaEstados
 * 
 */
typedef struct IPilaEstados_VT IPilaEstados_VT;

struct IPilaEstados_VT{
    /**
     * @brief Implementación del método @ref IPilaEstados_push
     * 
     */
    bool     (* const push   )(IPilaEstados *self, Estado *estado);
    /**
     * @brief Implementación del método @ref IPilaEstados_pop
     * 
     */
    Estado * (* const pop    )(IPilaEstados *self);
    /**
     * @brief Implementación del método @ref IPilaEstados_qEmpty
     * 
     */
    bool     (* const qEmpty )(IPilaEstados *self);
};

struct IPilaEstados{
    IPilaEstados_VT *_vptr;
};

/**
 * @brief Ingresa estado en la pila
 * 
 * @param self Este objeto
 * @param estado Estado a ingresar, No nulo
 * @return true Estado ingresado con exito
 * @return false Desborde de pila
 */
inline bool IPilaEstados_push(IPilaEstados *self,Estado *estado){
    return self->_vptr->push(self,estado);
}

/**
 * @brief Toma un estado del tope de la pila
 * 
 * @param self Este objeto
 * @return Estado* Estado que se encontraba en el tope de la pila
 * @return NULL Agotamiento de pila
 */
inline Estado * IPilaEstados_pop(IPilaEstados *self){
    return self->_vptr->pop(self);
}
/**
 * @brief Consulta si la pila está vacía
 * 
 * @param self Este objeto
 * @return true Pila vacía
 * @return false Hay estados en la pila
 */
inline bool IPilaEstados_qEmpty(IPilaEstados *self){
    return self->_vptr->qEmpty(self);
}
#endif
