#include <me_impl/maquina_estado_impl.h>
#include <stddef.h>

static bool IReceptorEvento_dispatch_impl(IReceptorEvento *iReceptorEvento,Evento e);

void Maquina_init(Maquina *self, Estado *inicial, IColaEventos *colaEventos, IDespachadorEvento *despachador, IPilaEstados *pilaEstados){
    self->iReceptorEvento.dispatch = IReceptorEvento_dispatch_impl;
    self->activo      = NULL;
    self->inicial     = inicial;
    self->colaEventos = colaEventos;
    self->despachador = despachador;
    self->pilaEstados = pilaEstados;
}

bool Maquina_dispatch(Maquina *self,Evento e){
    return IColaEventos_dispatch(self->colaEventos,e);
}

static bool IReceptorEvento_dispatch_impl(IReceptorEvento *iReceptorEvento,Evento e){
    Maquina *self = (Maquina*)iReceptorEvento;
    return Maquina_dispatch(self,e);
}

static Estado *abandonaEstado(Maquina *self,Estado *estado){
    if(estado){
        Estado_procesaEvento(estado,self->despachador,(Evento){.mensaje=MSG_SALIDA});
        estado = Estado_getPadre(estado);
    }
    return estado;
}
static Estado *abandonaHastaNivel(Maquina *self,Estado *estado, NivelEstado nivel){
    while(estado && Estado_getNivel(estado) > nivel)
        estado = abandonaEstado(self,estado);
    return estado;
}

static Estado *pushEstadoYSubeNivel(Maquina *self,Estado *estado){
    if(estado){
        bool const r = IPilaEstados_push(self->pilaEstados,estado);
        while(!r);
        estado = Estado_getPadre(estado);
    }
    return estado;
}

static Estado *pushEstadoHastaNivel(Maquina *self,Estado *estado,NivelEstado nivel){
    while(estado && Estado_getNivel > nivel)
        estado = pushEstadoYSubeNivel(self,estado);
}

static ResultadoEvento Maquina__transicion_(Maquina *self,Estado *destino,bool interna){
    
}

static ResultadoEvento Maquina__procesa_(Maquina *self,Evento e){
    ResultadoEvento r = Estado_procesaEvento(self->activo,self->despachador,e);
    if (RES_TRANSICION_EXTERNA == r.codigo || RES_TRANSICION_INTERNA == r.codigo){
        r = Maquina__transicion_(self,r.param,RES_TRANSICION_INTERNA == r.codigo);
    }
}

static ResultadoEvento Maquina__inicializa_(Maquina *self){
    self->activo = abandonaHastaNivel(self,self->activo,0);
    return Maquina__transicion_(self,self->inicial,false);
}

ResultadoEvento Maquina_procesa(Maquina *self){
    ResultadoEvento r = {.codigo=RES_NULO};
    if(IColaEventos_queryDisponible(self->colaEventos)){
        Evento e = IColaEventos_getSiguiente(self->colaEventos);
        if(e.mensaje == MSG_INICIALIZA){
            r = Maquina__inicializa_(self);
        }else{
            r = Maquina__procesa_(self,e);
        }
    }
    return r;
}

bool Maquina_addReceptorEvento(Maquina *self, IReceptorEvento *rx);

bool Maquina_remReceptorEvento(Maquina *self, IReceptorEvento *rx);

IReceptorEvento *Maquina_asIReceptorEvento(Maquina *self){
    return &self->iReceptorEvento;
}

