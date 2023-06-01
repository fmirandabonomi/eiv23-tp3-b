#include "maquina_estado_impl.h"
#include <stddef.h>
#include <stm32f1xx.h>

void Maquina_init(Maquina *self, Estado estadoInicial){
    self->estadoInicial = estadoInicial;
    self->estado = NULL;
    self->cola.lecturas = 0;
    self->cola.escrituras = 0;
    Maquina_despacha(self, EV_RESET);
}


static bool Maquina__pEspacioDisponibleEnCola(Maquina const *self){
    return (self->cola.escrituras - self->cola.lecturas) < MAX_EV_COLA;
}

static void Maquina__insertaEventoEnCola(Maquina *self, Evento evento){
    unsigned const posicion = (self->cola.escrituras) % MAX_EV_COLA;
    self->cola.eventos[posicion] = evento;
    self->cola.escrituras++;
    
}

bool Maquina_despacha(Maquina *self, Evento evento){
    bool hecho = false;
    __disable_irq();
    if((EV_NULO != evento) && Maquina__pEspacioDisponibleEnCola(self)){
        Maquina__insertaEventoEnCola(self,evento);
        hecho = true;
    }
    __enable_irq();
    return hecho;
}

static bool Maquina__pEventosDisponiblesEnCola(Maquina const *self){
    return self->cola.escrituras != self->cola.lecturas;
}

static Evento Maquina_siguienteEvento(Maquina *self){
    Evento evento = EV_NULO;
    if(Maquina__pEventosDisponiblesEnCola(self)){
        unsigned const posicion = (self->cola.lecturas) % MAX_EV_COLA;
        evento = self->cola.eventos[posicion];
        self->cola.lecturas++;
    }
    return evento;
}



bool Maquina_procesa(Maquina *self){
    Evento const evento = Maquina_siguienteEvento(self);
    bool procesado = false;
    if (evento != EV_NULO){
        procesado = true;
        if (evento == EV_RESET || !self->estado){
            self->estado = self->estadoInicial;
        }
        
        Resultado resultado = self->estado(self,evento);
        
        if (resultado.codigo == RES_TRANSICION){
            self->estado = resultado.param;
        }
    }
    return procesado;
}
