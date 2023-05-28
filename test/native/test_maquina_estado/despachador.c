#include "inc/despachador.h"
#include <unity.h>
#include <malloc.h>

/* Soporte: Despachador */


static bool Despachador_addReceptor(Despachador * self,IReceptorEvento *rx);
static bool Despachador_remReceptor(Despachador * self,IReceptorEvento *rx);
static bool Despachador_dispatch(Despachador *self, Evento evento);

static IDespachadorEvento_VT const despachadorEvento_VT = {
    .addReceptor=(bool(*)(IDespachadorEvento *,IReceptorEvento*)) Despachador_addReceptor,
    .remReceptor=(bool(*)(IDespachadorEvento *,IReceptorEvento*)) Despachador_remReceptor,
    .dispatch=(bool(*)(IDespachadorEvento *,Evento)) Despachador_dispatch
};

struct Despachador{
    IDespachadorEvento iDespachadorEvento;
    size_t maxReceptores;
    size_t numReceptores;
    IReceptorEvento *receptores[];
};

/* Interfaz pública */
Despachador * Despachador_new(size_t maxReceptores){
    TEST_ASSERT_GREATER_THAN_size_t(0,maxReceptores);
    Despachador *r;
    size_t sz = sizeof(*r) + maxReceptores * sizeof(r->receptores[0]);
    r = malloc(sz);
    TEST_ASSERT_NOT_NULL(r);
    r->iDespachadorEvento._vptr = &despachadorEvento_VT;
    r->maxReceptores = maxReceptores;
    r->numReceptores = 0;
    return r;
}

IDespachadorEvento * Despachador_asIDespachadorEvento(Despachador *self){
    return &self->iDespachadorEvento;
}

void Despachador_delete(Despachador *self){
    free(self);
}

/* funciones privadas */
static size_t Despachador__indiceReceptor(Despachador *self,IReceptorEvento *rx){
    size_t j;
    for(j=0;j<self->numReceptores;++j){
        IReceptorEvento *const rx_j = self->receptores[j];
        if (rx_j == rx)
            break;
    }
    return j;
}

static bool Despachador_addReceptor(Despachador * self,IReceptorEvento *rx){
    bool r=false;
    size_t i = Despachador__indiceReceptor(self,rx);
    if (i < self->maxReceptores){
        if(i >= self->numReceptores) 
            ++self->numReceptores;
        self->receptores[i]=rx;
        r=true;
    }
    return r;
}

static bool Despachador_remReceptor(Despachador * self,IReceptorEvento *rx){
    bool r=false;
    size_t i = Despachador__indiceReceptor(self,rx);
    size_t n = self->numReceptores;
    if (i<n){
        self->receptores[i] = self->receptores[n-1];
        self->numReceptores = n-1;
        r = true;
    }
    return r;
}

static bool Despachador_dispatch(Despachador *self, Evento evento){
    bool r = false;
    for(size_t i=0;i<self->numReceptores;++i){
        IReceptorEvento *const rx = self->receptores[i];
        bool const r_i = IReceptorEvento_dispatch(rx,evento);
        r = r || r_i;
    }
    return r;
}
