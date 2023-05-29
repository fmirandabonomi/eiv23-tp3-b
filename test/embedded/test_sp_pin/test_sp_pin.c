#include <soporte_placa.h>
#include <unity.h>
#include <stm32f1xx.h>

/* CONSTANTES */

/**
 * @brief Registros del puerto GPIO donde se realizarán las pruebas
 * 
 */
#define GPIO_PRUEBA GPIOB
/**
 * @brief Máscara del bit de habilitación de reloj en el registro 
 * RCC->APB2ENR correspondiente a GPIO_PRUEBA
 * 
 */
#define RCC_APB2ENR_GPIO_PRUEBA_ENABLE RCC_APB2ENR_IOPBEN
/**
 * @brief Máscara del bit de reset en el registro RCC->APB2ENR
 * correspondiente a GPIO_PRUEBA
 * 
 */
#define RCC_APB2RSTR_GPIO_PRUEBA_RESET RCC_APB2RSTR_IOPBRST

/**
 * @brief Número de pin del pin de prueba
 * 
 */
#define PIN_PRUEBA 9
/**
 * @brief Handle del pin sobre el que se realizarán las pruebas
 * de configuración, lectura y escritura
 * 
 */
#define HPIN_PRUEBA SP_PB9
/**
 * @brief Registro de configuración correspondiente al pin
 * PIN_PRUEBA
 * 
 */
#define PIN_PRUEBA_CR (GPIO_PRUEBA->CRH)
/**
 * @brief Offset en el PIN_PRUEBA_CR donde se encuentra el
 * bit menos significativo de los cuatro bits de 
 * configuración del pin PIN_PRUEBA
 * 
 */
#define PIN_PRUEBA_CR_Pos GPIO_CRH_MODE9_Pos
/**
 * @brief Máscara correspondiente PIN_PRUEBA en el registro
 * de salida
 * 
 */
#define ODR_PIN_PRUEBA_Msk (1<<PIN_PRUEBA)
/**
 * @brief Máscara correspondiente PIN_PRUEBA en el registro
 * de entrada
 * 
 */
#define IDR_PIN_PRUEBA_Msk (1<<PIN_PRUEBA)
/**
 * @brief Máscara correspondiente al bit de reset de
 * PIN_PRUEBA en el registro BRR
 * 
 */
#define BRR_PIN_PRUEBA_Msk (1<<PIN_PRUEBA)
/**
 * @brief Máscara correspondiente al bit de set de
 * PIN_PRUEBA en el registro BSRR
 * 
 */
#define BSRR_SET_PIN_PRUEBA_Msk (1<<PIN_PRUEBA)
/**
 * @brief Máscara correspondiente al bit de reset de
 * PIN_PRUEBA en el registro BSRR
 * 
 */
#define BSRR_RESET_PIN_PRUEBA_Msk (1<<(PIN_PRUEBA+16))

/* FUNCIONES DE UTILIDAD */
static uint32_t GpioPrueba_reset(void){
    RCC->APB2ENR |= RCC_APB2ENR_GPIO_PRUEBA_ENABLE;
    RCC->APB2RSTR = RCC_APB2RSTR_GPIO_PRUEBA_RESET;
    RCC->APB2RSTR = 0;
    RCC->APB2ENR &= ~RCC_APB2ENR_GPIO_PRUEBA_ENABLE;
}

static uint32_t PinPrueba_getBitsMode(void){
    return (PIN_PRUEBA_CR >> PIN_PRUEBA_CR_Pos) &0b0011;
}
static uint32_t PinPrueba_getBitsCnf(void){
    return ((PIN_PRUEBA_CR >> (PIN_PRUEBA_CR_Pos)) & 0b1100)>>2;
}

static bool PinPrueba_qEsSalidaOpenDrain(void){
    return PinPrueba_getBitsMode() !=0 && PinPrueba_getBitsCnf() == 0b01;
}
static bool PinPrueba_qEsSalidaPushPull(void){
    return PinPrueba_getBitsMode() !=0 && PinPrueba_getBitsCnf() == 0b00;
}
static bool PinPrueba_qEsEntradaFlotante(void){
    return PinPrueba_getBitsMode() == 0 && PinPrueba_getBitsCnf() == 0b01;
}
static bool PinPrueba_qEstadoOdr(void){
    return GPIO_PRUEBA->ODR & ODR_PIN_PRUEBA_Msk;
}
static bool PinPrueba_qEsEntradaPullUp(void){
    return    (PinPrueba_getBitsMode() == 0) 
           && (PinPrueba_getBitsCnf()  == 0b10) 
           && (PinPrueba_qEstadoOdr()  == 1);
}
static bool PinPrueba_qEsEntradaPullDown(void){
    return    (PinPrueba_getBitsMode() == 0) 
           && (PinPrueba_getBitsCnf()  == 0b10) 
           && (PinPrueba_qEstadoOdr()  == 0);
}


/* setUp y tearDown */

void setUp(void){
    GpioPrueba_reset();
}

void tearDown(void){
}

/* Definición de pruebas */

static void test_config_salida(void){
    TEST_ASSERT_TRUE(PinPrueba_qEsEntradaFlotante());
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA);
    TEST_ASSERT_TRUE(PinPrueba_qEsSalidaPushPull());
}

static void test_config_salidaOpenDrain(void){
    TEST_ASSERT_TRUE(PinPrueba_qEsEntradaFlotante());
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA_OPEN_DRAIN);
    TEST_ASSERT_TRUE(PinPrueba_qEsSalidaOpenDrain());
}
static void test_config_entrada(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA); // Verificado en otra prueba
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_ENTRADA);
    TEST_ASSERT_TRUE(PinPrueba_qEsEntradaFlotante());
}

static void test_config_entradaPullUp(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA); // Verificado en otra prueba
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_ENTRADA_PULLUP);
    TEST_ASSERT_TRUE(PinPrueba_qEsEntradaPullUp());
}

static void test_SP_Pin_setModo__SP_PIN_ENTRADA_PULLDN__DEBE_configurar_pin_como_ENTRADA_con_PULL_DOWN_INTERNO(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA); // Verificado en otra prueba
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_ENTRADA_PULLDN);
    TEST_ASSERT_TRUE(PinPrueba_qEsEntradaPullDown());
}

static void test_SP_Pin_write__TRUE__DEBE_establecer_pin_en_ALTO(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA); // Verificado en otra prueba
    TEST_ASSERT_EQUAL(0,PinPrueba_qEstadoOdr());
    SP_Pin_write(HPIN_PRUEBA,1);
    TEST_ASSERT_EQUAL(1,PinPrueba_qEstadoOdr());
}

static void test_SP_Pin_write__FALSE__DEBE_establecer_pin_en_BAJO(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA); // Verificado en otra prueba
    SP_Pin_write(HPIN_PRUEBA,1);               // Verificado en otra prueba
    SP_Pin_write(HPIN_PRUEBA,0);
    TEST_ASSERT_EQUAL(0,PinPrueba_qEstadoOdr());
}

static void test_SP_Pin_read__DEBE_retornar_TRUE_si_la_entrada_es_ALTA(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA);      // Verificado en otra prueba
    SP_Pin_write(HPIN_PRUEBA,1);                    // Verificado en otra prueba
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_ENTRADA);     // Verificado en otra prueba
    SP_Pin_write(HPIN_PRUEBA,0);                    // Verificado en otra prueba
    TEST_ASSERT_EQUAL(1,SP_Pin_read(HPIN_PRUEBA));
}

static void test_SP_Pin_read__DEBE_retornar_FALSE_si_la_entrada_es_BAJA(void){
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_SALIDA);      // Verificado en otra prueba
    SP_Pin_write(HPIN_PRUEBA,0);                    // Verificado en otra prueba
    SP_Pin_setModo(HPIN_PRUEBA,SP_PIN_ENTRADA);     // Verificado en otra prueba
    SP_Pin_write(HPIN_PRUEBA,1);                    // Verificado en otra prueba
    TEST_ASSERT_EQUAL(0,SP_Pin_read(HPIN_PRUEBA));
}

static void test_los_pines_asociados_a_JTAG__DEBEN_ser_desasociados_al_configurar(void){
    static SP_HPin const pinesJtag[] = {SP_PA15,SP_PB3,SP_PB4};
    static size_t const NUM_PINES_JTAG = sizeof(pinesJtag)/sizeof(*pinesJtag);
    for(size_t i=0;i<NUM_PINES_JTAG;++i){
        SP_HPin const hPin = pinesJtag[i];
        SP_Pin_setModo(hPin,SP_PIN_SALIDA);
        SP_Pin_write(hPin,0);
        SP_Pin_setModo(hPin,SP_PIN_ENTRADA);
        bool const valor_0 = SP_Pin_read(hPin);
        SP_Pin_setModo(hPin,SP_PIN_SALIDA);
        SP_Pin_write(hPin,1);
        SP_Pin_setModo(hPin,SP_PIN_ENTRADA);
        bool const valor_1 = SP_Pin_read(hPin);
        TEST_ASSERT_FALSE(valor_0);
        TEST_ASSERT_TRUE(valor_1);
    }
}


/* Ejecución de pruebas */

int main(void){
    SP_init();
    SP_Tiempo_delay(500);
    UNITY_BEGIN();
    RUN_TEST(test_config_salida);
    RUN_TEST(test_config_salidaOpenDrain);
    RUN_TEST(test_config_entrada);
    RUN_TEST(test_config_entradaPullUp);
    RUN_TEST(test_SP_Pin_setModo__SP_PIN_ENTRADA_PULLDN__DEBE_configurar_pin_como_ENTRADA_con_PULL_DOWN_INTERNO);
    RUN_TEST(test_SP_Pin_write__FALSE__DEBE_establecer_pin_en_BAJO);
    RUN_TEST(test_SP_Pin_write__TRUE__DEBE_establecer_pin_en_ALTO);
    RUN_TEST(test_SP_Pin_read__DEBE_retornar_TRUE_si_la_entrada_es_ALTA);
    RUN_TEST(test_SP_Pin_read__DEBE_retornar_FALSE_si_la_entrada_es_BAJA);
    RUN_TEST(test_los_pines_asociados_a_JTAG__DEBEN_ser_desasociados_al_configurar);
    UNITY_END();
    return 0;
}
