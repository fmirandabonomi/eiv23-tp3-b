#include "controlador_luz.h"
#include "accion_despacha_evento.h"
#include <maquina_estado_impl.h>

static Resultado estadoApagado(Maquina *ctx,Evento evento);
static Resultado estadoEncendido(Maquina *ctx,Evento evento);

void ControladorLuz_init(ControladorLuz *self, uint32_t tiempoOn, ControladorLuz_Acciones const *acciones){

    Maquina_init(&self->maquina,estadoApagado);
    self->tiempoOn = tiempoOn;
    self->acciones = acciones;
}

Maquina * ControladorLuz_asMaquina(ControladorLuz *self){
    return &self->maquina;
}

static Resultado estadoApagado(Maquina *ctx,Evento evento){
    ControladorLuz *self = (ControladorLuz*)ctx;
    Resultado r = {0};
    switch (evento){
    case EV_RESET:
        self->acciones->apagaLuz();
        r.codigo = RES_PROCESADO;
    break; case EV_BOTON_PULSADO:
        self->acciones->enciendeLuz();
        self->acciones->despachaLuegoDeTiempo(ctx,EV_TIMEOUT,self->tiempoOn);
        r.codigo = RES_TRANSICION;
        r.nuevoEstado = estadoEncendido;
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
        self->acciones->apagaLuz();
        r.codigo = RES_TRANSICION;
        r.nuevoEstado = estadoApagado;
    break;default:
        r.codigo = RES_IGNORADO;
    break;
    }
    return r;
}
