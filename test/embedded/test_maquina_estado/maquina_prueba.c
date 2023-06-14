#include "maquina_prueba.h"
#include "maquina_estado_impl.h"

static Resultado estadoA(Maquina *contexto,Evento evento);
static Resultado estadoB(Maquina *contexto,Evento evento);
static Resultado estadoC(Maquina *contexto,Evento evento);

void MaquinaPrueba_init(Maquina *self){
    Maquina_init(self,estadoA);
}


static Resultado estadoA(Maquina *contexto,Evento evento){
    (void)contexto;
    Resultado resultado = {.codigo=RES_PROCESADO};
    switch(evento){
    case EV_RESET:
        accion0();
    break;case EV_E1:
        accion1();
    break;case EV_E2:
        accion2();
        resultado.codigo = RES_TRANSICION;
        resultado.nuevoEstado  = estadoB;
    break;case EV_E7:
        accion7();
        resultado.codigo = RES_TRANSICION;
        resultado.nuevoEstado  = estadoC;
    break;default:
        resultado.codigo = RES_IGNORADO;
    }
    return resultado;
}
static Resultado estadoB(Maquina *contexto,Evento evento){
    (void)contexto;
    Resultado resultado = {.codigo=RES_PROCESADO};
    switch(evento){
    case EV_E3:
        accion3();
    break;case EV_E4:
        accion4();
        resultado.codigo = RES_TRANSICION;
        resultado.nuevoEstado  = estadoC;
    break;case EV_E9:
        accion9();
        resultado.codigo = RES_TRANSICION;
        resultado.nuevoEstado  = estadoA;
    break;default:
        resultado.codigo = RES_IGNORADO;
    }
    return resultado;
}

static Resultado estadoC(Maquina *contexto,Evento evento){
    Resultado resultado = {.codigo=RES_PROCESADO};
    switch(evento){
    case EV_E5:
        accion5(contexto);
    break;case EV_E6:
        accion6();
        resultado.codigo = RES_TRANSICION;
        resultado.nuevoEstado  = estadoA;
    break;case EV_E8:
        accion8();
        resultado.codigo = RES_TRANSICION;
        resultado.nuevoEstado  = estadoB;
    break;default:
        resultado.codigo = RES_IGNORADO;
    }
    return resultado;
}