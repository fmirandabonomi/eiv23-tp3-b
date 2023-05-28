#include <me_pub/me_i_cola_eventos.h>
#include <me_pub/me_i_receptor_evento.h>
#include <me_pub/me_i_despachador_evento.h>
#include <me_pub/me_i_soporte_estado.h>
#include <me_pub/me_i_pila_estados.h>
/* Versión no inline de las funciones inline */

extern inline bool IColaEventos_dispatch(IColaEventos *self, Evento e);
extern inline bool IColaEventos_queryDisponible(IColaEventos *self);
extern inline Evento IColaEventos_getSiguiente(IColaEventos *self);
extern inline bool IReceptorEvento_dispatch(IReceptorEvento *self,Evento e);
extern inline bool IDespachadorEvento_addReceptor(IDespachadorEvento *self, IReceptorEvento *rx);
extern inline bool IDespachadorEvento_remReceptor(IDespachadorEvento *self, IReceptorEvento *rx);
extern inline bool IDespachadorEvento_dispatch(IDespachadorEvento *self, Evento e);
extern inline bool ISoporteEstado_dispatchInterno(ISoporteEstado *self,Evento e);
extern inline bool ISoporteEstado_dispatchExterno(ISoporteEstado *self,Evento e);
extern inline ITimer * ISoporteEstado_getTimer(ISoporteEstado *self);
extern inline bool IPilaEstados_push(IPilaEstados *self,Estado *estado);
extern inline Estado * IPilaEstados_pop(IPilaEstados *self);
extern inline bool IPilaEstados_qEmpty(IPilaEstados *self);
