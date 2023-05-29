#ifndef I_ACCION_H
#define I_ACCION_H

typedef struct IAccion IAccion;

typedef void IAccion_Ejecutar(IAccion *self);

typedef struct IAccion_VT{
    IAccion_Ejecutar *ejecutar;
}IAccion_VT;

struct IAccion{
    IAccion_VT const * _vptr;
};

__attribute__((always_inline))
static inline void IAccion_ejecuta(IAccion *self){
    if(self->_vptr) self->_vptr->ejecutar(self);
}
#endif
