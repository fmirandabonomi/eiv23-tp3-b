#include "soporte_controlador_impl.h"
#include <soporte_placa.h>
#include <stddef.h>

void SoporteControlador_init(SoporteControlador *self, SP_HPin pinLuz, bool nivelOn){
    self->maquina = NULL;
    self->pinLuz = pinLuz;
    self->nivelOn = nivelOn;
    SP_Pin_setModo(pinLuz,SP_PIN_ENTRADA);
    SP_Pin_write(pinLuz,!nivelOn);
    SP_Pin_setModo(pinLuz,SP_PIN_SALIDA);
    for (size_t i=0;i<SC_MAX_TIMEOUTS;++i){
        AccionDespachaEvento_deInit(self->despachadoresTimeout+i);
    }
}

void SoporteControlador_asociaControlador(SoporteControlador *self, Maquina* controlador){
    self->maquina = controlador;
}

void SoporteControlador_luzOn(SoporteControlador *self){
    SP_Pin_write(self->pinLuz,self->nivelOn);
}
void SoporteControlador_luzOff(SoporteControlador *self){
    SP_Pin_write(self->pinLuz,!self->nivelOn);
}

static size_t SoporteControlador_indiceDespachador(SoporteControlador *self,Evento evento){
    size_t r;
    size_t primerLibre;
    bool libreHallado=false;
    for(r=0;r<SC_MAX_TIMEOUTS;++r){
        AccionDespachaEvento *const d = self->despachadoresTimeout+r;
        if (AccionDespachaEvento_getEstadoDespacho(d) == ED_NO_DESPACHADO)
        {
            if ((AccionDespachaEvento_getEvento(d) == evento)) break;
        }else if(!libreHallado){
            primerLibre = r;
            libreHallado =true;
        }
    }
    if (r>=SC_MAX_TIMEOUTS && libreHallado){
        r = primerLibre;
    }
    return r;
}

bool SoporteControlador_setTimeout(SoporteControlador *self, uint32_t tiempo, Evento evento){
    bool hecho = false;
    if (self->maquina){
        size_t i = SoporteControlador_indiceDespachador(self,evento);
        if (i < SC_MAX_TIMEOUTS){
            AccionDespachaEvento *const d = self->despachadoresTimeout + i; 
            AccionDespachaEvento_init(d,evento,self->maquina);
            hecho = SP_Tiempo_addTimeout(tiempo,AccionDespachaEvento_asIAccion(d));
        }
    }
    return hecho;
}
