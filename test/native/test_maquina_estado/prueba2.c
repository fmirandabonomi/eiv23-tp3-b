#include "maquinas_de_prueba_impl.h"

struct Prueba2{
    Maquina maquina;
    Estado estadoA;
    Estado estadoB;
};

static ResultadoEvento fnA(Estado *estadoA,ISoporteEstado *soporte,Evento evento);
static ResultadoEvento fnB(Estado *estadoB,ISoporteEstado *soporte,Evento evento);

Prueba2 *Prueba2_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer){
    Prueba2 *r;
    r = malloc(sizeof(*r));
    TEST_ASSERT_NOT_NULL(r);
    Estado_init(&r->estadoA,NULL,fnA);
    Estado_init(&r->estadoB,NULL,fnB);
    Maquina_init(&r->maquina,&r->estadoA,colaEventos,despachador,pilaEstados,timer);
    return r;
}

Maquina *Prueba2_asMaquina(Prueba2 *self){
    return &self->maquina;
}

void Prueba2_delete(Prueba2 *self){
    Maquina_finalize(&self->maquina);
    free(self);
}


static ResultadoEvento fnA(Estado *estadoA,ISoporteEstado *soporte,Evento evento){
    Prueba2 *const self = container_of(estadoA,Prueba2,estadoA);
    Evento const acuse = EVENTO_ACUSE('A',evento.mensaje);
    ResultadoEvento r={0};
    switch(evento.mensaje){
    case MSG_INICIALIZA:
    /* FALLTHRU */ case MSG_ENTRADA:
    /* FALLTHRU */ case MSG_T1:
    /* FALLTHRU */ case MSG_SALIDA:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T2:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = estadoA;
    break; case MSG_T3:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoB;
    break; default:
        r.codigo = RES_EVENTO_IGNORADO;
    break;
    }
    return r;
}

static ResultadoEvento fnB(Estado *estadoB,ISoporteEstado *soporte,Evento evento){
    Prueba2 *const self = container_of(estadoB,Prueba2,estadoB);
    Evento const acuse = EVENTO_ACUSE('B',evento.mensaje);
    ResultadoEvento r={0};
    switch(evento.mensaje){
    case MSG_INICIALIZA:
    /* FALLTHRU */ case MSG_ENTRADA:
    /* FALLTHRU */ case MSG_T4:
    /* FALLTHRU */ case MSG_SALIDA:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T5:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = estadoB;
    break; case MSG_T6:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoA;
    break; default:
        r.codigo = RES_EVENTO_IGNORADO;
    break;
    }
    return r;
}
