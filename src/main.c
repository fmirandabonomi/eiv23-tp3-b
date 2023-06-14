#include <soporte_placa.h>
#include "controlador_luz.h"
#include "pulsador.h"
#include "despacho_retardado.h"
#include <stddef.h>


#define PIN_LUZ SP_PIN_LED
#define PIN_PULSADOR SP_PB9

#define HISTERESIS_ANTIRREBOTE 5

#define LUZ_ON 0

#define PULSADOR_NIVEL_ACTIVO 0

#define TIEMPO_ON 60000


static Maquina * controlador;
static Pulsador pulsador[1];
static DespachoRetardado despachoRetardado[1];

/**
 * @brief Inicializa el estado del programa para iniciar la ejecución
 * 
 */
static void setup(void);


int main(void){    
    setup();
    for (;;){
        Maquina_procesa(controlador);
        DespachoRetardado_procesa(despachoRetardado);
        Pulsador_procesa(pulsador);
    }
    return 0;
}



static void setup(void){
    static ControladorLuz instanciaControlador;
    
    SP_init();
    
    DespachoRetardado_init(despachoRetardado);

    ControladorLuz_init(&instanciaControlador,TIEMPO_ON,PIN_LUZ,LUZ_ON,despachoRetardado);
    controlador = ControladorLuz_asMaquina(&instanciaControlador);
    Maquina_procesa(controlador); // Reset inicializa pin con luz apagada

    Pulsador_init(pulsador, 
                  controlador,
                  EV_BOTON_PULSADO,
                  PIN_PULSADOR,
                  PULSADOR_NIVEL_ACTIVO,
                  HISTERESIS_ANTIRREBOTE);
}