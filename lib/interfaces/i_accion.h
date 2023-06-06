#ifndef I_ACCION_H
#define I_ACCION_H

typedef struct IAccion IAccion;

typedef void (*IAccion_Ejecutar)(IAccion *self);

struct IAccion{
    IAccion_Ejecutar ejecutar;
};

__attribute__((always_inline))
static inline void IAccion_ejecuta(IAccion *self){
    if(self->ejecutar) self->ejecutar(self);
}
#endif
