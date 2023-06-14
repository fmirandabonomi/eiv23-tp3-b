#include <despacho_retardado.h>
#include <soporte_placa.h>

void DespachoRetardado_init(DespachoRetardado *self){
    *self = (DespachoRetardado){0};
}

void DespachoRetardado_programa(DespachoRetardado *self, Maquina *const destino, Evento const evento, uint32_t const tiempoMilisegundos){
    size_t i;
    size_t const N = self->numDespachosEnEspera;
    struct DespachoEnEspera *const d = self->despachosEnEspera;
    for(i=0;i<N;++i){
        if((destino == d[i].destino) && (evento == d[i].evento)) break;
    }
    if (i<MAX_DESPACHOS_RETARDADOS_ACTIVOS){
        d[i].cuenta  = tiempoMilisegundos;
        d[i].destino = destino;
        d[i].evento  = evento;
        if (N == i)
            self->numDespachosEnEspera = N+1;
    }else{
        Maquina_despacha(destino,evento);
    }
}



void DespachoRetardado_procesa(DespachoRetardado * self){
    uint32_t const t = SP_Tiempo_getMilisegundos();
    uint32_t const dt = t-self->t0;
    if (dt){
        self->t0 = t;

        size_t N = self->numDespachosEnEspera;
        struct DespachoEnEspera *const d = self->despachosEnEspera;
        for(size_t i=0;i<N;++i){
            if(d[i].cuenta > dt){
                d[i].cuenta -= dt;
            }else if (d[i].cuenta){
                d[i].cuenta = 0;
                Maquina_despacha(d[i].destino,d[i].evento);
            }
            if(!d[i].cuenta){ // Cuenta cumplida, deshecha el elemento y vuelve a procesar el lugar
                d[i] = d[N-1];
                --i;
                --N;
            }
        }
        self->numDespachosEnEspera = N;
    }
}
