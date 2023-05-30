#include <soporte_placa.h>
#include <unity.h>
#include "accion_contador.h"

#define PIN1 SP_PB5
#define PIN1b SP_PA5
#define PIN2 SP_PB6

#define NUM_CONTADORES 4
static Contador contadores[NUM_CONTADORES];

static void resetContadores(void){
    for (size_t i=0;i<NUM_CONTADORES;++i)
        Contador_init(contadores+i);
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

static void test_config_unica_por_interrupcion(void){   
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, Contador_getAccionIncrementaCuenta(contadores+0))
    );
    TEST_ASSERT_FALSE( // Recurso ocupado por el anterior setInterrupcion
        SP_Pin_setInterrupcion(PIN1b,SP_PIN_INT_FLANCO_ASCENDENTE, Contador_getAccionIncrementaCuenta(contadores+1))
    );
}

static void test_interrupcion_flanco_ascendente(void){
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, Contador_getAccionIncrementaCuenta(contadores+0))
    );
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(2,Contador_getCuenta(contadores+0));
}
static void test_interrupcion_flanco_descendente(void){
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_DESCENDENTE, Contador_getAccionIncrementaCuenta(contadores+0))
    );
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(1,Contador_getCuenta(contadores+0));
}
static void test_interrupcion_ambos_flancos(void){
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_AMBOS_FLANCOS, Contador_getAccionIncrementaCuenta(contadores+0))
    );
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(3,Contador_getCuenta(contadores+0));
}
static void test_si_config_un_pin_no_libera_otro(void){   
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE, Contador_getAccionIncrementaCuenta(contadores+0))
    );
    TEST_ASSERT_FALSE(
        SP_Pin_resetInterrupcion(PIN1b)
    );
    SP_Pin_write(PIN1,1);
    TEST_ASSERT_EQUAL_UINT32(1,Contador_getCuenta(contadores+0));
}
static void test_no_interfiere_liberacion_pin_mismo_grupo(void){   
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_ASCENDENTE,Contador_getAccionIncrementaCuenta(contadores+0))
    );
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN2,SP_PIN_INT_FLANCO_ASCENDENTE,Contador_getAccionIncrementaCuenta(contadores+1))
    );
    TEST_ASSERT_TRUE(
        SP_Pin_resetInterrupcion(PIN1)
    );
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN2,1);
    TEST_ASSERT_EQUAL_UINT32(0,Contador_getCuenta(contadores+0));
    TEST_ASSERT_EQUAL_UINT32(1,Contador_getCuenta(contadores+1));
}
static void test_no_interfieren_interrupciones_pin_mismo_grupo(void){
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN1,SP_PIN_INT_FLANCO_DESCENDENTE, Contador_getAccionIncrementaCuenta(contadores+0))
    );
    TEST_ASSERT_TRUE(
        SP_Pin_setInterrupcion(PIN2,SP_PIN_INT_AMBOS_FLANCOS,      Contador_getAccionIncrementaCuenta(contadores+1))
    );

    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);
    SP_Pin_write(PIN1,1);
    SP_Pin_write(PIN1,0);

    SP_Pin_write(PIN2,1);
    SP_Pin_write(PIN2,0);
    
    TEST_ASSERT_EQUAL_UINT32(3,Contador_getCuenta(contadores+0));
    TEST_ASSERT_EQUAL_UINT32(2,Contador_getCuenta(contadores+1));
}

int main(void){
    SP_init();
    SP_Tiempo_delay(500);
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