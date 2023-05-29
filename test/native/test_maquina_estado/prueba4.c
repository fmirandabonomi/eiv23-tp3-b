#include "maquinas_de_prueba_impl.h"

typedef struct Prueba4{
    Maquina maquina;
    Estado estadoA;
    Estado estadoB;
    Estado estadoC;
}Prueba4;

static ResultadoEvento fnA(Estado *estadoA,ISoporteEstado *soporte,Evento evento);
static ResultadoEvento fnB(Estado *estadoB,ISoporteEstado *soporte,Evento evento);
static ResultadoEvento fnC(Estado *estadoB,ISoporteEstado *soporte,Evento evento);

Maquina *Prueba4_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer){
    Prueba4 *r;
    r = malloc(sizeof(*r));
    TEST_ASSERT_NOT_NULL(r);
    Estado_init(&r->estadoA,NULL,fnA);
    Estado_init(&r->estadoB,&r->estadoA,fnB);
    Estado_init(&r->estadoC,NULL,fnC);
    Maquina_init(&r->maquina,&r->estadoA,colaEventos,despachador,pilaEstados,timer);
    return (Maquina*)r;
}


static ResultadoEvento fnA(Estado *estadoA,ISoporteEstado *soporte,Evento evento){
    Prueba4 *self = container_of(estadoA,Prueba4,estadoA);
    ResultadoEvento r = {0};
    Evento acuse = EVENTO_ACUSE('A',evento.mensaje);
    switch(evento.mensaje){
    case MSG_ENTRADA:
    /* FALL_THRU */ case MSG_SALIDA:
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_INICIALIZA:
        r.codigo = RES_TRANSICION_INTERNA;
        r.param = &self->estadoB;
    break; case MSG_T1:
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoB;
    break; case MSG_T5:
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoC;
    break; default:
        r.codigo = RES_EVENTO_IGNORADO;
    break;
    }
    if (r.codigo != RES_EVENTO_IGNORADO){
        ISoporteEstado_dispatchExterno(soporte,acuse);
    }
    return r;
}
static ResultadoEvento fnB(Estado *estadoB,ISoporteEstado *soporte,Evento evento){
    Prueba4 *self = container_of(estadoB,Prueba4,estadoB);
    ResultadoEvento r = {0};
    Evento acuse = EVENTO_ACUSE('B',evento.mensaje);
    switch(evento.mensaje){
    case MSG_ENTRADA:
    /* FALL_THRU */ case MSG_SALIDA:
    /* FALL_THRU */ case MSG_INICIALIZA:
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T2:
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoC;
    break; default:
        r.codigo = RES_EVENTO_IGNORADO;
    break; 
    }
    if (r.codigo != RES_EVENTO_IGNORADO){
        ISoporteEstado_dispatchExterno(soporte,acuse);
    }
    return r;
}
static ResultadoEvento fnC(Estado *estadoC,ISoporteEstado *soporte,Evento evento){
    Prueba4 *self = container_of(estadoC,Prueba4,estadoC);
    ResultadoEvento r = {0};
    Evento acuse = EVENTO_ACUSE('C',evento.mensaje);
    switch(evento.mensaje){
    case MSG_ENTRADA:
    /* FALL_THRU */ case MSG_SALIDA:
    /* FALL_THRU */ case MSG_INICIALIZA:
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T3:
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoA;
    break; case MSG_T6:
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param = &self->estadoB;
    break; default:
        r.codigo = RES_EVENTO_IGNORADO;
    break;
    }
    if (r.codigo != RES_EVENTO_IGNORADO){
        ISoporteEstado_dispatchExterno(soporte,acuse);
    }
    return r;
}
