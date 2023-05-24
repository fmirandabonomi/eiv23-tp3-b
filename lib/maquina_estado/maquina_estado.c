#include <me_impl/maquina_estado_impl.h>
#include <stddef.h>
#include <interf_impl/container_of.h>

static bool ReceptorEvento_dispatch(IReceptorEvento *iReceptorEvento,Evento e);

static bool SoporteEstado_dispatchInterno(ISoporteEstado *iSoporteEstado,Evento e);
static bool SoporteEstado_dispatchExterno(ISoporteEstado *iSoporteEstado,Evento e);
static ITimer *SoporteEstado_getITimer(ISoporteEstado *iSoporteEstado);

static const ISoporteEstado_VT soporteEstado_VT = {
    .dispatchInterno = SoporteEstado_dispatchInterno,
    .dispatchExterno = SoporteEstado_dispatchExterno,
    .getTimer = SoporteEstado_getITimer
};

void Maquina_init(
    Maquina *self,
    Estado *inicial,
    IColaEventos *colaEventos, 
    IDespachadorEvento *despachador, 
    IPilaEstados *pilaEstados, 
    ITimer *timer
){
    self->iReceptorEvento.dispatch = ReceptorEvento_dispatch;
    self->iSoporteEstado._vptr = &soporteEstado_VT;
    self->activo      = NULL;
    self->inicial     = inicial;
    self->colaEventos = colaEventos;
    self->despachador = despachador;
    self->pilaEstados = pilaEstados;
    self->timer       = timer;
}

bool Maquina_dispatch(Maquina *self,Evento e){
    return IColaEventos_dispatch(self->colaEventos,e);
}

static Estado *abandonaEstado(Maquina *self,Estado *estado){
    if(estado){
        Estado_procesaEvento(estado,&self->iSoporteEstado,(Evento){.mensaje=MSG_SALIDA});
        estado = Estado_getPadre(estado);
    }
    return estado;
}
static Estado *abandonaHastaNivel(Maquina *self,Estado *estado, NivelEstado nivel){
    while(estado && Estado_getNivel(estado) > nivel)
        estado = abandonaEstado(self,estado);
    return estado;
}

static void pushEstado(Maquina *self,Estado *estado){
    while(!estado);
    bool const r = IPilaEstados_push(self->pilaEstados,estado);
    while(!r);
}
static Estado *pushEstadoYSubeNivel(Maquina *self,Estado *estado){
    if(estado){
        pushEstado(self,estado);
        estado = Estado_getPadre(estado);
    }
    return estado;
}

static Estado *pushEstadoHastaNivel(Maquina *self,Estado *estado,NivelEstado nivel){
    while(estado && Estado_getNivel(estado) > nivel)
        estado = pushEstadoYSubeNivel(self,estado);
    return estado;
}

static NivelEstado NivelEstado_min(NivelEstado a,NivelEstado b){
    return (a<b) ? a : b;
}
/**
 * @brief Asciende al ancestro común del estado actual y el destino procesando
 * el evento de salida de los estados salientes y añadiendo a la pila los entrantes
 * 
 * @param self Este objeto Maquina de estado
 * @param destino Estado de destino (el origen es el estado actual)
 * @param interna Si es true y origen es subestado de destino o viceversa, evita 
 * ejecutar salida y entrada del estado superior. Si es falso ejecuta salida y entrada
 * del estado superior.
 */
static void Maquina__asciendeAlAncestroComun_(Maquina *self,Estado *destino, bool interna){
    Estado *pd=destino,*po=self->activo;
    if(!pd) return;

    if (po){
        /* Recorre hasta el ancestro común*/
        NivelEstado n = NivelEstado_min(Estado_getNivel(po),Estado_getNivel(pd));
        po = abandonaHastaNivel(self,po,n);
        pd = pushEstadoHastaNivel(self,pd,n);
        if(Estado_getNivel(po) != Estado_getNivel(pd)){
            while(1);
        }
        if(po == pd && !interna){
            abandonaEstado(self,po);
            pushEstado(self,pd);
        }
        while(po != pd && n>0){
            po = abandonaEstado(self,po);
            pd = pushEstadoYSubeNivel(self,pd);
            --n;
        }
        if (po != pd){
            abandonaEstado(self,po);
            pushEstado(self,pd);
        }
    }else{
        pd = pushEstadoHastaNivel(self,pd,0);
        pushEstado(self,pd);
    }

}
/**
 * @brief Ejecuta el evento de entrada en todos los estados de
 * la pila de estados, removiéndolos de la misma.
 * 
 * @param self Este objeto Maquina de estado
 */
static void Maquina__desciendeAlObjetivo_(Maquina *self){
    while(!IPilaEstados_qEmpty(self->pilaEstados)){
        Estado *const pd = IPilaEstados_pop(self->pilaEstados);
        while(!pd);
        Estado_procesaEvento(pd,&self->iSoporteEstado,(Evento){.mensaje=MSG_ENTRADA});
    }
}

/**
 * @brief Procesa un evento usando el estado actual de la máquina
 * 
 * @param self Este objeto Maquina de estado. El estado actual es el que procesa
 * el evento. No puede ser NULL.
 * @param e Evento a procesar
 * @return ResultadoEvento Resultado del proceso, puede ser un pedido de transición
 */
static ResultadoEvento Maquina__procesaEvento_(Maquina *self,Evento e){
    Estado *estado = self->activo;
    while(!estado);
    return Estado_procesaEvento(self->activo,&self->iSoporteEstado,e);
}
/**
 * @brief Ejecuta una transición de estado en tres fases. Asciende al ancestro
 * común de origen y destino saliendo de los estados de la rama origen y añadiendo
 * a la cola de estados los estados de la rama destino. Desciende al estado destino
 * entrando en todos los estados guardados en la pila. Sustituye el estado actual
 * con el destino y procesa el evento de inicialización.
 * 
 * @note El estado origen es el estado actual, si el estado actual es NULL entonces
 * como primer paso alternativo pone en la pila el estado destino y todos sus ancestros.
 * 
 * @param self Este objeto Máquina de estado. El estado origen es el estado actual y
 * puede ser NULL.
 * @param destino Estado destino. No puede ser NULL.
 * @param interna Si es true y origen es subestado de destino o viceversa, evita 
 * ejecutar salida y entrada del estado superior. Si es falso ejecuta salida y entrada
 * del estado superior.
 * @return ResultadoEvento Resultado del evento de inicialización en el estado destino
 */
static ResultadoEvento Maquina__transicion_(Maquina *self,Estado *destino,bool interna){
    Maquina__asciendeAlAncestroComun_(self,destino,interna);
    Maquina__desciendeAlObjetivo_(self);
    self->activo = destino;
    return Maquina__procesaEvento_(self,(Evento){.mensaje=MSG_INICIALIZA});
}

/**
 * @brief Procesa solicitudes de transición generadas al procesar eventos
 * 
 * @param self Este objeto máquina de estado
 * @param r Resultado de procesamiento de evento
 * @return ResultadoEvento Valor original del resultado de procesamiento de
 * evento si este no contenía solicitud de transición. Resultado del procesamiento
 * del evento de inicialización por parte del primer estado que no disparó transición
 * al inicializar.
 */
static ResultadoEvento Maquina__procesaSolicitudesDeTransicion_(Maquina *self,ResultadoEvento r){
    while(RES_TRANSICION_EXTERNA == r.codigo || RES_TRANSICION_INTERNA == r.codigo){
        r = Maquina__transicion_(self,r.param,RES_TRANSICION_INTERNA == r.codigo);
    }
    return r;
}


ResultadoEvento Maquina_procesa(Maquina *self){
    ResultadoEvento r = {.codigo=RES_NULO};
    if(IColaEventos_queryDisponible(self->colaEventos)){
        Evento e = IColaEventos_getSiguiente(self->colaEventos);
        if(e.mensaje == MSG_INICIALIZA){
            r = Maquina__transicion_(self,self->inicial,false);
        }else{
            r = Maquina__procesaEvento_(self,e);
        }
        r = Maquina__procesaSolicitudesDeTransicion_(self,r);
    }
    return r;
}

bool Maquina_addReceptorEvento(Maquina *self, IReceptorEvento *rx){
    return IDespachadorEvento_addReceptor(self->despachador,rx);
}

bool Maquina_remReceptorEvento(Maquina *self, IReceptorEvento *rx){
    return IDespachadorEvento_remReceptor(self->despachador,rx);
}

IReceptorEvento *Maquina_asIReceptorEvento(Maquina *self){
    return &self->iReceptorEvento;
}


/* Implementación de interfaces */

static bool ReceptorEvento_dispatch(IReceptorEvento *iReceptorEvento,Evento e){
    Maquina *self = container_of(iReceptorEvento,Maquina,iReceptorEvento);
    return Maquina_dispatch(self,e);
}


static bool SoporteEstado_dispatchInterno(ISoporteEstado *iSoporteEstado,Evento e){
    Maquina *self = container_of(iSoporteEstado, Maquina, iSoporteEstado);
    return Maquina_dispatch(self,e);
}
static bool SoporteEstado_dispatchExterno(ISoporteEstado *iSoporteEstado,Evento e){
    Maquina *self = container_of(iSoporteEstado, Maquina, iSoporteEstado);
    return IDespachadorEvento_dispatch(self->despachador,e);
}
static ITimer *SoporteEstado_getITimer(ISoporteEstado *iSoporteEstado){
    Maquina *self = container_of(iSoporteEstado, Maquina, iSoporteEstado);
    return self->timer;
}
