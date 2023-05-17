#include <soporte_placa.h>
#include <unity.h>
#include <stm32f1xx.h>

static uint32_t rstCrh,rstCrl,rstOdr;


typedef struct DeltaRegs{
    uint32_t CRH;
    uint32_t CRL;
    uint32_t ODR;
}DeltaRegs;

static DeltaRegs getDeltaRegs(void){
    return (DeltaRegs){
        .CRH = rstCrh ^ GPIOB->CRH,
        .CRL = rstCrl ^ GPIOB->CRL,
        .ODR = rstOdr ^ GPIOB->ODR
    };
}
static void resetDeltaRegs(void){
    rstCrh = GPIOB->CRH;
    rstCrl = GPIOB->CRL;
    rstOdr = GPIOB->ODR;    
}

void setUp(){
    __disable_irq();
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2RSTR = RCC_APB2RSTR_IOPBRST;
    RCC->APB2RSTR = 0;
    resetDeltaRegs();
    RCC->APB2ENR &= ~RCC_APB2ENR_IOPBEN;
    __enable_irq();
}

void tearDown(){
}


    enum{OFFSET = GPIO_CRH_MODE9_Pos, 
         MASCARA_CRH = 0xFUL << OFFSET,
         BIT_ODR = 9,
         MASCARA_ODR = (1UL << BIT_ODR)};

static void test_config_salida(void){
    enum{BITS_MODO_ALGUNO_HIGH = 0b0011,
         BITS_MODO_LOW = 0b1100};
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA);
    resetDeltaRegs();
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA);
    bool const GPIOB_ON = RCC->APB2ENR & RCC_APB2ENR_IOPBEN;
    
    DeltaRegs const DELTA = getDeltaRegs();

    uint32_t const MODO = (GPIOB->CRH & MASCARA_CRH) >> OFFSET;

    TEST_ASSERT_TRUE(GPIOB_ON);
    TEST_ASSERT_BITS_LOW(~MASCARA_CRH,DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_FALSE(DELTA.ODR);
    TEST_ASSERT_TRUE(MODO & BITS_MODO_ALGUNO_HIGH);
    TEST_ASSERT_BITS_LOW(BITS_MODO_LOW, MODO);
}
static void test_config_salidaOpenDrain(void){
    enum{BITS_MODO_ALGUNO_HIGH = 0b0011,
         BITS_CFG = 0b1100,
         BITS_CFG_ESPERADO = 0b0100};
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA);
    resetDeltaRegs();
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA_OPEN_DRAIN);
    bool const GPIOB_ON = RCC->APB2ENR & RCC_APB2ENR_IOPBEN;
    
    DeltaRegs const DELTA = getDeltaRegs();

    uint32_t const MODO = (GPIOB->CRH & MASCARA_CRH) >> OFFSET;

    TEST_ASSERT_TRUE(GPIOB_ON);
    TEST_ASSERT_BITS_LOW(~MASCARA_CRH,DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_FALSE(DELTA.ODR);
    TEST_ASSERT_TRUE(MODO & BITS_MODO_ALGUNO_HIGH);
    TEST_ASSERT_BITS(BITS_CFG, BITS_CFG_ESPERADO,MODO);
}
static void test_config_entrada(void){
    enum{MODO_ESPERADO = 0b0100};
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA);
    resetDeltaRegs();
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA);
    bool const GPIOB_ON = RCC->APB2ENR & RCC_APB2ENR_IOPBEN;

    DeltaRegs const DELTA = getDeltaRegs();

    uint32_t const MODO = (GPIOB->CRH & MASCARA_CRH) >> OFFSET;

    TEST_ASSERT_TRUE(GPIOB_ON);
    TEST_ASSERT_BITS_LOW(~MASCARA_CRH,DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_FALSE(DELTA.ODR);
    TEST_ASSERT_EQUAL_HEX8(MODO_ESPERADO,MODO);
}

static void test_config_entradaPullUp(void){
    enum{MODO_ESPERADO = 0b1000};
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA);
    resetDeltaRegs();
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA_PULLUP);
    bool const GPIOB_ON = RCC->APB2ENR & RCC_APB2ENR_IOPBEN;

    DeltaRegs const DELTA = getDeltaRegs();
    uint32_t const ODR = GPIOB->ODR;

    uint32_t const MODO = (GPIOB->CRH & MASCARA_CRH) >> OFFSET;
    
    TEST_ASSERT_TRUE(GPIOB_ON);
    TEST_ASSERT_BITS_LOW(~MASCARA_CRH,DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_EQUAL_HEX8(MODO_ESPERADO,MODO);
    TEST_ASSERT_BITS_LOW(~MASCARA_ODR,DELTA.ODR);
    TEST_ASSERT_BIT_HIGH(BIT_ODR,ODR);
}

static void test_config_entradaPullDown(void){
    enum{MODO_ESPERADO = 0b1000};
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA_PULLDN);
    bool const GPIOB_ON = RCC->APB2ENR & RCC_APB2ENR_IOPBEN;

    DeltaRegs const DELTA = getDeltaRegs();
    uint32_t const ODR = GPIOB->ODR;

    uint32_t const MODO = (GPIOB->CRH & MASCARA_CRH) >> OFFSET;
    TEST_ASSERT_TRUE(GPIOB_ON);
    TEST_ASSERT_BITS_LOW(~MASCARA_CRH,DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_EQUAL_HEX8(MODO_ESPERADO,MODO);
    TEST_ASSERT_BITS_LOW(~MASCARA_ODR,DELTA.ODR);
    TEST_ASSERT_BIT_LOW(BIT_ODR,ODR);
}

static void test_write_1(void){
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA_OPEN_DRAIN);
    resetDeltaRegs();
    SP_Pin_write(SP_PB9,1);
    DeltaRegs DELTA = getDeltaRegs();
    uint32_t ODR = GPIOB->ODR;
    TEST_ASSERT_FALSE(DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_BITS_LOW(~MASCARA_ODR,DELTA.ODR);
    TEST_ASSERT_BIT_HIGH(BIT_ODR,DELTA.ODR);
    TEST_ASSERT_BIT_HIGH(BIT_ODR,ODR);
}

static void test_write_0(void){
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA_OPEN_DRAIN);
    SP_Pin_write(SP_PB9,1);
    resetDeltaRegs();
    SP_Pin_write(SP_PB9,0);
    DeltaRegs DELTA = getDeltaRegs();
    uint32_t ODR = GPIOB->ODR;
    TEST_ASSERT_FALSE(DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_BITS_LOW(~MASCARA_ODR,DELTA.ODR);
    TEST_ASSERT_BIT_LOW(BIT_ODR,ODR);
}

static void test_read_1(void){
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA);
    SP_Pin_write(SP_PB9,1);
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA);
    SP_Pin_write(SP_PB9,0);
    resetDeltaRegs();
    bool const valor_1 = SP_Pin_read(SP_PB9);
    DeltaRegs DELTA = getDeltaRegs();
    TEST_ASSERT_FALSE(DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_FALSE(DELTA.ODR);
    TEST_ASSERT_TRUE(valor_1);
}

static void test_read_0(void){
    SP_Pin_setModo(SP_PB9,SP_PIN_SALIDA);
    SP_Pin_write(SP_PB9,0);
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA);
    SP_Pin_write(SP_PB9,1);
    resetDeltaRegs();
    bool const valor_0 = SP_Pin_read(SP_PB9);
    DeltaRegs DELTA = getDeltaRegs();
    TEST_ASSERT_FALSE(DELTA.CRH);
    TEST_ASSERT_FALSE(DELTA.CRL);
    TEST_ASSERT_FALSE(DELTA.ODR);
    TEST_ASSERT_FALSE(valor_0);
}

static void test_pinesJtag(void){
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

int main(void){
    SP_init();
    for(unsigned volatile i=0;i<(8000000/13)/2;++i);
    UNITY_BEGIN();
    RUN_TEST(test_config_salida);
    RUN_TEST(test_config_salidaOpenDrain);
    RUN_TEST(test_config_entrada);
    RUN_TEST(test_config_entradaPullUp);
    RUN_TEST(test_config_entradaPullDown);
    RUN_TEST(test_write_0);
    RUN_TEST(test_write_1);
    RUN_TEST(test_read_1);
    RUN_TEST(test_read_0);
    RUN_TEST(test_pinesJtag);
    UNITY_END();
    return 0;
}
