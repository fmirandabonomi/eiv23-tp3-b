#include <unity.h>
#include <soporte_placa.h>
#include "maquina_prueba.h"
#include "accion_despacha_evento.h"

static Maquina m;

static struct Contadores{
    unsigned accion0;
    unsigned accion1;
    unsigned accion2;
    unsigned accion3;
    unsigned accion4;
    unsigned accion5;
    unsigned accion6;
    unsigned accion7;
    unsigned accion8;
    unsigned accion9;
}contadores;

static Maquina *m_accion5;

void accion0(void){
    contadores.accion0++;
}
void accion1(void){
    contadores.accion1++;
}
void accion2(void){
    contadores.accion2++;
}
void accion3(void){
    contadores.accion3++;
}
void accion4(void){
    contadores.accion4++;
}
void accion5(Maquina *maquina){
    contadores.accion5++;
    m_accion5 = maquina;
}
void accion6(void){
    contadores.accion6++;
}
void accion7(void){
    contadores.accion7++;
}
void accion8(void){
    contadores.accion8++;
}
void accion9(void){
    contadores.accion9++;
}

void setUp(void){
    MaquinaPrueba_init(&m);
    contadores = (struct Contadores){0};
    m_accion5 = NULL;
}

void tearDown(void){

}


static void test_maquina_estado_inicial_procesa_EV_RESET_como_primer_evento(void){
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_EQUAL(1,contadores.accion0);
}

static void test_maquina_estado_informa_si_no_hay_eventos_para_procesar(void){
    Maquina_procesa(&m); // EV_RESET
    TEST_ASSERT_FALSE(Maquina_procesa(&m));
}

static void test_maquina_de_prueba_soporta_eventos_sin_transicion_de_estado(void){
    TEST_ASSERT_TRUE(Maquina_despacha(&m,EV_E1));
    Maquina_procesa(&m);
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_EQUAL(1,contadores.accion1);
}

static void test_maquina_de_prueba_soporta_transicion_de_estado(void){
    Maquina_despacha(&m,EV_E2);
    TEST_ASSERT_TRUE(Maquina_despacha(&m,EV_E1));
    TEST_ASSERT_TRUE(Maquina_despacha(&m,EV_E3));
    Maquina_procesa(&m);
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_FALSE(Maquina_procesa(&m));
    TEST_ASSERT_EQUAL(1,contadores.accion2);
    TEST_ASSERT_EQUAL(0,contadores.accion1);
    TEST_ASSERT_EQUAL(1,contadores.accion3);
}

static void test_parametro_contexto_es_puntero_a_maquina(void){
    Maquina_despacha(&m,EV_E2);
    Maquina_despacha(&m,EV_E4);
    TEST_ASSERT_TRUE(Maquina_despacha(&m,EV_E5));
    Maquina_procesa(&m);
    Maquina_procesa(&m);
    Maquina_procesa(&m);
    TEST_ASSERT_NULL(m_accion5);
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_FALSE(Maquina_procesa(&m));
    TEST_ASSERT_EQUAL_PTR(&m,m_accion5);
    TEST_ASSERT_EQUAL(1,contadores.accion5);
 }

 static void test_todas_las_transiciones_tres_estados(void){
    Maquina_despacha(&m,EV_E2);
    Maquina_despacha(&m,EV_E4);
    Maquina_despacha(&m,EV_E6);
    Maquina_despacha(&m,EV_E7);
    Maquina_despacha(&m,EV_E8);
    Maquina_despacha(&m,EV_E9);

    Maquina_procesa(&m);
    Maquina_procesa(&m);
    Maquina_procesa(&m);
    Maquina_procesa(&m);
    Maquina_procesa(&m);
    Maquina_procesa(&m);
    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_FALSE(Maquina_procesa(&m));

    TEST_ASSERT_EQUAL(1,contadores.accion0);
    TEST_ASSERT_EQUAL(0,contadores.accion1);
    TEST_ASSERT_EQUAL(1,contadores.accion2);
    TEST_ASSERT_EQUAL(0,contadores.accion3);
    TEST_ASSERT_EQUAL(1,contadores.accion4);
    TEST_ASSERT_EQUAL(0,contadores.accion5);
    TEST_ASSERT_EQUAL(1,contadores.accion6);
    TEST_ASSERT_EQUAL(1,contadores.accion7);
    TEST_ASSERT_EQUAL(1,contadores.accion8);
    TEST_ASSERT_EQUAL(1,contadores.accion9);
 }

 static void test_accion_despacha_evento(void){
    AccionDespachaEvento d;

    Maquina_procesa(&m);
    TEST_ASSERT_FALSE(Maquina_procesa(&m));

    AccionDespachaEvento_init(&d,EV_E1,&m);

    IAccion *a = AccionDespachaEvento_asIAccion(&d);
    TEST_ASSERT_EQUAL(ED_NO_DESPACHADO, AccionDespachaEvento_getEstadoDespacho(&d));

    IAccion_ejecuta(a);
    TEST_ASSERT_EQUAL(ED_DESPACHADO,AccionDespachaEvento_getEstadoDespacho(&d));

    AccionDespachaEvento_deInit(&d);

    TEST_ASSERT_EQUAL(ED_NO_INICIALIZADO,AccionDespachaEvento_getEstadoDespacho(&d));
    IAccion_ejecuta(a);
    

    TEST_ASSERT_TRUE(Maquina_procesa(&m));
    TEST_ASSERT_FALSE(Maquina_procesa(&m));

    TEST_ASSERT_EQUAL(1,contadores.accion1);

 }

int main(void){
    SP_init();
    SP_Tiempo_delay(500);
    UNITY_BEGIN();
    RUN_TEST(test_maquina_estado_inicial_procesa_EV_RESET_como_primer_evento);
    RUN_TEST(test_maquina_estado_informa_si_no_hay_eventos_para_procesar);
    RUN_TEST(test_maquina_de_prueba_soporta_eventos_sin_transicion_de_estado);
    RUN_TEST(test_maquina_de_prueba_soporta_transicion_de_estado);
    RUN_TEST(test_parametro_contexto_es_puntero_a_maquina);
    RUN_TEST(test_todas_las_transiciones_tres_estados);
    RUN_TEST(test_accion_despacha_evento);
    UNITY_END();
    return 0;
}