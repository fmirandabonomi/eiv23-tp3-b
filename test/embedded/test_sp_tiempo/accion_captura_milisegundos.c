#include "accion_captura_milisegundos.h"
#include <stddef.h>
#include <interfaces_impl/container_of.h>
#include <soporte_placa.h>

static void CapturaMilisegundos_ejecutar(IAccion *accion);
struct IAccion_VT const accion_VT ={
    .ejecutar = CapturaMilisegundos_ejecutar
};


static void CapturaMilisegundos_ejecutar(IAccion *accion){
    CapturaMilisegundos *self = container_of(accion,CapturaMilisegundos,accion);
    if(self && !self->capturado){
        self->captura = SP_Tiempo_getMilisegundos() - self->captura;
        self->capturado = true;
    }
}


void CapturaMilisegundos_init(CapturaMilisegundos *self,uint32_t valorInicial){
    if (self){
        self->accion._vptr = &accion_VT;
        self->captura = valorInicial;
        self->capturado = false;
    }
}
IAccion *CapturaMilisegundos_getAccionCaptura(CapturaMilisegundos *self){
    IAccion *r = NULL;
    if(self){
        r = &self->accion;
    }
    return r;
}
bool CapturaMilisegundos_qCapturado(CapturaMilisegundos *self){
    bool r = false;
    if (self){
        r = self->capturado;
    }
    return r;
}
uint32_t CapturaMilisegundos_getValor(CapturaMilisegundos *self){
    uint32_t r = 0;
    if (CapturaMilisegundos_qCapturado(self)){
        r = self->captura;
    }
    return r;
}