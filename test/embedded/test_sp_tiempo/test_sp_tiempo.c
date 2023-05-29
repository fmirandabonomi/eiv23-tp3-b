#include <soporte_placa.h>
#include <unity.h>
#include <stm32f1xx.h>
#include <interfaces_impl/container_of.h>

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

static void test_SP_Tiempo_getMilisegundos(void){
    uint32_t const ciclos_por_ms = SystemCoreClock/1000;
    CycleCounter_resetValue();
    uint32_t const cuentaInicial = SP_Tiempo_getMilisegundos();
    for(uint32_t volatile i =0;i<ciclos_por_ms*10;++i);
    uint32_t const esperado = CycleCounter_getValue_ms();
    uint32_t const obtenido = SP_Tiempo_getMilisegundos()-cuentaInicial;
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(esperado+1,obtenido);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(esperado-1,obtenido);
}


static void accionCuentaTiempo(IAccion *accion);

typedef struct ContadorTiempo{
    IAccion accion;
    uint32_t volatile tiempo;
    bool finalizado;
}ContadorTiempo;

static void ContadorTiempo_init(ContadorTiempo *self,uint32_t tiempoInicial){
    static IAccion_VT const contadorTiempo_VT = {.ejecutar = accionCuentaTiempo};
    self->accion._vptr = &contadorTiempo_VT;
    self->tiempo = tiempoInicial;
    self->finalizado = false;
}

static IAccion *ContadorTiempo_asIAccion(ContadorTiempo *self){
    return &self->accion;
}

static bool ContadorTiempo_qCuentaFinalizada(ContadorTiempo *self){
    return self->finalizado;
}
static uint32_t ContadorTiempo_getTiempo(ContadorTiempo *self){
    return self->tiempo;
}

#define NUM_CONTADORES_TIEMPO 4

static ContadorTiempo contadoresTiempo[NUM_CONTADORES_TIEMPO];


static void ContadoresTiempo_init(void){
    uint32_t const t0 = SP_Tiempo_getMilisegundos();
    for(size_t i = 0;i<NUM_CONTADORES_TIEMPO;++i)
        ContadorTiempo_init(contadoresTiempo + i, t0);
}


static void accionCuentaTiempo(IAccion *accion){
    ContadorTiempo *self = container_of(accion,ContadorTiempo,accion);
    if (!self->finalizado){
        self->tiempo = SP_Tiempo_getMilisegundos() - self->tiempo;
        self->finalizado = true;
    }
}


static void test_un_timeout(void){
    ContadoresTiempo_init();
    bool const aceptado = SP_Tiempo_addTimeout(100,ContadorTiempo_asIAccion(contadoresTiempo));
    TEST_ASSERT_TRUE(aceptado);
    SP_Tiempo_delay(100);
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo));
    TEST_ASSERT_EQUAL_UINT32(100,ContadorTiempo_getTiempo(contadoresTiempo));
}

static void test_varios_timeouts(void){
    ContadoresTiempo_init();
    bool aceptado;
    aceptado = SP_Tiempo_addTimeout(100,ContadorTiempo_asIAccion(contadoresTiempo+3));
    TEST_ASSERT_TRUE(aceptado);
    aceptado = SP_Tiempo_addTimeout(30,ContadorTiempo_asIAccion(contadoresTiempo+0));
    TEST_ASSERT_TRUE(aceptado);
    aceptado = SP_Tiempo_addTimeout(150,ContadorTiempo_asIAccion(contadoresTiempo+1));
    TEST_ASSERT_TRUE(aceptado);
    aceptado = SP_Tiempo_addTimeout(73,ContadorTiempo_asIAccion(contadoresTiempo+2));
    TEST_ASSERT_TRUE(aceptado);

    SP_Tiempo_delay(150);

    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 0));
    TEST_ASSERT_EQUAL_UINT32(30,ContadorTiempo_getTiempo(contadoresTiempo + 0));
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 1));
    TEST_ASSERT_EQUAL_UINT32(150,ContadorTiempo_getTiempo(contadoresTiempo + 1));
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 2));
    TEST_ASSERT_EQUAL_UINT32(73,ContadorTiempo_getTiempo(contadoresTiempo + 2));
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 3));
    TEST_ASSERT_EQUAL_UINT32(100,ContadorTiempo_getTiempo(contadoresTiempo + 3));
}

static void test_varios_timeouts_iguales(void){
    ContadoresTiempo_init();
    bool aceptado;
    aceptado = SP_Tiempo_addTimeout(30,ContadorTiempo_asIAccion(contadoresTiempo+3));
    TEST_ASSERT_TRUE(aceptado);
    aceptado = SP_Tiempo_addTimeout(30,ContadorTiempo_asIAccion(contadoresTiempo+0));
    TEST_ASSERT_TRUE(aceptado);
    aceptado = SP_Tiempo_addTimeout(30,ContadorTiempo_asIAccion(contadoresTiempo+1));
    TEST_ASSERT_TRUE(aceptado);
    aceptado = SP_Tiempo_addTimeout(30,ContadorTiempo_asIAccion(contadoresTiempo+2));
    TEST_ASSERT_TRUE(aceptado);

    SP_Tiempo_delay(30);

    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 0));
    TEST_ASSERT_EQUAL_UINT32(30,ContadorTiempo_getTiempo(contadoresTiempo + 0));
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 1));
    TEST_ASSERT_EQUAL_UINT32(30,ContadorTiempo_getTiempo(contadoresTiempo + 1));
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 2));
    TEST_ASSERT_EQUAL_UINT32(30,ContadorTiempo_getTiempo(contadoresTiempo + 2));
    TEST_ASSERT_TRUE(ContadorTiempo_qCuentaFinalizada(contadoresTiempo + 3));
    TEST_ASSERT_EQUAL_UINT32(30,ContadorTiempo_getTiempo(contadoresTiempo + 3));

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
    RUN_TEST(test_SP_Tiempo_getMilisegundos);
    RUN_TEST(test_un_timeout);
    RUN_TEST(test_varios_timeouts);
    RUN_TEST(test_varios_timeouts_iguales);
    CycleCounter_deinit();
    UNITY_END();
    return 0;
}