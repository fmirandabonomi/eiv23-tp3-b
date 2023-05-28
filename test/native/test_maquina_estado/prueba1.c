#include "inc/prueba1.h"
#include "inc/mensajes.h"
/* Prueba1 */

#include <me_impl/maquina_estado_impl.h>
#include <me_impl/me_estado_impl.h>
#include <unity.h>
#include <stddef.h>
#include <malloc.h>

typedef struct Prueba1{
    Maquina maquina[1];
    struct{
        Estado A[1];
    }estados[1];
}Prueba1;

static ResultadoEvento Prueba1_EstadoA_fEstado(Estado *self,ISoporteEstado *soporte,Evento e);

static ResultadoEvento Prueba1_EstadoA_fEstado(Estado *self,ISoporteEstado *soporte,Evento e){
    ResultadoEvento r={0};
    Evento acuse = {.mensaje=MSG_EVENTO_PROCESADO,.param.uVal = (unsigned)'A' << 8 | e.mensaje};
    switch(e.mensaje){
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

Prueba1 *Prueba1_new(IColaEventos *colaEventos,IDespachadorEvento *despachador,IPilaEstados *pilaEstados,ITimer *timer){
    Prueba1 *r;
    r = malloc(sizeof(*r));
    TEST_ASSERT_NOT_NULL(r);
    Estado_init(r->estados->A,NULL,Prueba1_EstadoA_fEstado);
    Maquina_init(r->maquina,r->estados->A,colaEventos,despachador,pilaEstados,timer);
    return r;
}

Maquina *Prueba1_asMaquina(Prueba1 *self){
    return self->maquina;
}

void Prueba1_delete(Prueba1 *self){
    Maquina_finalize(self->maquina);
    free(self);
}