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



/**
 * @brief Configura los pines
 * 
 */
static void setupPines(void);
/**
 * @brief Enciende la luz
 * 
 */
static void luzOn(void);
/**
 * @brief Apaga la luz
 * 
 */
static void luzOff(void);

/**
 * @brief Llama a despacho retardado en la instancia
 * despachoRetardado en particular
 * 
 * @param destino 
 * @param evento 
 * @param tiempoMilisegundos 
 */
static void DespachoRetardado_programa_bound(Maquina *const destino, Evento const evento, uint32_t const tiempoMilisegundos);

static void setup(void){
    static ControladorLuz_Acciones const accionesControladorLuz ={
        .apagaLuz               = luzOff,
        .enciendeLuz            = luzOn,
        .despachaLuegoDeTiempo  = DespachoRetardado_programa_bound 
    };

    static ControladorLuz instanciaControlador;
    
    SP_init();
    
    setupPines();
    
    DespachoRetardado_init(despachoRetardado);

    ControladorLuz_init(&instanciaControlador,TIEMPO_ON,&accionesControladorLuz);
    controlador = ControladorLuz_asMaquina(&instanciaControlador);
    Maquina_procesa(controlador); // Inicializa

    Pulsador_init(pulsador, 
                  controlador,
                  EV_BOTON_PULSADO,
                  PIN_PULSADOR,
                  PULSADOR_NIVEL_ACTIVO,
                  HISTERESIS_ANTIRREBOTE);
}





static void setupPines(void){
    SP_Pin_setModo(PIN_LUZ,SP_PIN_ENTRADA);
    SP_Pin_write(PIN_LUZ,!LUZ_ON);
    SP_Pin_setModo(PIN_LUZ,SP_PIN_SALIDA);
}

static void luzOn(void){
    SP_Pin_write(PIN_LUZ,LUZ_ON);
}
static void luzOff(void){
    SP_Pin_write(PIN_LUZ,!LUZ_ON);
}
static void DespachoRetardado_programa_bound(Maquina *const destino, Evento const evento, uint32_t const tiempoMilisegundos){
    DespachoRetardado_programa(despachoRetardado,destino,evento,tiempoMilisegundos);
}



