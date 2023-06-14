#ifndef PULSADOR_H
#define PULSADOR_H
#include <maquina_estado.h>
#include <soporte_placa.h>


/**
 * @brief Controlador de pulsador con filtrado antirrebote.
 * Despacha un evento al detectar pulsación.
 * Implementado como máquina de estado de control
 * 
 */
typedef struct Pulsador{
    struct{
        bool nivelActivo;
        SP_HPin pin;
        uint8_t histeresis;
    }parametros;
    uint32_t t0;
    Maquina *destino;
    Evento evento;
    struct{
        bool nivelAnterior;
        uint8_t contador;
    }estado;
} Pulsador;


/**
 * @brief Inicializa la lectura de pulsador y configura el evento
 * de pulsado y su destino
 * 
 * @param self Este objeto
 * @param destino Maquina de estado
 * @param evento Evento a despachar cuando se detecta pulsación
 * @param pin Handle de pin donde está conectado el pulsador
 * @param nivelActivo Nivel lógico que toma el pin cuando el
 * pulsador es presionado
 * @param histeresis Número de lecturas consecutivas idénticas
 * necesarias para cambiar el estado filtrado
 */
void Pulsador_init(Pulsador *self, 
                   Maquina *destino,
                   Evento evento,
                   SP_HPin pin,
                   bool nivelActivo,
                   uint8_t histeresis);
/**
 * @brief Lee pulsador, actualiza la máquina de estado y
 * despacha evento si detecta pulsación
 * 
 * @param self 
 */
void Pulsador_procesa(Pulsador *self);

#endif
