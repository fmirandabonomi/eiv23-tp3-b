#ifndef I_ACTION_H
#define I_ACTION_H

/**
 * @brief Objeto que representa una acción a ejecutar
 * 
 */
typedef struct IAction IAction;

struct IAction{
    void (*execute)(IAction *self);
};
/**
 * @brief Ejecuta la acción encapsulada
 * 
 * @param self Este objeto IAction
 */
inline void IAction_execute(IAction *self){
    self->execute(self);
}

#endif
