#include "accion_despacha_evento.h"
#include <interfaces_impl/container_of.h>
#include <stm32f1xx.h>

static void DespachaEvento_ejecutar(IAccion *iAccion);

static IAccion_VT const despachaEvento_VT= {
    .ejecutar = DespachaEvento_ejecutar
};



void DespachaEvento_init(DespachaEvento *self,Evento evento, Maquina *destino){
    self->accion._vptr=&despachaEvento_VT;
    self->destino = destino;
    self->evento = evento;
    self->estadoDespacho = ED_NO_DESPACHADO;
}

IAccion * DespachaEvento_asIAccion(DespachaEvento *self){
    return &self->accion;
}

static void DespachaEvento_ejecutar(IAccion *iAccion){
    DespachaEvento *self = container_of(iAccion,DespachaEvento,accion);
    if (self->estadoDespacho != ED_NO_INICIALIZADO){
        bool despachado = Maquina_despacha(self->destino,self->evento);
        if(despachado){
            self->estadoDespacho = ED_DESPACHADO;
        }else{
            self->estadoDespacho = ED_FALLA;
        }
    }
}

EstadoDespacho DespachaEvento_getEstadoDespacho(DespachaEvento *self){
    return self->estadoDespacho;
}

Evento DespachaEvento_getEvento(DespachaEvento *self){
    Evento r=EV_NULO;
    if (self->estadoDespacho != ED_NO_INICIALIZADO){
        r = self->evento;
    }
    return r;
}
void DespachaEvento_deInit(DespachaEvento *self){
    self->estadoDespacho=ED_NO_INICIALIZADO;
}
