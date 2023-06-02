#include "controlador_luz.h"
#include "accion_despacha_evento.h"
#include <maquina_estado_impl.h>

static Resultado estadoApagado(Maquina *ctx,Evento evento);
static Resultado estadoEncendido(Maquina *ctx,Evento evento);

void ControladorLuz_init(ControladorLuz *self, uint32_t tiempoOn, SoporteControlador *soporte){

    Maquina_init(&self->maquina,estadoApagado);
    self->tiempoOn = tiempoOn;
    self->soporte = soporte;
    SoporteControlador_asociaControlador(soporte,&self->maquina);
}

Maquina * ControladorLuz_asMaquina(ControladorLuz *self){
    return &self->maquina;
}

static Resultado estadoApagado(Maquina *ctx,Evento evento){
    ControladorLuz *self = (ControladorLuz*)ctx;
    Resultado r = {0};
    switch (evento){
    case EV_RESET:
        SoporteControlador_luzOff(self->soporte);
        r.codigo = RES_PROCESADO;
    break; case EV_BOTON_PULSADO:
        if(SoporteControlador_setTimeout(self->soporte,self->tiempoOn,EV_TIMEOUT)){
            SoporteControlador_luzOn(self->soporte);
            r.codigo = RES_TRANSICION;
            r.nuevoEstado = estadoEncendido;
        }else{
            r.codigo = RES_IGNORADO;
        }
    break;default:
        r.codigo = RES_IGNORADO;
    break;
    }
    return r;
}
static Resultado estadoEncendido(Maquina *ctx,Evento evento){
    ControladorLuz *self = (ControladorLuz*)ctx;
    Resultado r = {0};
    switch (evento){
    break; case EV_TIMEOUT:
        SoporteControlador_luzOff(self->soporte);
        r.codigo = RES_TRANSICION;
        r.nuevoEstado = estadoApagado;
    break;default:
        r.codigo = RES_IGNORADO;
    break;
    }
    return r;
}
