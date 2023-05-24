#ifndef I_TIMER_H
#define I_TIMER_H
#include <i_action.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Realiza acciones temporizadas
 * 
 */
typedef struct ITimer ITimer;

/**
 * @brief Tiempo en milisegundos
 * 
 */
typedef uint32_t Tiempo_ms;

/**
 * @brief Tabla de funciones virtuales para una implementación
 * de la interfaz @ref ITimer
 * 
 */
typedef struct ITimer_VT ITimer_VT;


struct ITimer_VT{
    bool (* const setTimeout)(ITimer *self,IAction *accion, Tiempo_ms tiempo, Tiempo_ms periodo);
    bool (* const delTimeout)(ITimer *self,IAction *accion);
};

struct ITimer{
    ITimer_VT const * _vptr;
};

/**
 * @brief Configura una acción temporizada o cambia la configuración de una acción
 * anteriormente configurada. La acción es llamada inicialmente cuando transcurre
 * el tiempo especificado por `tiempo` y luego periódicamente con el periodo 
 * especificado por `periodo`.
 * 
 * @param self Este objeto ITimer
 * @param accion Accion a realizar. Cada temporización es identificada por su acción,
 * que debe ser única en el mismo ITimer. Si es NULL la llamada falla siempre.
 * @param tiempo Tiempo en que es llamada la acción por primera vez. Si `tiempo` y 
 * `periodo` son cero entonces se cancela la temporización asociada a `accion` si
 * existiese.
 * @param periodo Periodo de repetición. Si tiempo es 0 y `periodo` es distinto de cero
 * entonces tiempo toma el valor de repetición
 * @return true Acción programada con exito
 * @return false No pudo programarse la acción
 */
inline bool ITimer_setTimeout(ITimer *self,IAction *accion, Tiempo_ms tiempo, Tiempo_ms periodo){
    return self->_vptr->setTimeout(self,accion,tiempo,periodo);
}
/**
 * @brief Elimina la temporización asociada a la acción `accion`.
 * 
 * @param self Este objeto ITimer
 * @param accion Acción cuya temporización asociada se desea eliminar.
 * @return true Temporización eliminada.
 * @return false La temporización no estaba programada.
 */
inline bool ITimer_delTimeout(ITimer *self,IAction *accion){
    return self->_vptr->delTimeout(self,accion);
}

#endif
