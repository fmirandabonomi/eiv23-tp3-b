#include <me_impl/me_estado_impl.h>


void Estado_init(Estado *self, Estado *padre,Estado_FnProcesaEvento fnEstado){
    self->padre = padre;
    if (padre){
        self->nivel = Estado_getNivel(padre) + 1;
    }else{
        self->nivel = 0;
    }
    self->procesaEvento=fnEstado;
}

NivelEstado Estado_getNivel(Estado *self){
    return self->nivel;
}

Estado *Estado_getPadre(Estado *self){
    return self->padre;
}

ResultadoEvento Estado_procesaEvento(Estado *self, IReceptorEvento *ctx,Evento e){
    ResultadoEvento r;
    r = self->procesaEvento(self,ctx,e);
    while(RES_EVENTO_IGNORADO == r.codigo && Estado_getNivel(self)){
        self = Estado_getPadre(self);
        r = self->procesaEvento(self,ctx,e);
    }
    return r;
}

