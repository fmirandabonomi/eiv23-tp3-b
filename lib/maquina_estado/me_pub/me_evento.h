#ifndef ME_EVENTO_H
#define ME_EVENTO_H
#include <stdint.h>

/**
 * @brief Tipo de dato que describe un evento. Debe pasarse
 * por valor.
 * 
 */
typedef struct Evento Evento;

/**
 * @brief Describe la naturaleza de un evento
 * 
 */
typedef unsigned Mensaje;
/**
 * @brief Contiene información adicional sobre un evento
 * 
 */
typedef union Param Param;

enum MensajeSistema{
    /**
     * @brief Mensaje nulo, no tiene efecto, debe ser ignorado
     * 
     */
    MSG_NULO,
    /**
     * @brief Mensaje de inicialización. Al ser recibido por una
     * máquina de estado produce su inicialización. Es recibido por
     * el nuevo estado activo luego de @ref MSG_ENTRADA
     * 
     */
    MSG_INICIALIZA,
    /**
     * @brief Mensaje que permite ejecutar las acciones de entrada a
     * un estado. Cuando hay un cambio de estado reciben este mensaje
     * todos los descendientes del primer ancestro común a los estados
     * saliente y entrante hasta llegar al nuevo estado activo. Luego
     * el nuevo estado activo recibe el mensaje @ref MSG_INICIALIZA
     * 
     * @warning Este mensaje puede ser ignorado o aceptado pero no puede
     * dar lugar a una solicitud de cambio de estado. Si es necesario
     * solicitar un cambio de estado (estado compuesto) eso corresponde
     * al proceso de @ref MSG_ENTRADA
     * 
     */
    MSG_ENTRADA,
    /**
     * @brief Mensaje que permite ejecutar las acciones de salida de
     * un estado. Cuando hay un cambio de estado reciben este mensaje
     * el estado saliente y todos sus ancestros que no son comúnes con
     * los del estado entrante.
     * 
     * @warning Este mensaje puede ser ignorado o aceptado pero no puede
     * dar lugar a una solicitud de cambio de estado. No tiene sentido en
     * este contexto dicha solicitud. El nuevo estado activo puede solicitar
     * cambio de estado al recibir @ref MSG_INICIALIZA
     * 
     */
    MSG_SALIDA,
    /**
     * @brief Primer mensaje definido por el usuario. Este código y todos
     * los códigos subsiguientes están disponibles para uso de la aplicación
     * 
     */
    MSG_USUARIO
};

union Param{
    /**
     * @brief Parámetro entero con signo
     * 
     */
    intptr_t iVal;
    /**
     * @brief Parámetro entero sin signo
     * 
     */
    uintptr_t uVal;
    /**
     * @brief Parámetro puntero genérico
     * 
     */
    void *pVal;
};

struct Evento {
    /**
     * @brief Determina el tipo de evento ocurrido
     * 
     */
    Mensaje mensaje;
    /**
     * @brief Parámetros del evento. Su interpretación depende
     * del mensaje.
     * 
     */
    Param param;
};

#endif