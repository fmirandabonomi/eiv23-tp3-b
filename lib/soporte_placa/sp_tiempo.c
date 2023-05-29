#include <soporte_placa/sp_tiempo.h>
#include <stdbool.h> // bool, true, false
#include <stdint.h>  // uint32_t
#include <stddef.h>  // size_t
#include <stm32f1xx.h> // __WFI

/* Temporización */


#ifndef SP_MAX_TIMEOUTS
#define SP_MAX_TIMEOUTS 4
#endif

/**
 * @brief Variable actualizada una vez por milisegundo en el handler
 * de interrupción del timer del sistema (SysTick)
 * 
 */
static uint32_t volatile ticks;
static uint32_t limiteRedondeo;

typedef struct SP_TimeoutDescriptor{
    uint32_t volatile tiempo;
    IAccion *volatile accion;
} SP_TimeoutDescriptor;

static struct SP_Timeouts{
    size_t numDescriptores;
    SP_TimeoutDescriptor descriptores[SP_MAX_TIMEOUTS];
}volatile timeouts;

void SP_Tiempo_init(void){
    // Ver documentación CMSIS
    // https://arm-software.github.io/CMSIS_5/Core/html/group__system__init__gr.html#gae0c36a9591fe6e9c45ecb21a794f0f0f
    SystemCoreClockUpdate();
    
    uint32_t const frecuencia_hertz = SystemCoreClock;
    uint32_t const cuentas_por_milisgundo = frecuencia_hertz/1000;

    // https://arm-software.github.io/CMSIS_5/Core/html/group__SysTick__gr.html#gabe47de40e9b0ad465b752297a9d9f427
    SysTick_Config(cuentas_por_milisgundo); // Configura SysTick y la interrupción
    limiteRedondeo = (SysTick->LOAD+1)/2;
}

uint32_t SP_Tiempo_getMilisegundos(void){
    return ticks;
}

void SP_Tiempo_delay(uint32_t tiempo){
    uint32_t const ticks_inicial = ticks;
    uint32_t tiempo_transcurrido = ticks - ticks_inicial;
    if (tiempo_transcurrido || (tiempo < UINT32_MAX && SysTick->VAL < limiteRedondeo)) ++tiempo; // Redondeo
    while(tiempo_transcurrido < tiempo){
        // https://arm-software.github.io/CMSIS_5/Core/html/group__intrinsic__CPU__gr.html#gaed91dfbf3d7d7b7fba8d912fcbeaad88
        __WFI();
        tiempo_transcurrido = ticks - ticks_inicial;
    }

}

static size_t buscaTimeout(IAccion *const accion){
    size_t i;
    for (i=0;i<timeouts.numDescriptores;++i){
        SP_TimeoutDescriptor volatile const * const d = timeouts.descriptores + i;
        if (d->accion == accion) break;
    }
    return i;
}

bool SP_Tiempo_addTimeout(uint32_t const tiempo,IAccion *const accion){
    bool hecho = false;
    __disable_irq();
    size_t const i = buscaTimeout(accion);
    if(i<SP_MAX_TIMEOUTS){
        SP_TimeoutDescriptor volatile * const td = timeouts.descriptores + i;
        td->tiempo = tiempo;
        td->accion = accion;
        if (i >= timeouts.numDescriptores)
            ++timeouts.numDescriptores;
        hecho = true;
    }
    __enable_irq();
    return hecho;
}

static bool actualizaEjecutaTimeout(SP_TimeoutDescriptor volatile *td){
    const uint32_t tiempo_restante = --td->tiempo;
    bool ejecutado = false;
    if(!tiempo_restante){
        IAccion_ejecuta((IAccion*)td->accion);
        ejecutado = true;
    }
    return ejecutado;
}
static void procesaYEliminaTimeoutsEnPosicion(size_t const i){
    SP_TimeoutDescriptor volatile *const td = timeouts.descriptores + i;
    bool pasoAdicional = false;
    size_t N = timeouts.numDescriptores;
    do{
        if (!td->tiempo){
            *td = timeouts.descriptores[N-1];
            --N;
            pasoAdicional = true;
        }else{
            pasoAdicional = actualizaEjecutaTimeout(td);
        }
    }while(pasoAdicional && i<N);
    timeouts.numDescriptores = N;
}
static void procesaTimeouts(void){
    for (size_t i=0;i<timeouts.numDescriptores;++i){
        procesaYEliminaTimeoutsEnPosicion(i);
    }

}

void SysTick_Handler(void){
    ++ticks;
    procesaTimeouts();
}
