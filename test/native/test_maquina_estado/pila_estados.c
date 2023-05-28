#include "inc/pila_estados.h"
#include <unity.h>

#include <malloc.h>

/* PilaEstados */


static bool PilaEstados_push(PilaEstados *self,Estado *estado);
static Estado* PilaEstados_pop(PilaEstados *self);
static bool PilaEstados_qEmpty(PilaEstados *self);

static IPilaEstados_VT const pilaEstados_VT = {
    .push =(bool(*)(IPilaEstados*,Estado*)) PilaEstados_push,
    .pop = (Estado*(*)(IPilaEstados*)) PilaEstados_pop,
    .qEmpty = (bool(*)(IPilaEstados*)) PilaEstados_qEmpty
};

struct PilaEstados{
    IPilaEstados iPilaEstados;
    size_t maxEstados;
    size_t primerLibre;
    Estado *estados[];
};

/* Interfaz pública */

PilaEstados * PilaEstados_new(size_t maxEstados){
    TEST_ASSERT_GREATER_THAN_size_t(0,maxEstados);
    PilaEstados *r;
    size_t sz = sizeof(*r) + maxEstados*sizeof(r->estados[0]);
    r = malloc(sz);
    TEST_ASSERT_NOT_NULL(r);
    r->iPilaEstados._vptr = &pilaEstados_VT;
    r->maxEstados = maxEstados;
    r->primerLibre = 0;
    return r;
}

IPilaEstados *PilaEstados_asIPilaEstados(PilaEstados *self){
    return &self->iPilaEstados;
}

size_t PilaEstados_getNivelesUsados(PilaEstados *self){
    return self->primerLibre;
}

void PilaEstados_delete(PilaEstados *self){
    free(self);
}
/* Funciones privadas */

static bool PilaEstados_push(PilaEstados *self,Estado *estado){
    bool r=false;
    size_t const n = self->primerLibre;
    if(n<self->maxEstados){
        self->estados[n]=estado;
        self->primerLibre = n+1;
        r = true;
    }
    return r;
}
static Estado* PilaEstados_pop(PilaEstados *self){
    Estado *r = NULL;
    size_t const n = self->primerLibre;
    if (n){
        r = self->estados[n-1];
        self->primerLibre = n-1;
    }
    return r;
}
static bool PilaEstados_qEmpty(PilaEstados *self){
    return !(self->primerLibre);
}
