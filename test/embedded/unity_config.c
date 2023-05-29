#include "unity_config.h"

#include <stm32f1xx.h>
#include <stdbool.h>

/* --- Funciones de utilidad internas --- */

static void GpioA_conectaReloj(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}
static void Usart1_conectaReloj(void){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}
static void Usart1_desconectaReloj(void){
    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
}
static void GpioA_setModoPin9_salida50MHzFuncionAlternativa(void){
    enum{ CONFIGURACION_PIN_SALIDA_RAPIDA_CONECTADO_A_PERIFERICO = 0b1011,
          POSICION_CONFIGURACION_PIN9_EN_CRH = GPIO_CRH_MODE9_Pos,
          MASCARA_CONFIGURACION_PIN = 0b1111};
    GPIOA->CRH = (GPIOA->CRH & ~(MASCARA_CONFIGURACION_PIN << POSICION_CONFIGURACION_PIN9_EN_CRH)) 
                | (CONFIGURACION_PIN_SALIDA_RAPIDA_CONECTADO_A_PERIFERICO << POSICION_CONFIGURACION_PIN9_EN_CRH);
}

static void Usart1_reset(void){
    RCC->APB2RSTR = RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR = 0;
}

static void Usart1_iniciaSalidaAsincronica_8bits_sinParidad_1stop_sinHandshake(unsigned long const baudrate){
    Usart1_reset();
    USART1->BRR = (SystemCoreClock+1) / baudrate;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}
static bool Usart1_qBufferEntradaDisponible(void){
    return USART1->SR & USART_SR_TXE;
}
static void Usart1_cargaByteParaTransmitir(uint8_t const b){
    USART1->DR = b;
}
static bool Usart1_qTransmisionCompletada(void){
    return USART1->SR & USART_SR_TC;
}
/* --- Implementación de funciones requeridas por Unity --- */

void unityOutputStart(unsigned long const baudrate){
    /* Determina frecuencia de reloj y actualiza SystemCoreClock */
    SystemCoreClockUpdate();
    GpioA_conectaReloj();
    Usart1_conectaReloj();
    // PA9 es el pin usado por USART1 como salida de transmisión por defecto
    GpioA_setModoPin9_salida50MHzFuncionAlternativa();
    Usart1_iniciaSalidaAsincronica_8bits_sinParidad_1stop_sinHandshake(baudrate);
}
void unityOutputChar(unsigned int const c){
    while(!Usart1_qBufferEntradaDisponible());
    Usart1_cargaByteParaTransmitir(c);
}
void unityOutputFlush(void){
    while(!Usart1_qTransmisionCompletada());
}
void unityOutputComplete(void){
    unityOutputFlush();
    Usart1_reset();
    Usart1_desconectaReloj();
}