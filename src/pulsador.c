#include "pulsador.h"

void Pulsador_init(Pulsador *self, 
                   Maquina *destino,
                   Evento evento,
                   SP_HPin pin,
                   bool nivelActivo,
                   uint8_t histeresis)
{
    self->destino = destino;
    self->evento  = evento;
    self->estado.nivelAnterior = !nivelActivo;
    if (nivelActivo == false){
        self->estado.contador = self->parametros.histeresis;
        SP_Pin_setModo(pin,SP_PIN_ENTRADA_PULLUP);
    }else{
        self->estado.contador = 0;
        SP_Pin_setModo(pin,SP_PIN_ENTRADA_PULLDN);
    }
    self->parametros.pin = pin;
    self->parametros.nivelActivo = nivelActivo;
    self->parametros.histeresis = histeresis;
    self->t0 = SP_Tiempo_getMilisegundos();
}

void Pulsador_procesa(Pulsador *self){
    uint32_t const t = SP_Tiempo_getMilisegundos();
    if (self->t0 != t){
        self->t0 = t;
        bool const nivelPin = SP_Pin_read(self->parametros.pin);
        bool nivelFlitrado = self->estado.nivelAnterior;
        if (nivelPin && (self->estado.contador < self->parametros.histeresis)){
            uint8_t const nuevoContador = self->estado.contador + 1;
            if (nuevoContador == self->parametros.histeresis){
                nivelFlitrado = 1;
            }
            self->estado.contador = nuevoContador;
        }else if(!nivelPin && (self->estado.contador > 0)){
            uint8_t const nuevoContador = self->estado.contador - 1;
            if (!nuevoContador){
                nivelFlitrado = 0;
            }
            self->estado.contador = nuevoContador;
        }
        if (nivelFlitrado != self->estado.nivelAnterior){
            self->estado.nivelAnterior = nivelFlitrado;
            if (self->parametros.nivelActivo == nivelFlitrado){ // Pulsador presionado
                Maquina_despacha(self->destino,self->evento);
            }
        }
    }   
}