#include <interf_impl/container_of.h>
#include "inc/cola_eventos.h"
#include <unity.h>
#include <malloc.h>
#include <stdio.h>
/* Soporte: ColaEventos */

static bool ColaEventos_dispatch(ColaEventos *self,Evento evento);
static bool ColaEventos_queryDisponible(ColaEventos *self);
static Evento ColaEventos_getSiguiente(ColaEventos *self);

IColaEventos_VT const colaEventos_VT = {
    .dispatch = (bool(*)(IColaEventos*,Evento)) ColaEventos_dispatch,
    .queryDisponible = (bool(*)(IColaEventos*)) ColaEventos_queryDisponible,
    .getSiguiente = (Evento(*)(IColaEventos*)) ColaEventos_getSiguiente
};

static bool ReceptorEvento_dispatch(IReceptorEvento *self,Evento evento);

struct ColaEventos{
    IColaEventos iColaEventos;
    IReceptorEvento iReceptorEvento;
    size_t maxEventos;
    size_t escrituras;
    size_t lecturas;
    Evento eventos[];
};


/* Interfaz pública */
ColaEventos *ColaEventos_new(size_t maxEventos){
    TEST_ASSERT_GREATER_THAN_size_t(0,maxEventos);
    ColaEventos *r;
    size_t const sz = sizeof(*r) + maxEventos*sizeof(r->eventos[0]);
    r = malloc(sz);
    TEST_ASSERT_NOT_NULL(r);
    r->iColaEventos._vptr=&colaEventos_VT;
    r->iReceptorEvento.dispatch = &ReceptorEvento_dispatch;
    r->maxEventos = maxEventos;
    r->lecturas = 0;
    r->escrituras = 0;
    return r;
}
IColaEventos * ColaEventos_asIColaEventos(ColaEventos *self){
    return &self->iColaEventos;
}
IReceptorEvento * ColaEventos_asIReceptorEvento(ColaEventos *self){
    return &self->iReceptorEvento;
}
size_t ColaEventos_numEventos(ColaEventos *self){
    return self->escrituras - self->lecturas;
}
void ColaEventos_clear(ColaEventos *self){
    self->lecturas=self->escrituras;
}
size_t ColaEventos_getEventos(ColaEventos *self,size_t maxEventos,Evento *eventos){
    size_t const Ne = ColaEventos_numEventos(self);
    size_t const N = (Ne>maxEventos) ? maxEventos : Ne;
    for (size_t i=0;i<N;++i){
        size_t const k = (self->lecturas+i)%self->maxEventos;
        eventos[i] = self->eventos[k];
    }
    return N;
}

void ColaEventos_delete(ColaEventos *self){
    free(self);
}
/* funciones privadas */
static bool ColaEventos__qCapacidadDisponible(ColaEventos *self){
    return (self->escrituras - self->lecturas) < self->maxEventos;
}
static void ColaEventos__inserta(ColaEventos *self,Evento evento){
    size_t const i = self->escrituras;
    ++self->escrituras;
    self->eventos[i%self->maxEventos]=evento;
}
static bool ColaEventos_dispatch(ColaEventos *self,Evento evento){
    bool r=false;
    if (ColaEventos__qCapacidadDisponible(self)){
        ColaEventos__inserta(self,evento);
        r=true;
    }
    return r;
}
static bool ColaEventos_queryDisponible(ColaEventos *self){
    return self->escrituras - self->lecturas;
}
static Evento ColaEventos__getPrimerEvento(ColaEventos *self){
    size_t const i = self->lecturas;
    ++self->lecturas;
    return self->eventos[i%self->lecturas];
}
static Evento ColaEventos_getSiguiente(ColaEventos *self){
    bool const disponible = ColaEventos_queryDisponible(self);
    TEST_ASSERT_TRUE(disponible);
    return ColaEventos__getPrimerEvento(self);
}

static bool ReceptorEvento_dispatch(IReceptorEvento *rxe,Evento evento){
    ColaEventos *self = container_of(rxe,ColaEventos,iReceptorEvento);
    return ColaEventos_dispatch(self,evento);
}