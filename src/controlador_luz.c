#include "controlador_luz.h"
#include <maquina_estado_impl.h>

static Resultado estadoApagado(Maquina *ctx,Evento evento);
static Resultado estadoEncendido(Maquina *ctx,Evento evento);

void ControladorLuz_init(ControladorLuz *self,uint32_t tiempoOn,SP_HPin pinLuz,bool nivelLuzOn,DespachoRetardado *despachoRetardado){

    Maquina_init(&self->maquina,estadoApagado);
    self->tiempoOn = tiempoOn;
    self->interfazLuz.pin=pinLuz;
    self->interfazLuz.nivelOn=nivelLuzOn;
    self->despachoRetardado = despachoRetardado;
}

Maquina * ControladorLuz_asMaquina(ControladorLuz *self){
    return &self->maquina;
}

static void ControladorLuz__apagaLuz(ControladorLuz *self){
    SP_Pin_write(self->interfazLuz.pin,!self->interfazLuz.nivelOn);
}

static void ControladorLuz__enciendeLuz(ControladorLuz *self){
    SP_Pin_write(self->interfazLuz.pin,self->interfazLuz.nivelOn);
}

static Resultado estadoApagado(Maquina *ctx,Evento evento){
    ControladorLuz *self = (ControladorLuz*)ctx;
    Resultado r = {0};
    switch (evento){
    case EV_RESET:
        SP_Pin_setModo(self->interfazLuz.pin,SP_PIN_ENTRADA);
        ControladorLuz__apagaLuz(self);
        SP_Pin_setModo(self->interfazLuz.pin,SP_PIN_SALIDA);
        r.codigo = RES_PROCESADO;
    break; case EV_BOTON_PULSADO:
        ControladorLuz__enciendeLuz(self);
        DespachoRetardado_programa(self->despachoRetardado,ctx,EV_TIMEOUT,self->tiempoOn);
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
        ControladorLuz__apagaLuz(self);
        r.codigo = RES_TRANSICION;
        r.nuevoEstado = estadoApagado;
    break;default:
        r.codigo = RES_IGNORADO;
    break;
    }
    return r;
}
