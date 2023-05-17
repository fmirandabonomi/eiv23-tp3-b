#include <soporte_placa.h>
#include <unity.h>
#include <stm32f1xx.h>

#define CICLOS_DIF 200UL

void setUp(){

}
void tearDown(){

}

static void CycleCounter_resetValue(void){
    DWT->CYCCNT = 0;
}
static uint32_t CycleCounter_getValue(void){
    return DWT->CYCCNT;
}


static uint32_t CycleCounter_getValue_ms(void){
    uint32_t const ciclos = CycleCounter_getValue();
    uint32_t const ciclos_por_milisegundo = SystemCoreClock/1000UL;
    return (ciclos+(ciclos_por_milisegundo+1UL)/2UL)/ciclos_por_milisegundo;
}


static void CycleCounter_init(void){
    __disable_irq();
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __enable_irq();
    CycleCounter_resetValue();
}
static void CycleCounter_deinit(void){
    __disable_irq();
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    __enable_irq();
    CycleCounter_resetValue();
}


static void test_SystemCoreClock_actualizado(void){
    uint32_t const SystemCoreClock_orig = SystemCoreClock;
    SystemCoreClockUpdate();
    TEST_ASSERT_EQUAL_UINT32(SystemCoreClock_orig,SystemCoreClock);
}

static void test_SP_delay_10ms_iniciaCuentaNueva(void){
    SysTick->VAL=0;
    CycleCounter_resetValue();
    SP_Tiempo_delay(10);
    uint32_t const tiempo = CycleCounter_getValue_ms();
    TEST_ASSERT_EQUAL_UINT32(10,tiempo);
}
static void test_SP_delay_10ms_iniciaAntesMediaCuenta(void){
    uint32_t const ciclos_ms = SystemCoreClock/1000;
    uint32_t const mediaCuenta = ciclos_ms/2;
    uint32_t const limite = mediaCuenta - ciclos_ms/8;
    CycleCounter_resetValue();
    SysTick->VAL=0;
    while(CycleCounter_getValue() < limite);
    CycleCounter_resetValue();
    SP_Tiempo_delay(10);
    uint32_t const tiempo = CycleCounter_getValue_ms();
    TEST_ASSERT_EQUAL_UINT32(10,tiempo);
}

static void test_SP_delay_10ms_iniciaMediaCuenta(void){
    uint32_t const ciclos_ms = SystemCoreClock/1000;
    uint32_t const mediaCuenta = ciclos_ms/2;
    uint32_t const limite = mediaCuenta;
    CycleCounter_resetValue();
    SysTick->VAL=0;
    while(CycleCounter_getValue() < limite);
    CycleCounter_resetValue();
    SP_Tiempo_delay(10);
    uint32_t const tiempo = CycleCounter_getValue_ms();
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(11,tiempo);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(10,tiempo);
}

static void test_SP_delay_10ms_iniciaDespuesMediaCuenta(void){
    uint32_t const ciclos_ms = SystemCoreClock/1000;
    uint32_t const mediaCuenta = ciclos_ms/2;
    uint32_t const limite = mediaCuenta+ciclos_ms/8;
    CycleCounter_resetValue();
    SysTick->VAL=0;
    while(CycleCounter_getValue() < limite);
    CycleCounter_resetValue();
    SP_Tiempo_delay(10);
    uint32_t const tiempo = CycleCounter_getValue_ms();
    TEST_ASSERT_EQUAL_UINT32(10,tiempo);
}

int main(void){
    SP_init();
    SP_Tiempo_delay(500);
    UNITY_BEGIN();
    CycleCounter_init();
    RUN_TEST(test_SystemCoreClock_actualizado);
    RUN_TEST(test_SP_delay_10ms_iniciaCuentaNueva);
    RUN_TEST(test_SP_delay_10ms_iniciaAntesMediaCuenta);
    RUN_TEST(test_SP_delay_10ms_iniciaMediaCuenta);
    RUN_TEST(test_SP_delay_10ms_iniciaDespuesMediaCuenta);
    CycleCounter_deinit();
    UNITY_END();
    return 0;
}