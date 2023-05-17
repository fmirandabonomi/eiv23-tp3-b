#include "unity_config.h"

#include <stm32f1xx.h>

void unityOutputStart(unsigned long const baudrate){
    SystemCoreClockUpdate();
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    GPIOA->CRH = (GPIOA->CRH & ~(0xF << GPIO_CRH_MODE9_Pos)) | (0b1011 << GPIO_CRH_MODE9_Pos);
    USART1->BRR = (SystemCoreClock+1) / baudrate;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}
void unityOutputChar(unsigned int const c){
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}
void unityOutputFlush(void){
    while(!(USART1->SR & USART_SR_TC));
}
void unityOutputComplete(void){
    unityOutputFlush();
    USART1->CR1 &= ~(USART_CR1_TE | USART_CR1_UE);
    RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN);
}