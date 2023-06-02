#include <soporte_placa.h>
#include "pulsador.h"
#include "controlador_luz.h"
#include "accion_despacha_evento.h"
#include "soporte_controlador_impl.h"

#define PIN_LUZ SP_PIN_LED
#define PIN_PULSADOR SP_PB9

#define HISTERESIS_ANTIRREBOTE 5

#define LUZ_ON 0
#define LUZ_OFF 1
#define PULSADOR_ACTIVO 0
#define PULSADOR_NORMAL 1
#define TIEMPO_ON 60000


static Maquina* controlador;
static Pulsador *pulsador;

static void setup(void){
    static ControladorLuz implControlador;
    static SoporteControlador soporteControlador;
    static DespachaEvento evBotonPulsado;
    static Pulsador implPulsador;
    SP_init();

    SoporteControlador_init(&soporteControlador,PIN_LUZ,LUZ_ON);
    ControladorLuz_init(&implControlador,TIEMPO_ON,&soporteControlador);
    controlador = ControladorLuz_asMaquina(&implControlador);

    DespachaEvento_init(&evBotonPulsado,EV_BOTON_PULSADO,controlador);
    Pulsador_init(&implPulsador,
                     PIN_PULSADOR,
                     PULSADOR_ACTIVO,
                     HISTERESIS_ANTIRREBOTE,
                     DespachaEvento_asIAccion(&evBotonPulsado));
    pulsador = &implPulsador;

    Maquina_procesa(controlador); // Estado inicial luz

}

int main(void){    
    setup();
    for (;;){
        Maquina_procesa(controlador);
        Pulsador_procesa(pulsador);
    }
    return 0;
}