#include "maquinas_de_prueba_impl.h"

typedef struct Prueba3{
    Maquina maquina;
    Estado estadoA;
    Estado estadoB;
}Prueba3;

static ResultadoEvento fnA(Estado *estadoA,ISoporteEstado *soporte,Evento evento);
static ResultadoEvento fnB(Estado *estadoB,ISoporteEstado *soporte,Evento evento);

static Prueba3 *Prueba3_new_base(void){
    Prueba3 *r;
    r = malloc(sizeof(*r));
    TEST_ASSERT_NOT_NULL(r);
    Estado_init(&r->estadoA,NULL,fnA);
    Estado_init(&r->estadoB,&r->estadoA,fnB);
    return r;
}
Maquina *Prueba3_new_a(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer){
    Prueba3 *r = Prueba3_new_base();
    Maquina_init(&r->maquina,&r->estadoA,colaEventos,despachador,pilaEstados,timer);
    return (Maquina*) r;
}

Maquina *Prueba3_new_b(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer){
    Prueba3 *r= Prueba3_new_base();
    Maquina_init(&r->maquina,&r->estadoB,colaEventos,despachador,pilaEstados,timer);
    return (Maquina*)r;
}


static ResultadoEvento fnA(Estado *estadoA,ISoporteEstado *soporte,Evento evento){
    Prueba3 *const self = container_of(estadoA,Prueba3,estadoA);
    Evento const acuse = EVENTO_ACUSE('A',evento.mensaje);
    ResultadoEvento r={0};
    switch(evento.mensaje){
    case MSG_INICIALIZA:
    /* FALLTHRU */ case MSG_T5:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_INTERNA;
        r.param  = &self->estadoB;
    break;case MSG_ENTRADA:
    /* FALLTHRU */ case MSG_T1:
    /* FALLTHRU */ case MSG_SALIDA:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T7:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = estadoA;
    break; case MSG_T8:
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
    Prueba3 *const self = container_of(estadoB,Prueba3,estadoB);
    Evento const acuse = EVENTO_ACUSE('B',evento.mensaje);
    ResultadoEvento r={0};
    switch(evento.mensaje){
    case MSG_INICIALIZA:
    /* FALLTHRU */ case MSG_ENTRADA:
    /* FALLTHRU */ case MSG_T2:
    /* FALLTHRU */ case MSG_SALIDA:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T3:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = estadoB;
    break; case MSG_T4:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_INTERNA;
        r.param = &self->estadoA;
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
