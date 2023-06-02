#include <accion_despacha_evento.h>
#include <interfaces_impl/container_of.h>

static void AccionDespachaEvento_ejecutar(IAccion *iAccion);

static IAccion_VT const despachaEvento_VT= {
    .ejecutar = AccionDespachaEvento_ejecutar
};



void AccionDespachaEvento_init(AccionDespachaEvento *self,Evento evento, Maquina *destino){
    self->accion._vptr=&despachaEvento_VT;
    self->destino = destino;
    self->evento = evento;
    self->estadoDespacho = ED_NO_DESPACHADO;
}

IAccion * AccionDespachaEvento_asIAccion(AccionDespachaEvento *self){
    return &self->accion;
}

static void AccionDespachaEvento_ejecutar(IAccion *iAccion){
    AccionDespachaEvento *self = container_of(iAccion,AccionDespachaEvento,accion);
    if (self->estadoDespacho != ED_NO_INICIALIZADO){
        bool despachado = Maquina_despacha(self->destino,self->evento);
        if(despachado){
            self->estadoDespacho = ED_DESPACHADO;
        }else{
            self->estadoDespacho = ED_FALLA;
        }
    }
}

EstadoDespacho AccionDespachaEvento_getEstadoDespacho(AccionDespachaEvento *self){
    return self->estadoDespacho;
}

Evento AccionDespachaEvento_getEvento(AccionDespachaEvento *self){
    Evento r=EV_NULO;
    if (self->estadoDespacho != ED_NO_INICIALIZADO){
        r = self->evento;
    }
    return r;
}
void AccionDespachaEvento_deInit(AccionDespachaEvento *self){
    self->estadoDespacho=ED_NO_INICIALIZADO;
}
