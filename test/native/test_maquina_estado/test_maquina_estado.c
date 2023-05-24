#include <maquina_estado.h>
#include <unity.h>
#include <malloc.h>
#include <string.h>

typedef struct MTimer MTimer;

static bool MTimer_setTimeout(MTimer *self,IAction *action,Tiempo_ms tiempo,Tiempo_ms periodo);
static bool MTimer_delTimeour(MTimer *self,IAction *action);
static void MTimer_avanzaTiempo(MTimer *self, Tiempo_ms tiempo);
static Tiempo_ms MTimer_avanzaAlSiguienteTimeout(MTimer *self);

void setUp(void){

}
void tearDown(void){

}

static IColaEventos * Factory_createColaEventos(size_t maxEventos);
static IDespachadorEvento * Factory_createDespachadorEvento(size_t maxReceptores);
static IPilaEstados * Factory_createPilaEstados(size_t maxEstados);
static MTimer * Factory_createMTimer(size_t maxTimeouts);
static void Factory_free(void *objeto);


/* Pruebas */


/* Punto de entrada*/
int main(void)
{
    UNITY_BEGIN();

    return UNITY_END();
}


/* Soporte: ColaEventos */
typedef struct ColaEventos ColaEventos;

static bool ColaEventos_dispatch(ColaEventos *self,Evento evento);
static bool ColaEventos_queryDisponible(ColaEventos *self);
static Evento ColaEventos_getSiguiente(ColaEventos *self);

IColaEventos_VT const colaEventos_VT = {
    .dispatch = (bool(*)(IColaEventos*,Evento)) ColaEventos_dispatch,
    .queryDisponible = (bool(*)(IColaEventos*)) ColaEventos_queryDisponible,
    .getSiguiente = (bool(*)(IColaEventos*)) ColaEventos_getSiguiente
};

struct ColaEventos{
    IColaEventos iColaEventos;
    size_t maxEventos;
    size_t escrituras;
    size_t lecturas;
    Evento eventos[];
};

static IColaEventos *Factory_createColaEventos(size_t maxEventos){
    TEST_ASSERT_GREATER_THAN_size_t(0,maxEventos);
    ColaEventos *r;
    size_t const sz = sizeof(*r) + maxEventos*sizeof(r->eventos[0]);
    r = malloc(sz);
    TEST_ASSERT_NOT_NULL(r);
    r->iColaEventos._vptr=&colaEventos_VT;
    r->maxEventos = maxEventos;
    r->lecturas = 0;
    r->escrituras = 0;
    return r;
}

static bool ColaEventos__qCapacidadDisponible(ColaEventos *self){
    return (self->escrituras - self->lecturas) < self->maxEventos;
}
static void ColaEventos__inserta(ColaEventos *self,Evento evento){
    size_t const i = self->escrituras;
    ++self->escrituras;
    self->eventos[i%self->maxEventos]=evento;
}
static bool ColaEventos_dispatch(ColaEventos *self,Evento evento){
    if (ColaEventos__qCapacidadDisponible(self)){
        ColaEventos__inserta(self,evento);
    }
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

/* Soporte: Despachador */

typedef struct Despachador Despachador;

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

static IDespachadorEvento * Factory_createDespachadorEvento(size_t maxReceptores){
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


/* PilaEstados */

typedef struct PilaEstados PilaEstados;

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

static IPilaEstados * Factory_createPilaEstados(size_t maxEstados){
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

/* MTimer */


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

static MTimer * Factory_createMTimer(size_t maxTimeouts){
    TEST_ASSERT_GREATER_THAN_size_t(0,maxTimeouts);
    MTimer *r;
    size_t sz = sizeof(*r) + maxTimeouts * sizeof(r->timeouts[0]);
    r = malloc(r);
    TEST_ASSERT_NOT_NULL(r);
    r->iTimer._vptr = &timer_VT;
    r->maxTimeouts = maxTimeouts;
    r->numTimeouts = 0;
}

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
    MTimer_Descritor *const desc = self->timeouts + pos;
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
    for(size_t i=0;i<self->numTimeouts;++i){
        Tiempo_ms t_i = self->timeouts[i].restante;
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
static void MTimer_avanzaTiempo(MTimer *self, Tiempo_ms tiempo){
    bool const ejecutado = MTimer__actualizaTimeouts(self,tiempo);
    if(ejecutado){
        MTimer__podaTimeouts(self);
    }
}
static Tiempo_ms MTimer_avanzaAlSiguienteTimeout(MTimer *self){
    Tiempo_ms const r = MTimer__getMinTiempo(self);
    MTimer_avanzaTiempo(self,r);
    return r;
}

/* Free */

static void Factory_free(void *objeto){
    free(objeto);
}