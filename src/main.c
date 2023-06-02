#include <soporte_placa.h>
#include "controlador_luz.h"
#include <stddef.h>

#define MAX_DESPACHOS_RETARDADOS_ACTIVOS 4

#define PIN_LUZ SP_PIN_LED
#define PIN_PULSADOR SP_PB9

#define HISTERESIS_ANTIRREBOTE 5

#define LUZ_ON 0
#define LUZ_OFF 1
#define PULSADOR_NIVEL_ACTIVO 0
#define PULSADOR_NIVEL_NORMAL 1
#define TIEMPO_ON 60000


static Maquina* controlador;


/**
 * @brief Inicializa el estado del programa para iniciar la ejecución
 * 
 */
static void setup(void);
/**
 * @brief Función de proceso de despacho retardado, debe ejecutarse en el lazo
 * principal. No bloquea.
 * 
 */
static void DespachoRetardado_procesa(void);
/**
 * @brief Función de lectura de pulsador, debe ejecutarse en el lazo principal
 * 
 */
static void Pulsador_procesa(void);


int main(void){    
    setup();
    for (;;){
        Maquina_procesa(controlador);
        DespachoRetardado_procesa();
        Pulsador_procesa();
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
 * @brief Programa el despacho retardado de un evento
 * 
 * @param destino Destino, máquina de estado
 * @param evento Evento
 * @param tiempoMilisegundos Retardo 
 */
static void DespachoRetardado_programa(Maquina *destino, Evento evento, uint32_t tiempoMilisegundos);

/**
 * @brief Inicializa la lectura de pulsador y configura el evento
 * de pulsado y su destino
 * 
 * @param destino Maquina de estado
 * @param evento Evento a despachar cuando se detecta pulsación
 */
static void Pulsador_init(Maquina *destino, Evento evento);

static void setup(void){
    static ControladorLuz_Acciones const accionesControladorLuz ={
        .apagaLuz               = luzOff,
        .enciendeLuz            = luzOn,
        .despachaLuegoDeTiempo  = DespachoRetardado_programa 
    };

    static ControladorLuz implControlador;

    SP_init();
    
    setupPines();
    
    ControladorLuz_init(&implControlador,TIEMPO_ON,&accionesControladorLuz);
    controlador = ControladorLuz_asMaquina(&implControlador);
    Maquina_procesa(controlador); // Estado inicial luz

    Pulsador_init(controlador,EV_BOTON_PULSADO);
}





static void setupPines(void){
    if (PULSADOR_NIVEL_NORMAL)
        SP_Pin_setModo(PIN_PULSADOR,SP_PIN_ENTRADA_PULLUP);
    else
        SP_Pin_setModo(PIN_PULSADOR,SP_PIN_ENTRADA_PULLDN);

    SP_Pin_setModo(PIN_LUZ,SP_PIN_ENTRADA);
    SP_Pin_write(PIN_LUZ,LUZ_OFF);
    SP_Pin_setModo(PIN_LUZ,SP_PIN_SALIDA);
}






/************************* Enciende/Apaga Luz *************************/
static void luzOn(void){
    SP_Pin_write(PIN_LUZ,LUZ_ON);
}
static void luzOff(void){
    SP_Pin_write(PIN_LUZ,LUZ_OFF);
}





/********************* Maneja despacho retardado **********************/

static struct DespachoRetardado{
    uint32_t t0;
    struct DespachoEnEspera{
        Maquina *destino;
        Evento evento;
        uint32_t cuenta;
    }despachosEnEspera[MAX_DESPACHOS_RETARDADOS_ACTIVOS];
    size_t numDespachosEnEspera;
}despachoRetardado={0};

static void DespachoRetardado_programa(Maquina *const destino, Evento const evento, uint32_t const tiempoMilisegundos){
    size_t i;
    size_t const N = despachoRetardado.numDespachosEnEspera;
    struct DespachoEnEspera *const d = despachoRetardado.despachosEnEspera;
    for(i=0;i<N;++i){
        if((destino == d[i].destino) && (evento == d[i].evento)) break;
    }
    if (i<MAX_DESPACHOS_RETARDADOS_ACTIVOS){
        d[i].cuenta  = tiempoMilisegundos;
        d[i].destino = destino;
        d[i].evento  = evento;
        if (N == i)
            despachoRetardado.numDespachosEnEspera = N+1;
    }else{
        Maquina_despacha(destino,evento);
    }
}

static void DespachoRetardado_procesa(void){
    uint32_t const t = SP_Tiempo_getMilisegundos();

    if (despachoRetardado.t0 != t){
        despachoRetardado.t0 = t;

        size_t N = despachoRetardado.numDespachosEnEspera;
        struct DespachoEnEspera *const d = despachoRetardado.despachosEnEspera;
        for(size_t i=0;i<N;++i){
            if(d[i].cuenta > 1){
                --(d[i].cuenta);
            }else if (d[i].cuenta){
                d[i].cuenta = 0;
                Maquina_despacha(d[i].destino,d[i].evento);
            }
            if(!d[i].cuenta){ // Cuenta cumplida, deshecha el elemento y vuelve a procesar el lugar
                d[i] = d[N-1];
                --i;
                --N;
            }
        }
        despachoRetardado.numDespachosEnEspera = N;
    }
}





/********************* Lectura pulsador **********************/

static struct Pulsador{
    uint32_t t0;
    Maquina *destino;
    Evento evento;
    struct{
        bool nivelAnterior;
        uint8_t contador;
    }estado;
} pulsador = {0};

void Pulsador_init(Maquina *destino,Evento evento){
    pulsador.destino = destino;
    pulsador.evento  = evento;
    pulsador.estado.nivelAnterior = PULSADOR_NIVEL_NORMAL;
    if (PULSADOR_NIVEL_NORMAL){
        pulsador.estado.contador = HISTERESIS_ANTIRREBOTE;
    }
}

void Pulsador_procesa(void){
    uint32_t const t = SP_Tiempo_getMilisegundos();
    if (pulsador.t0 != t){
        pulsador.t0 = t;
        bool const nivelPin = SP_Pin_read(PIN_PULSADOR);
        bool nivelFlitrado = pulsador.estado.nivelAnterior;
        if (nivelPin && (pulsador.estado.contador < HISTERESIS_ANTIRREBOTE)){
            uint8_t const nuevoContador = pulsador.estado.contador + 1;
            if (nuevoContador == HISTERESIS_ANTIRREBOTE){
                nivelFlitrado = 1;
            }
            pulsador.estado.contador = nuevoContador;
        }else if(!nivelPin && (pulsador.estado.contador > 0)){
            uint8_t const nuevoContador = pulsador.estado.contador - 1;
            if (!nuevoContador){
                nivelFlitrado = 0;
            }
            pulsador.estado.contador = nuevoContador;
        }
        if (nivelFlitrado != pulsador.estado.nivelAnterior){
            pulsador.estado.nivelAnterior = nivelFlitrado;
            if (PULSADOR_NIVEL_ACTIVO == nivelFlitrado){ // Pulsador presionado
                Maquina_despacha(pulsador.destino,pulsador.evento);
            }
        }
    }   
}