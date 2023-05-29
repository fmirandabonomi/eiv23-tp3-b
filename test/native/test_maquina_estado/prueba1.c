#include "maquinas_de_prueba_impl.h"

/* Prueba1 */
typedef struct Prueba1{
    Maquina maquina;
    Estado A;
}Prueba1;

static ResultadoEvento Prueba1_EstadoA_fEstado(Estado *self,ISoporteEstado *soporte,Evento evento);


Maquina *Prueba1_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer){
    Prueba1 *r;
    r = malloc(sizeof(*r));
    TEST_ASSERT_NOT_NULL(r);
    Estado_init(&r->A,NULL,Prueba1_EstadoA_fEstado);
    Maquina_init(&r->maquina,&r->A,colaEventos,despachador,pilaEstados,timer);
    return (Maquina*) r;
}

static ResultadoEvento Prueba1_EstadoA_fEstado(Estado *self,ISoporteEstado *soporte,Evento evento){
    ResultadoEvento r={0};
    Evento const acuse = EVENTO_ACUSE('A',evento.mensaje);
    switch(evento.mensaje){
                 case MSG_ENTRADA:
    /*FALLTHRU*/ case MSG_INICIALIZA:
    /*FALLTHRU*/ case MSG_SALIDA:
    /*FALLTHRU*/ case MSG_T1:
    /*FALLTHRU*/ case MSG_T2:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_EVENTO_PROCESADO;
    break; case MSG_T3:
        ISoporteEstado_dispatchExterno(soporte,acuse);
        r.codigo = RES_TRANSICION_EXTERNA;
        r.param  = self;
    break; default:
        r.codigo = RES_EVENTO_IGNORADO;
    }
    return r;
}
