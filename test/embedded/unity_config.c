#include "unity_config.h"

#include <stm32f1xx.h>
#include <stdbool.h>

/** 
 * @brief Causa que la función sea "insertada" en el punto donde es invocada en lugar
 * de generar un llamado a función 
 */
#define FUERZA_INLINE __attribute__((always_inline)) static inline


/* --- Funciones de utilidad internas --- */

FUERZA_INLINE void GpioA_conectaReloj(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}
FUERZA_INLINE void Usart1_conectaReloj(void){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}
FUERZA_INLINE void Usart1_desconectaReloj(void){
    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
}
FUERZA_INLINE void GpioA_setModoPin9_salida50MHzFuncionAlternativa(void){
    enum{ CONFIGURACION_PIN_SALIDA_RAPIDA_CONECTADO_A_PERIFERICO = 0b1011,
          POSICION_CONFIGURACION_PIN9_EN_CRH = GPIO_CRH_MODE9_Pos,
          MASCARA_CONFIGURACION_PIN = 0b1111};
    GPIOA->CRH = (GPIOA->CRH & ~(MASCARA_CONFIGURACION_PIN << POSICION_CONFIGURACION_PIN9_EN_CRH)) 
                | (CONFIGURACION_PIN_SALIDA_RAPIDA_CONECTADO_A_PERIFERICO << POSICION_CONFIGURACION_PIN9_EN_CRH);
}

FUERZA_INLINE void Usart1_reset(void){
    RCC->APB2RSTR = RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR = 0;
}

FUERZA_INLINE void Usart1_iniciaSalidaAsincronica_8bits_sinParidad_1stop_sinHandshake(unsigned long const baudrate){
    Usart1_reset();
    USART1->BRR = (SystemCoreClock+1) / baudrate;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}
FUERZA_INLINE bool Usart1_qBufferEntradaDisponible(void){
    return USART1->SR & USART_SR_TXE;
}
FUERZA_INLINE void Usart1_cargaByteParaTransmitir(uint8_t const b){
    USART1->DR = b;
}
FUERZA_INLINE bool Usart1_qTransmisionCompletada(void){
    return USART1->SR & USART_SR_TC;
}
/* --- Implementación de API --- */

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