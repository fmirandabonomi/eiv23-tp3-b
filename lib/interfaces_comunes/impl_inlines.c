#include <i_action.h>
#include <i_timer.h>

extern inline void IAction_execute(IAction *self);
extern inline bool ITimer_setTimeout(ITimer *self,IAction *accion, Tiempo_ms tiempo, Tiempo_ms periodo);
extern inline bool ITimer_delTimeout(ITimer *self,IAction *accion);