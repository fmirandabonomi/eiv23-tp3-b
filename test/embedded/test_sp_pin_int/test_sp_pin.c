#include <soporte_placa.h>
#include <unity.h>

#define PIN1 SP_PB5
#define PIN1b SP_PA5
#define PIN2 SP_PB6

static struct Contadores {uint32_t c[4];} volatile contadores;


static void resetContadores(void){
    contadores = (struct Contadores){0};
}

void setUp(void){
    SP_Pin_setModo(PIN1,SP_PIN_SALIDA);
    SP_Pin_setModo(PIN1b,SP_PIN_SALIDA);
    SP_Pin_setModo(PIN2,SP_PIN_SALIDA);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1b,0);
    SP_Pin_write(PIN2,0);
    resetContadores();
}
void tearDown(void)
{
    SP_Pin_resetInterrupcion(PIN1);
    SP_Pin_resetInterrupcion(PIN1b);
    SP_Pin_resetInterrupcion(PIN2);
}

static void incrementaContador(uint32_t volatile *contador){
    ++(*contador);
}

static void test_config_unica_por_interrupcion(void){   
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    bool const r2 = SP_Pin_setInterrupcion(PIN1b,SP_PIN_INT_FLANCO_ASCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c+1);
    TEST_ASSERT_TRUE(r1);
    TEST_ASSERT_FALSE(r2);
}

static void test_interrupcion_flanco_ascendente(void){
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    TEST_ASSERT_TRUE(r1);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(2,contadores.c[0]);
}
static void test_interrupcion_flanco_descendente(void){
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_DESCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    TEST_ASSERT_TRUE(r1);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(1,contadores.c[0]);
}
static void test_interrupcion_ambos_flancos(void){
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_AMBOS_FLANCOS, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    TEST_ASSERT_TRUE(r1);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(3,contadores.c[0]);
}
static void test_si_config_un_pin_no_libera_otro(void){   
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    bool const r2 = SP_Pin_resetInterrupcion(PIN1b);
    TEST_ASSERT_TRUE(r1);
    TEST_ASSERT_FALSE(r2);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(1,contadores.c[0]);
}
static void test_no_interfiere_liberacion_pin_mismo_grupo(void){   
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    bool const r2 = SP_Pin_setInterrupcion(PIN2,SP_PIN_INT_FLANCO_ASCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c+1);
    bool const r3 = SP_Pin_resetInterrupcion(PIN1);
    TEST_ASSERT_TRUE(r1);
    TEST_ASSERT_TRUE(r2);
    TEST_ASSERT_TRUE(r3);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN2,1);
    TEST_ASSERT_EQUAL_UINT32(0,contadores.c[0]);
    TEST_ASSERT_EQUAL_UINT32(1,contadores.c[1]);
}
static void test_no_interfieren_interrupciones_pin_mismo_grupo(void){
    bool const r1 = SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_DESCENDENTE, (SP_Pin_IntHandler) incrementaContador,contadores.c);
    bool const r2 = SP_Pin_setInterrupcion(PIN2,SP_PIN_INT_AMBOS_FLANCOS, (SP_Pin_IntHandler) incrementaContador,contadores.c+1);

    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);

    SP_Pin_write(PIN2,1);
    SP_Pin_write(PIN2,0);

    TEST_ASSERT_EQUAL_UINT32(3,contadores.c[0]);
    TEST_ASSERT_EQUAL_UINT32(2,contadores.c[1]);
}
int main(void){
    SP_init();
    SP_Tiempo_delay(100);
    UNITY_BEGIN();
    RUN_TEST(test_config_unica_por_interrupcion);
    RUN_TEST(test_interrupcion_flanco_ascendente);
    RUN_TEST(test_interrupcion_flanco_descendente);
    RUN_TEST(test_interrupcion_ambos_flancos);
    RUN_TEST(test_si_config_un_pin_no_libera_otro);
    RUN_TEST(test_no_interfiere_liberacion_pin_mismo_grupo);
    RUN_TEST(test_no_interfieren_interrupciones_pin_mismo_grupo);
    UNITY_END();
    return 0;
}