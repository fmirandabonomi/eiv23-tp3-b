#include <pulsador.h>

void Pulsador_init(Pulsador *self, SP_HPin pin,bool nivelPresionado,uint8_t histeresis, IAccion *accionPulsado){
    self->histeresis=histeresis;
    self->pin=pin;
    self->nivelPresionado=nivelPresionado;
    self->accionPulsado=accionPulsado;

    if (1 == nivelPresionado){
        SP_Pin_setModo(pin,SP_PIN_ENTRADA_PULLDN);
        self->acumulador = 0;
    }else{
        SP_Pin_setModo(pin,SP_PIN_ENTRADA_PULLUP);
        self->acumulador = self->histeresis;
    }
    self->t0 = SP_Tiempo_getMilisegundos();
    self->estadoPresionado = false;
}

static void Pulsador__procesaCambioNivel(Pulsador *self){
    bool const valido =    (self->acumulador == self->histeresis)
                        || (self->acumulador == 0);
    bool const presionado = (bool)(self->acumulador) == self->nivelPresionado;
    
    if (valido && (presionado != self->estadoPresionado)){
        self->estadoPresionado = presionado;
        if (!presionado && self->accionPulsado)
            IAccion_ejecuta(self->accionPulsado);
    }
}

void Pulsador_procesa(Pulsador *self){
    uint32_t const t = SP_Tiempo_getMilisegundos();
    if(t!=self->t0){
        self->t0 = t;
        bool const nivel = SP_Pin_read(self->pin);
        if (nivel && (self->acumulador < self->histeresis)){
            ++self->acumulador;
            Pulsador__procesaCambioNivel(self);
        }else if(!nivel && (self->acumulador>0)){
            --self->acumulador;
            Pulsador__procesaCambioNivel(self);
        }
    }
}

bool Pulsador_qPresionado(Pulsador *self){
    return self->estadoPresionado;
}
