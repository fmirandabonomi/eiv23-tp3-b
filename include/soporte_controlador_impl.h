#ifndef SOPORTE_CONTROLADOR_IMPL_H
#define SOPORTE_CONTROLADOR_IMPL_H
#include <soporte_controlador.h>
#include <soporte_placa.h>
#include <accion_despacha_evento.h>

#ifndef SC_MAX_TIMEOUTS
#define SC_MAX_TIMEOUTS 2
#endif

struct SoporteControlador{
    AccionDespachaEvento despachadoresTimeout[2];
    SP_HPin pinLuz;
    bool nivelOn;
    Maquina *maquina;
};

void SoporteControlador_init(SoporteControlador *self, SP_HPin pinLuz, bool nivelOn);

#endif
