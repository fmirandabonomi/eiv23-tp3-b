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
    Maquina *maquina;
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


#define COMPRUEBA_SECUENCIA_EVENTOS(arrayEventosEsperados) Aux_compruebaSecuenciaEventos((arrayEventosEsperados),sizeof(arrayEventosEsperados)/sizeof((arrayEventosEsperados)[0]),__LINE__)


static void Aux_copiaColaEventosEnBorrador(ColaEventos *cola){
    aux.borradorEventos.numEventos = ColaEventos_getEventos(cola,MAX_EVENTOS_BORRADOR,aux.borradorEventos.eventos);
}

static void Aux_compruebaSecuenciaEventos(Evento const *const eventos,size_t const numEventos, UNITY_LINE_TYPE const linea){
    Aux_copiaColaEventosEnBorrador(aux.receptor);
    size_t N = numEventos;
    if (N>aux.borradorEventos.numEventos) 
        N = aux.borradorEventos.numEventos;
    CustomAssertEqualEventoArray(eventos,aux.borradorEventos.eventos,N,linea);
    //TEST_ASSERT_EQUAL_MESSAGE(numEventos,aux.borradorEventos.numEventos," Numero de eventos "); con linea en lugar de __LINE__
    UnityAssertEqualNumber((UNITY_INT)((numEventos)), (UNITY_INT)((aux.borradorEventos.numEventos)), ((" Numero de eventos ")), (UNITY_UINT)(linea), UNITY_DISPLAY_STYLE_INT);
    
    ColaEventos_clear(aux.receptor);\

}

static void Aux_finPrueba(void){
    if(aux.maquina){
        Maquina_finalize(aux.maquina);
        free(aux.maquina);
        aux.maquina = NULL;
    }
}

void setUp(void){
    aux.cola = ColaEventos_new(MAX_EVENTOS_COLA);
    aux.despachador = Despachador_new(MAX_RECEPTORES);
    aux.pila = PilaEstados_new(NIVELES_PILA);
    aux.timer = MTimer_new(MAX_TIMEOUTS);
    aux.receptor = ColaEventos_new(MAX_EVENTOS_RECEPTOR);
    memset(&aux.borradorEventos,0,sizeof(aux.borradorEventos));
    aux.maquina = NULL;
}

void tearDown(void){
    /* debe borrarse primero el objeto dependiente (con finPrueba) y luego los auxiliares */
    Aux_finPrueba();
    ColaEventos_delete(aux.cola);
    Despachador_delete(aux.despachador);
    PilaEstados_delete(aux.pila);
    MTimer_delete(aux.timer);
    ColaEventos_delete(aux.receptor);
}

static Maquina * Aux_creaMaquina(MaquinaNew maquinaNew){
    TEST_ASSERT_NULL(aux.maquina);
    aux.maquina = maquinaNew(
        ColaEventos_asIColaEventos(aux.cola),
        Despachador_asIDespachadorEvento(aux.despachador),
        PilaEstados_asIPilaEstados(aux.pila),
        MTimer_asITimer(aux.timer));
    TEST_ASSERT_NOT_NULL(aux.maquina);
    TEST_ASSERT_TRUE(Maquina_addReceptorEvento(aux.maquina,ColaEventos_asIReceptorEvento(aux.receptor)));
    return aux.maquina;
}

static void test_maquina_debe_admitir_al_menos_un_receptor(void){
    Maquina *volatile maquina = Aux_creaMaquina(Prueba1_new);
    (void)maquina;
}
static void test_evento_inicial_debe_estar_en_cola_de_maquina_nueva(void){
    Maquina *volatile maquina = Aux_creaMaquina(Prueba1_new);
    (void)maquina;
    Aux_copiaColaEventosEnBorrador(aux.cola);
    TEST_ASSERT_EQUAL_size_t(1,aux.borradorEventos.numEventos);
    TEST_ASSERT_EQUAL(MSG_INICIALIZA,aux.borradorEventos.eventos[0].mensaje);
    TEST_ASSERT_EQUAL(0,aux.borradorEventos.eventos[0].param.uVal);
    Aux_copiaColaEventosEnBorrador(aux.receptor);
    TEST_ASSERT_EQUAL_size_t(0,aux.borradorEventos.numEventos);
}


static void test_debe_ejecutar_inicializacion_de_maquina(void){
    Maquina * maquina = Aux_creaMaquina(Prueba1_new);

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
    Maquina * maquina = Aux_creaMaquina(Prueba1_new);

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
    Maquina * maquina = Aux_creaMaquina(Prueba1_new);
    Maquina_procesa(maquina);
    ColaEventos_clear(aux.receptor);
    Aux_finPrueba();
    static Evento const esperadoEventos[]={EVENTO_ACUSE_SALIDA('A')};
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);

}

static void test_debe_soportar_todas_las_transiciones_para_dos_estados_nivel_0(void){
    Maquina *maquina = Aux_creaMaquina(Prueba2_new);

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

static void test_debe_soportar_todas_las_transiciones_en_una_maquina_de_un_estado_anidado_iniciando_en_estado_superior(void){
    Maquina *maquina = Aux_creaMaquina(Prueba3_new_a);

    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // INICIALIZA
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T1);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T2);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T3);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T4);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T5);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T6);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T7);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T8);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_NULO(maquina);
    Aux_finPrueba();
    maquina = NULL;
    static Evento const esperadoEventos[]={
        EVENTO_ACUSE_ENTRADA('A'),      // [*] --> A
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T1('A'),           // A : T1
        EVENTO_ACUSE_T2('B'),           // B : T2
        EVENTO_ACUSE_T3('B'),           // A :: B --> B : T3
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T4('B'),           // A :: B --> A : T4
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T5('A'),           // A :: A --> B : T5
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T6('B'),           // A.B --> A : T6
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T7('A'),           // A --> A : T7
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T8('A'),           // A --> A.B : T8
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_SALIDA('B'),       // finaliza
        EVENTO_ACUSE_SALIDA('A')
    };
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);
}
static void test_debe_soportar_todas_las_transiciones_en_una_maquina_de_un_estado_anidado_iniciando_en_estado_anidado(void){
    Maquina *maquina = Aux_creaMaquina(Prueba3_new_b);

    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // INICIALIZA
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T1);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T2);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T3);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T4);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T5);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T6);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T7);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T8);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_NULO(maquina);
    Aux_finPrueba();
    maquina = NULL;
    static Evento const esperadoEventos[]={
        EVENTO_ACUSE_ENTRADA('A'),      // [*]-->A.B
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T1('A'),           // A : T1
        EVENTO_ACUSE_T2('B'),           // B : T2
        EVENTO_ACUSE_T3('B'),           // A :: B --> B : T3
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'), 
        EVENTO_ACUSE_T4('B'),           // A :: B --> A : T4
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T5('A'),           // A :: A --> B : T5
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T6('B'),           // A.B --> A : T6
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T7('A'),           // A --> A : T7
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T8('A'),           // A --> A.B : T8
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_SALIDA('B'),       // finaliza
        EVENTO_ACUSE_SALIDA('A')
    };
    COMPRUEBA_SECUENCIA_EVENTOS(esperadoEventos);
}
static void test_debe_soportar_transiciones_entre_estados_anidados_y_de_nivel_superior(void){
    Maquina *maquina = Aux_creaMaquina(Prueba4_new);

    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina); // INICIALIZA
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T1);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T2);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T3);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T5);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_DESPACHA_ESPERA_TRUE(maquina,evento.T6);
    MAQUINA_PROCESA_ESPERA_PROCESADO(maquina);
    MAQUINA_PROCESA_ESPERA_NULO(maquina);

    static Evento const esperadoEventos[] = {
        EVENTO_ACUSE_ENTRADA('A'),      // [*]-->A
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T1('A'),           // A-->A.B : T1
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T2('B'),           // A.B-->C : T2
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('C'),
        EVENTO_ACUSE_INICIALIZA('C'),
        EVENTO_ACUSE_T3('C'),           // C-->A : T3
        EVENTO_ACUSE_SALIDA('C'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_INICIALIZA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
        EVENTO_ACUSE_T5('A'),           // A-->C : T5
        EVENTO_ACUSE_SALIDA('B'),
        EVENTO_ACUSE_SALIDA('A'),
        EVENTO_ACUSE_ENTRADA('C'),
        EVENTO_ACUSE_INICIALIZA('C'),
        EVENTO_ACUSE_T6('C'),           // C-->A.B : T6
        EVENTO_ACUSE_SALIDA('C'),
        EVENTO_ACUSE_ENTRADA('A'),
        EVENTO_ACUSE_ENTRADA('B'),
        EVENTO_ACUSE_INICIALIZA('B'),
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
    RUN_TEST(test_debe_soportar_todas_las_transiciones_para_dos_estados_nivel_0);
    RUN_TEST(test_debe_soportar_todas_las_transiciones_en_una_maquina_de_un_estado_anidado_iniciando_en_estado_superior);
    RUN_TEST(test_debe_soportar_todas_las_transiciones_en_una_maquina_de_un_estado_anidado_iniciando_en_estado_anidado);
    RUN_TEST(test_debe_soportar_transiciones_entre_estados_anidados_y_de_nivel_superior);
    UNITY_END();
    return 0;
}
