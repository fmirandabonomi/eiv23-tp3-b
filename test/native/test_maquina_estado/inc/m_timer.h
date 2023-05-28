#ifndef M_TIMER_H
#define M_TIMER_H
#include <i_timer.h>
#include <stddef.h>

typedef struct MTimer MTimer;

MTimer * MTimer_new(size_t maxTimeouts);
void MTimer_avanzaTiempo(MTimer *self, Tiempo_ms tiempo);
Tiempo_ms MTimer_avanzaAlSiguienteTimeout(MTimer *self);
ITimer * MTimer_asITimer(MTimer *self);
void MTimer_delete(MTimer *self);

#endif
