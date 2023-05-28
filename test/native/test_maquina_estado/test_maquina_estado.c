#include <maquina_estado.h>
#include <unity.h>
#include <malloc.h>
#include <string.h>
#include "inc/cola_eventos.h"
#include "inc/despachador.h"
#include "inc/pila_estados.h"
#include "inc/m_timer.h"
#include "inc/mensajes.h"
#include "inc/prueba1.h"
#include "inc/test_array_evento_equal.h"

static struct{
    Evento T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15;
} const evento={
    .T1 = {.mensaje=MSG_T1,.param.uVal=0},
    .T2 = {.mensaje=MSG_T2,.param.uVal=0},
    .T3 = {.mensaje=MSG_T3,.param.uVal=0},
    .T4 = {.mensaje=MSG_T4,.param.uVal=0},
    .T5 = {.mensaje=MSG_T5,.param.uVal=0},
    .T6 = {.mensaje=MSG_T6,.param.uVal=0},
    .T7 = {.mensaje=MSG_T7,.param.uVal=0},
    .T8 = {.mensaje=MSG_T8,.param.uVal=0},
    .T9 = {.mensaje=MSG_T9,.param.uVal=0},
    .T10 = {.mensaje=MSG_T10,.param.uVal=0},
    .T11 = {.mensaje=MSG_T11,.param.uVal=0},
    .T12 = {.mensaje=MSG_T12,.param.uVal=0},
    .T13 = {.mensaje=MSG_T13,.param.uVal=0},
    .T14 = {.mensaje=MSG_T14,.param.uVal=0},
    .T15 = {.mensaje=MSG_T15,.param.uVal=0},
};

enum{
    MAX_EVENTOS_COLA=8,
    MAX_RECEPTORES=4,
    NIVELES_PILA=4,
    MAX_TIMEOUTS=4,
    MAX_EVENTOS_RECEPTOR=16,
    MAX_EVENTOS_BORRADOR = MAX_EVENTOS_COLA > MAX_EVENTOS_RECEPTOR ? MAX_EVENTOS_COLA : MAX_EVENTOS_RECEPTOR,
    AUX_MAX_PASOS_DELETE=2
};

typedef void (*FnDelete)(void*);
typedef struct PasoDelete{
    FnDelete fnDelete;
    void *pDelete;
}PasoDelete;
static struct Aux {
    ColaEventos *cola;
    Despachador *despachador;
    PilaEstados *pila;
    MTimer *timer;
    ColaEventos *receptor;
    struct BorradorEventos{
        size_t numEventos;
        Evento eventos[MAX_EVENTOS_BORRADOR];
    }borradorEventos;
    FnDelete fnDelete;
    void *pDelete;
}aux;

static void Aux_copiaColaEventosEnBorrador(ColaEventos *cola){
    aux.borradorEventos.numEventos = ColaEventos_getEventos(cola,MAX_EVENTOS_BORRADOR,aux.borradorEventos.eventos);
}

void setUp(void){
    aux.cola = ColaEventos_new(MAX_EVENTOS_COLA);
    aux.despachador = Despachador_new(MAX_RECEPTORES);
    aux.pila = PilaEstados_new(NIVELES_PILA);
    aux.timer = MTimer_new(MAX_TIMEOUTS);
    aux.receptor = ColaEventos_new(MAX_EVENTOS_RECEPTOR);
    memset(&aux.borradorEventos,0,sizeof(aux.borradorEventos));
    aux.pDelete=NULL;
    aux.fnDelete=(FnDelete)0;
}

void tearDown(void){
    /* debe borrarse primero el objeto dependiente (con fnDelete) y luego los auxiliares */
    if(aux.fnDelete && aux.pDelete)
        aux.fnDelete(aux.pDelete);
    
    ColaEventos_delete(aux.cola);
    Despachador_delete(aux.despachador);
    PilaEstados_delete(aux.pila);
    MTimer_delete(aux.timer);
    ColaEventos_delete(aux.receptor);
}

static Maquina * Aux_conPrueba1(void){
    TEST_ASSERT_NULL(aux.pDelete);
    Prueba1 * obj = Prueba1_new(
        ColaEventos_asIColaEventos(aux.cola),
        Despachador_asIDespachadorEvento(aux.despachador),
        PilaEstados_asIPilaEstados(aux.pila),
        MTimer_asITimer(aux.timer));
    TEST_ASSERT_NOT_NULL(obj);
    aux.pDelete = obj;
    aux.fnDelete = (FnDelete)Prueba1_delete;

    Maquina * const result = Prueba1_asMaquina(obj);
    TEST_ASSERT_NOT_NULL(result);

    bool const receptorAgregado = Maquina_addReceptorEvento(result,ColaEventos_asIReceptorEvento(aux.receptor));
    TEST_ASSERT_TRUE(receptorAgregado);
    
    return result;
}

static void Aux_finPrueba(void){
    if(aux.fnDelete && aux.pDelete)
        aux.fnDelete(aux.pDelete);
    aux.fnDelete = (FnDelete)0;
    aux.pDelete = NULL;
}

static void test_maquina_debe_admitir_al_menos_un_receptor(void){
    Maquina *volatile maquina = Aux_conPrueba1();
    (void)maquina;
}
static void test_evento_inicial_debe_estar_en_cola_de_maquina_nueva(void){
    Maquina *volatile maquina = Aux_conPrueba1();
    (void)maquina;
    Aux_copiaColaEventosEnBorrador(aux.cola);
    TEST_ASSERT_EQUAL_size_t(1,aux.borradorEventos.numEventos);
    TEST_ASSERT_EQUAL(MSG_INICIALIZA,aux.borradorEventos.eventos[0].mensaje);
    TEST_ASSERT_EQUAL(0,aux.borradorEventos.eventos[0].param.uVal);
    Aux_copiaColaEventosEnBorrador(aux.receptor);
    TEST_ASSERT_EQUAL_size_t(0,aux.borradorEventos.numEventos);
}


static void test_debe_ejecutar_inicializacion_de_maquina(void){
    Maquina * maquina = Aux_conPrueba1();
    ResultadoEvento r = Maquina_procesa(maquina);
    TEST_ASSERT_EQUAL(r.codigo,RES_EVENTO_PROCESADO);
    Aux_copiaColaEventosEnBorrador(aux.receptor);
    static Evento const esperadoEventos[] = {
        EVENTO_ACUSE('A',MSG_ENTRADA),
        EVENTO_ACUSE('A',MSG_INICIALIZA)
    };
    static size_t const esperadoNumEventos = sizeof(esperadoEventos)/sizeof(*esperadoEventos);
    TEST_ASSERT_EQUAL(esperadoNumEventos,aux.borradorEventos.numEventos);
    /* En máquina de 64 bit falla porque al copiar una estructura no necesariamente se copian 
     * los bytes de relleno (padding) para alineación y Evento tiene la estructura
     * {unsigned mensaje:32;unsigned <padding>:32;unsigned param:64}
     * porque en x86_64 los punteros son de 64 bit (con alineación de 64 bit) y param debe acomodar
     * un puntero
     *``` 
     * TEST_ASSERT_EQUAL_MEMORY(esperadoEventos,aux.borradorEventos.eventos,sizeof(esperadoEventos));
     * ```
    */
    TEST_ASSERT_EQUAL_EVENTO_ARRAY(esperadoEventos,aux.borradorEventos.eventos,esperadoNumEventos);
}
static void test_ejecuta_secuencia_de_eventos(void){
    Maquina * maquina = Aux_conPrueba1();
    ResultadoEvento rE;
    bool rB;
    
    Maquina_procesa(maquina);
    rE = Maquina_procesa(maquina); 
    TEST_ASSERT_EQUAL(rE.codigo,RES_NULO);
    rB = Maquina_dispatch(maquina,evento.T1);
    TEST_ASSERT_TRUE(rB);
    rB = Maquina_dispatch(maquina,evento.T5);
    TEST_ASSERT_TRUE(rB);
    rB = Maquina_dispatch(maquina,evento.T2);
    TEST_ASSERT_TRUE(rB);
    rB = Maquina_dispatch(maquina,evento.T3);
    TEST_ASSERT_TRUE(rB);
    rE = Maquina_procesa(maquina);
    TEST_ASSERT_EQUAL(rE.codigo,RES_EVENTO_PROCESADO);
    rE = Maquina_procesa(maquina);
    TEST_ASSERT_EQUAL(rE.codigo,RES_EVENTO_IGNORADO);
    rE = Maquina_procesa(maquina);
    TEST_ASSERT_EQUAL(rE.codigo,RES_EVENTO_PROCESADO);
    rE = Maquina_procesa(maquina);
    TEST_ASSERT_EQUAL(rE.codigo,RES_EVENTO_PROCESADO);
    

    static Evento const esperadoEventos[] = {
        EVENTO_ACUSE('A',MSG_ENTRADA),
        EVENTO_ACUSE('A',MSG_INICIALIZA),
        EVENTO_ACUSE('A',MSG_T1),
        EVENTO_ACUSE('A',MSG_T2),
        EVENTO_ACUSE('A',MSG_T3),
        EVENTO_ACUSE('A',MSG_SALIDA),
        EVENTO_ACUSE('A',MSG_ENTRADA),
        EVENTO_ACUSE('A',MSG_INICIALIZA)
    };
    static size_t const esperadoNumEventos = sizeof(esperadoEventos)/sizeof(*esperadoEventos);

    Aux_copiaColaEventosEnBorrador(aux.receptor);
    TEST_ASSERT_EQUAL(esperadoNumEventos,aux.borradorEventos.numEventos);
    TEST_ASSERT_EQUAL_EVENTO_ARRAY(esperadoEventos,aux.borradorEventos.eventos,esperadoNumEventos);
}
static void test_sale_de_estados_al_finalizar_maquina(void){
    Maquina * maquina = Aux_conPrueba1();
    Maquina_procesa(maquina);
    ColaEventos_clear(aux.receptor);
    Aux_finPrueba();
    static Evento const esperadoEventos[]={EVENTO_ACUSE('A',MSG_SALIDA)};
    static size_t const esperadoNumEventos = sizeof(esperadoEventos)/sizeof(esperadoEventos[0]);
    Aux_copiaColaEventosEnBorrador(aux.receptor);
    TEST_ASSERT_EQUAL(esperadoNumEventos,aux.borradorEventos.numEventos);
    TEST_ASSERT_EQUAL_EVENTO_ARRAY(esperadoEventos,aux.borradorEventos.eventos,esperadoNumEventos);
}

/* Punto de entrada*/
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_maquina_debe_admitir_al_menos_un_receptor);
    RUN_TEST(test_evento_inicial_debe_estar_en_cola_de_maquina_nueva);
    RUN_TEST(test_debe_ejecutar_inicializacion_de_maquina);
    RUN_TEST(test_ejecuta_secuencia_de_eventos);
    RUN_TEST(test_sale_de_estados_al_finalizar_maquina);
    UNITY_END();
    return 0;
}
