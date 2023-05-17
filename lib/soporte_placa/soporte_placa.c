#include <soporte_placa.h>
#include <stm32f1xx.h> // SystemCoreClockUpdate, SystemCoreClock, SysTick_Config
#include <stdint.h>    // uint32_t

// Implementación

/* Inicialización general */

void SP_init(void){
    SP_Tiempo_init(); 
}
