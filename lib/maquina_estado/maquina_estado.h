#ifndef MAQUINA_ESTADO_H
#define MAQUINA_ESTADO_H

#include <stdbool.h>

#ifndef MAX_EV_COLA
#define MAX_EV_COLA 16 /* debe ser potencia de 2*/
#endif

#if MAX_EV_COLA & (MAX_EV_COLA - 1)
#error MAX_EV_COLA debe ser potencia de dos
#endif

typedef struct Maquina Maquina;
typedef unsigned Evento;
typedef struct Resultado Resultado;
typedef Resultado (*Estado)(Maquina* contexto, Evento evento);


enum EventoSistema{
    /**
     * @brief Significa que no hay eventos disponibles
     */
    EV_NULO,
    /**
     * @brief Inicia o reinicia la máquina de estado. Establece
     * como estado actual el estado inicial y luego envía el
     * evento de reset a dicho estado.
     * 
     */
    EV_RESET,
    /**
     * @brief Primer evento disponible para la aplicación
     */
    EV_USUARIO
};


/**
 * @brief Máquina de estado
 */
struct Maquina{
    struct {
        Evento eventos[MAX_EV_COLA];
        /**
         * @brief lecturas % MAX_EV_COLA : indice del proximo elemento a leer
         * SI lecturas != escrituras
         */
        unsigned lecturas;
        /**
         * @brief escrituras % MAX_EV_COLA : indice del proximo espacio libre 
         * SI (escrituras - lecturas) < MAX_EV_COLA
         */
        unsigned escrituras;
    }cola;
    Estado estadoInicial;
    Estado estadoActual;
};


/**
 * @brief Despacha un evento para posterior procesamiento
 * 
 * @param self Este objeto
 * @param evento Evento a despachar
 * @return true Evento despachado
 * @return false Falla al despachar evento
 */
bool Maquina_despacha(Maquina *self, Evento evento);

/**
 * @brief Procesa un evento disponible. Este método debe ser llamado
 * desde un solo punto del programa.
 * 
 * @param self Este objeto
 * @return true Evento procesado
 * @return false No había eventos disponibles
 */
bool Maquina_procesa(Maquina *self);


#endif
