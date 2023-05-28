#include "inc/m_timer.h"
#include <unity.h>
#include <malloc.h>

/* MTimer */
static bool MTimer_setTimeout(MTimer *self,IAction *action,Tiempo_ms tiempo,Tiempo_ms periodo);
static bool MTimer_delTimeour(MTimer *self,IAction *action);


static ITimer_VT const timer_VT = {
    .setTimeout = (bool (*)(ITimer *,IAction *,Tiempo_ms,Tiempo_ms)) MTimer_setTimeout,
    .delTimeout = (bool (*)(ITimer *,IAction *)) MTimer_delTimeour
};

typedef struct MTimer_Descritor{
    IAction * accion;
    Tiempo_ms restante;
    Tiempo_ms periodo;
}MTimer_Descritor;

struct MTimer{
    ITimer iTimer;
    size_t maxTimeouts;
    size_t numTimeouts;
    MTimer_Descritor timeouts[];
};


/* Funciones privadas */
static size_t MTimer__indiceTimeout(MTimer *self,IAction *action){
    size_t i;
    for(i=0;i<self->numTimeouts;++i){
        IAction *const action_i = self->timeouts[i].accion;
        if (action_i == action)
            break;
    }
    return i;
}
static void MTimer__setDescriptor(MTimer *self,size_t pos,IAction *action,Tiempo_ms tiempo,Tiempo_ms periodo){
    if (pos >= self->numTimeouts)
        ++(self->numTimeouts);
    self->timeouts[pos] = (MTimer_Descritor){
        .accion = action,
        .restante = tiempo,
        .periodo = periodo
    };
}
static void MTimer__delDescriptor(MTimer *self,size_t pos){
    size_t const n=self->numTimeouts;
    if (pos < n){
        self->timeouts[pos] = self->timeouts[n-1];
        self->numTimeouts = n-1;
    }
}
static bool MTimer_setTimeout(MTimer *self,IAction *action,Tiempo_ms tiempo,Tiempo_ms periodo){
    bool r=false;
    size_t i = MTimer__indiceTimeout(self,action);
    if(i < self->maxTimeouts){
        if (0 == tiempo)
            tiempo = periodo;
        if(tiempo)
            MTimer__setDescriptor(self,i,action,tiempo,periodo);
        else
            MTimer__delDescriptor(self,i);
        r=true;
    }
    return r;
}
static bool MTimer_delTimeour(MTimer *self,IAction *action){
    bool r=false;
    size_t i = MTimer__indiceTimeout(self,action);
    if(i < self->numTimeouts){
        MTimer__delDescriptor(self,i);
        r = true;
    }
    return r;
}

static Tiempo_ms MTimer__getMinTiempo(MTimer *self){
    Tiempo_ms minTiempo=0;
    if(self->numTimeouts > 0)
        minTiempo = self->timeouts[0].restante;
    for(size_t i=1;i<self->numTimeouts;++i){
        Tiempo_ms t_i = self->timeouts[i].restante;
        if (t_i<minTiempo)
            minTiempo = t_i;
    }
    return minTiempo;
}
static bool MTimer__actualizaTimeouts(MTimer *self, Tiempo_ms tiempo)
{
    bool ejecutado = false;
    for(size_t i=0;i<self->numTimeouts;++i){
        MTimer_Descritor *const d=self->timeouts + i;
        if(d->restante > tiempo){
            d->restante -= tiempo;
        }else{
            d->restante = d->periodo;
            IAction_execute(d->accion);
            ejecutado = true;
        }
    }
    return ejecutado;
}
static void MTimer__podaTimeouts(MTimer *self){
    size_t n = self->numTimeouts;
    for(size_t i=0;i<n;++i){
        while(!(self->timeouts[i].restante) && (i < (n-1)) ){
            self->timeouts[i] = self->timeouts[n-1];
            --n;
        }
    }
    self->numTimeouts = n;
}

/* interfaz pública */

MTimer * MTimer_new(size_t maxTimeouts){
    TEST_ASSERT_GREATER_THAN_size_t(0,maxTimeouts);
    MTimer *r;
    size_t sz = sizeof(*r) + maxTimeouts * sizeof(r->timeouts[0]);
    r = malloc(sz);
    TEST_ASSERT_NOT_NULL(r);
    r->iTimer._vptr = &timer_VT;
    r->maxTimeouts = maxTimeouts;
    r->numTimeouts = 0;
    return r;
}


void MTimer_avanzaTiempo(MTimer *self, Tiempo_ms tiempo){
    bool const ejecutado = MTimer__actualizaTimeouts(self,tiempo);
    if(ejecutado){
        MTimer__podaTimeouts(self);
    }
}
Tiempo_ms MTimer_avanzaAlSiguienteTimeout(MTimer *self){
    Tiempo_ms const r = MTimer__getMinTiempo(self);
    MTimer_avanzaTiempo(self,r);
    return r;
}

ITimer * MTimer_asITimer(MTimer *self){
    return &self->iTimer;
}

void MTimer_delete(MTimer *self){
    free(self);
}