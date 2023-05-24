#ifndef ME_ESTADO_IMPL_H
#define ME_ESTADO_IMPL_H
#include <me_pub/me_estado.h>

typedef ResultadoEvento (*Estado_FnProcesaEvento)(Estado *self,ISoporteEstado *ctx, Evento e);

struct Estado{
    Estado_FnProcesaEvento procesaEvento;
    Estado *padre;
    NivelEstado nivel;
};


void Estado_init(Estado *self, Estado *padre,Estado_FnProcesaEvento fnEstado);

#endif
