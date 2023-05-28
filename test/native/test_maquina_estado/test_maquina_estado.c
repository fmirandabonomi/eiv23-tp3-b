#include <maquina_estado.h>
#include <unity.h>
#include <malloc.h>
#include <string.h>
#include "inc/cola_eventos.h"
#include "inc/despachador.h"
#include "inc/pila_estados.h"
#include "inc/m_timer.h"
#include "inc/mensajes.h"
#include "inc/maquinas_de_prueba.h"
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

#define EVENTO_ACUSE_INICIALIZA(estado) EVENTO_ACUSE(estado,MSG_INICIALIZA)
#define EVENTO_ACUSE_ENTRADA(estado) EVENTO_ACUSE(estado,MSG_ENTRADA)
#define EVENTO_ACUSE_SALIDA(estado) EVENTO_ACUSE(estado,MSG_SALIDA)
#define EVENTO_ACUSE_T1(estado) EVENTO_ACUSE(estado,MSG_T1)
#define EVENTO_ACUSE_T2(estado) EVENTO_ACUSE(estado,MSG_T2)
#define EVENTO_ACUSE_T3(estado) EVENTO_ACUSE(estado,MSG_T3)
#define EVENTO_ACUSE_T4(estado) EVENTO_ACUSE(estado,MSG_T4)
#define EVENTO_ACUSE_T5(estado) EVENTO_ACUSE(estado,MSG_T5)
#define EVENTO_ACUSE_T6(estado) EVENTO_ACUSE(estado,MSG_T6)
#define EVENTO_ACUSE_T7(estado) EVENTO_ACUSE(estado,MSG_T7)
#define EVENTO_ACUSE_T8(estado) EVENTO_ACUSE(estado,MSG_T8)
#define EVENTO_ACUSE_T9(estado) EVENTO_ACUSE(estado,MSG_T9)
#define EVENTO_ACUSE_T10(estado) EVENTO_ACUSE(estado,MSG_T10)
#define EVENTO_ACUSE_T11(estado) EVENTO_ACUSE(estado,MSG_T11)
#define EVENTO_ACUSE_T12(estado) EVENTO_ACUSE(estado,MSG_T12)
#define EVENTO_ACUSE_T13(estado) EVENTO_ACUSE(estado,MSG_T13)
#define EVENTO_ACUSE_T14(estado) EVENTO_ACUSE(estado,MSG_T14)
#define EVENTO_ACUSE_T15(estado) EVENTO_ACUSE(estado,MSG_T15)

enum{
    MAX_EVENTOS_COLA=32,
    MAX_RECEPTORES=4,
    NIVELES_PILA=4,
    MAX_TIMEOUTS=4,
    MAX_EVENTOS_RECEPTOR = 128,
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

#define MAQUINA_PROCESA_ESPERA_RESULTADO(maquina,resultado) do{\
    ResultadoEvento const r = Maquina_procesa((maquina));\
    TEST_ASSERT_EQUAL(resultado,r.codigo);\
}while(0)

#define MAQUINA_PROCESA_ESPERA_NULO(maquina) MAQUINA_PROCESA_ESPERA_RESULTADO(maquina,RES_NULO)
#define MAQUINA_PROCESA_ESPERA_IGNORADO(maquina) MAQUINA_PROCESA_ESPERA_RESULTADO(maquina,RES_EVENTO_IGNORADO)
#define MAQUINA_PROCESA_ESPERA_PROCESADO(maquina) MAQUINA_PROCESA_ESPERA_RESULTADO(maquina,RES_EVENTO_PROCESADO)

#define MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento) TEST_ASSERT_TRUE(Maquina_dispatch(maquina,evento))
#define MAQUINA_DESPACHA_ESPERA_FALSE(maquina,evento) TEST_ASSERT_FALSE(Maquina_dispatch(maquina,evento))


#define COMPRUEBA_SECUENCIA_EVENTOS(arrayEventosEsperados) do{\
    Aux_copiaColaEventosEnBorrador(aux.receptor);\
    static size_t const esperadoNumEventos = sizeof(arrayEventosEsperados)/sizeof(arrayEventosEsperados[0]);\
    TEST_ASSERT_EQUAL_MESSAGE(esperadoNumEventos,aux.borradorEventos.numEventos," Numero de eventos ");\
    TEST_ASSERT_EQUAL_EVENTO_ARRAY(esperadoEventos,aux.borradorEventos.eventos,esperadoNumEventos);\
    ColaEventos_clear(aux.receptor);\
}while(0)


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

static Maquina * Aux_conPrueba2(void){
    TEST_ASSERT_NULL(aux.pDelete);
    Prueba2 *obj = Prueba2_new(ColaEventos_asIColaEventos(aux.cola),
                               Despachador_asIDespachadorEvento(aux.despachador),
                               PilaEstados_asIPilaEstados(aux.pila),
                               MTimer_asITimer(aux.timer));
    TEST_ASSERT_NOT_NULL(obj);
    aux.pDelete = obj;
    aux.fnDelete = (FnDelete) Prueba2_delete;

    Maquina * const result = Prueba2_asMaquina(obj);
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

    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);

    static Evento const esperadoEventos[] = {
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A')
    };
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);

    /* En máquina de 64 bit falla porque al copiar una estructura no necesariamente se copian 
     * los bytes de relleno (padding) para alineación y Evento tiene la estructura
     * {unsigned mensaje:32;unsigned <padding>:32;unsigned param:64}
     * porque en x86_64 los punteros son de 64 bit (con alineación de 64 bit) y param debe acomodar
     * un puntero
     *``` 
     * TEST_ASSERT_EQUAL_MEMORY(esperadoEventos,aux.borradorEventos.eventos,sizeof(esperadoEventos));
     * ```
    */
}
static void test_ejecuta_secuencia_de_eventos(void){
    Maquina * maquina = Aux_conPrueba1();

    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_NULO(maquina); 

    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T1);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T5);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T2);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T3);
    
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_IGNORADO(maquina);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_NULO(maquina);

    static Evento const esperadoEventos[] = {
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_T1('A'),
        EVENTO_ACUSE_T2('A'),
        EVENTO_ACUSE_T3('A'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A')
    };
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);
}
static void test_sale_de_estados_al_finalizar_maquina(void){
    Maquina * maquina = Aux_conPrueba1();
    Maquina_procesa(maquina);
    ColaEventos_clear(aux.receptor);
    Aux_finPrueba();
    static Evento const esperadoEventos[]={EVENTO_ACUSE_SALIDA('A')};
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);

}

static void test_transicion_entre_estados_debe_generar_salida_entrada_inicializacion(void){
    Maquina *maquina = Aux_conPrueba2();

    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T1);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T2);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T3);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T3);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T4);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T5);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T6);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T1);

    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // INICIALIZA
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T1
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T2
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T3
    MAQUINA_PROCESA_ESPERA_IGNORADO(maquina);  // T3
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T4
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T5
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T6
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // T1
    MAQUINA_PROCESA_ESPERA_NULO(maquina);
    Aux_finPrueba(); // Finaliza y elimina máquina
    maquina = NULL;    
    static Evento const esperadoEventos[]={
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_T1('A'),
        EVENTO_ACUSE_T2('A'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_T3('A'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T4('B'),
        EVENTO_ACUSE_T5('B'),
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T6('B'),
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_T1('A'),
        EVENTO_ACUSE_SALIDA('A')
    };
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);
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
    RUN_TEST(test_transicion_entre_estados_debe_generar_salida_entrada_inicializacion);
    UNITY_END();
    return 0;
}
