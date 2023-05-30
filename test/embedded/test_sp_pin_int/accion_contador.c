#include "accion_contador.h"
#include <stddef.h>
#include <interfaces_impl/container_of.h>

static void Contador_accionIncrementaCuenta(IAccion *accion);

static IAccion_VT const accion_VT = {
    .ejecutar = Contador_accionIncrementaCuenta
};

void Contador_init(Contador *self){
    if(self){
        self->accion._vptr=&accion_VT;
        self->cuenta=0;
    }
}

IAccion * Contador_getAccionIncrementaCuenta(Contador *self){
    IAccion *r = NULL;
    if(self){
        r = &self->accion;
    }
    return r;
}

uint32_t Contador_getCuenta(Contador *self){
    uint32_t r=0;
    if(self){
        r=self->cuenta;
    }
    return r;
}


static void Contador_accionIncrementaCuenta(IAccion *accion){
    Contador *self = container_of(accion,Contador,accion);
    if(self){
        ++self->cuenta;
    }
}