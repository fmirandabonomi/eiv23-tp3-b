#include <soporte_placa/sp_pin.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stm32f1xx.h>
#include <i_accion.h>

typedef struct SP_PinExti SP_PinExti;
typedef enum SP_LineaExti SP_LineaExti;
typedef enum SP_PuertoExti SP_PuertoExti;
typedef struct SP_DescriptorExti SP_DescriptorExti;

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

struct SP_DescriptorExti{
    IAccion *accion;
    SP_PinExti const *pin;
};


enum SP_LineaExti{
    SP_EXTI_0,
    SP_EXTI_1,
    SP_EXTI_2,
    SP_EXTI_3,
    SP_EXTI_4,
    SP_EXTI_5,
    SP_EXTI_6,
    SP_EXTI_7,
    SP_EXTI_8,
    SP_EXTI_9,
    SP_EXTI_10,
    SP_EXTI_11,
    SP_EXTI_12,
    SP_EXTI_13,
    SP_EXTI_14,
    SP_EXTI_15,
    SP_NUM_EXTI_SOPORTADAS
};

enum SP_PuertoExti{SP_EXTI_A,SP_EXTI_B,SP_EXTI_C,SP_EXTI_D,SP_EXTI_E,SP_EXTI_F,SP_EXTI_G,SP_EXTI_H};

struct SP_PinExti{
    SP_LineaExti lineaExti;
    SP_PuertoExti puertoExti;
};


static SP_PinExti const pinesExti[SP_NUM_PINES]={
    [SP_PA0 ] = {.lineaExti = SP_EXTI_0,  .puertoExti = SP_EXTI_A},
    [SP_PA1 ] = {.lineaExti = SP_EXTI_1,  .puertoExti = SP_EXTI_A},
    [SP_PA2 ] = {.lineaExti = SP_EXTI_2,  .puertoExti = SP_EXTI_A},
    [SP_PA3 ] = {.lineaExti = SP_EXTI_3,  .puertoExti = SP_EXTI_A},
    [SP_PA4 ] = {.lineaExti = SP_EXTI_4,  .puertoExti = SP_EXTI_A},
    [SP_PA5 ] = {.lineaExti = SP_EXTI_5,  .puertoExti = SP_EXTI_A},
    [SP_PA6 ] = {.lineaExti = SP_EXTI_6,  .puertoExti = SP_EXTI_A},
    [SP_PA7 ] = {.lineaExti = SP_EXTI_7,  .puertoExti = SP_EXTI_A},
    [SP_PA8 ] = {.lineaExti = SP_EXTI_8,  .puertoExti = SP_EXTI_A},
    [SP_PA9 ] = {.lineaExti = SP_EXTI_9,  .puertoExti = SP_EXTI_A},
    [SP_PA10] = {.lineaExti = SP_EXTI_10, .puertoExti = SP_EXTI_A},
    [SP_PA11] = {.lineaExti = SP_EXTI_11, .puertoExti = SP_EXTI_A},
    [SP_PA12] = {.lineaExti = SP_EXTI_12, .puertoExti = SP_EXTI_A},
    [SP_PA15] = {.lineaExti = SP_EXTI_15, .puertoExti = SP_EXTI_A},
    [SP_PB0 ] = {.lineaExti = SP_EXTI_0,  .puertoExti = SP_EXTI_B},
    [SP_PB1 ] = {.lineaExti = SP_EXTI_1,  .puertoExti = SP_EXTI_B},
    [SP_PB3 ] = {.lineaExti = SP_EXTI_3,  .puertoExti = SP_EXTI_B},
    [SP_PB4 ] = {.lineaExti = SP_EXTI_4,  .puertoExti = SP_EXTI_B},
    [SP_PB5 ] = {.lineaExti = SP_EXTI_5,  .puertoExti = SP_EXTI_B},
    [SP_PB6 ] = {.lineaExti = SP_EXTI_6,  .puertoExti = SP_EXTI_B},
    [SP_PB7 ] = {.lineaExti = SP_EXTI_7,  .puertoExti = SP_EXTI_B},
    [SP_PB9 ] = {.lineaExti = SP_EXTI_9,  .puertoExti = SP_EXTI_B},
    [SP_PB8 ] = {.lineaExti = SP_EXTI_8,  .puertoExti = SP_EXTI_B},
    [SP_PB10] = {.lineaExti = SP_EXTI_10, .puertoExti = SP_EXTI_B},
    [SP_PB11] = {.lineaExti = SP_EXTI_11, .puertoExti = SP_EXTI_B},
    [SP_PB12] = {.lineaExti = SP_EXTI_12, .puertoExti = SP_EXTI_B},
    [SP_PB13] = {.lineaExti = SP_EXTI_13, .puertoExti = SP_EXTI_B},
    [SP_PB14] = {.lineaExti = SP_EXTI_14, .puertoExti = SP_EXTI_B},
    [SP_PB15] = {.lineaExti = SP_EXTI_15, .puertoExti = SP_EXTI_B},
    [SP_PC13] = {.lineaExti = SP_EXTI_13, .puertoExti = SP_EXTI_C},
    [SP_PC14] = {.lineaExti = SP_EXTI_14, .puertoExti = SP_EXTI_C},
    [SP_PC15] = {.lineaExti = SP_EXTI_15, .puertoExti = SP_EXTI_C}
};


static SP_DescriptorExti descriptores[SP_NUM_EXTI_SOPORTADAS] = {0};

#define OPT_TYPE(base) struct Opt_##base {bool _isvalid; base _value;}
#define OPT_SET_NOTHING(opt) ((opt)._isvalid = false)
#define OPT_SET_VALUE(opt,val) ((opt)._isvalid = true, (opt)._value = (val))
#define OPT_ISVALID(opt) ((opt)._isvalid)
#define OPT_GET_VALUE(opt) ((opt)._value)
#define OPT_INIT_NOTHING() {._isvalid = false}
#define OPT_INIT_VALUE(val) {._isvalid = true, ._value = (val)}


typedef OPT_TYPE(IRQn_Type) Opt_IRQn_Type;

static Opt_IRQn_Type SP_PinExti_getIrq(SP_PinExti const * pin){
    Opt_IRQn_Type irq = OPT_INIT_NOTHING();
    switch(pin->lineaExti){
    case SP_EXTI_0:
        OPT_SET_VALUE(irq,EXTI0_IRQn);
    break;case SP_EXTI_1:
        OPT_SET_VALUE(irq,EXTI1_IRQn);
    break;case SP_EXTI_2:
        OPT_SET_VALUE(irq,EXTI2_IRQn);
    break;case SP_EXTI_3:
        OPT_SET_VALUE(irq,EXTI3_IRQn);
    break;case SP_EXTI_4:
        OPT_SET_VALUE(irq,EXTI4_IRQn);
    break;      case SP_EXTI_5:
    /*fallthru*/case SP_EXTI_6:
    /*fallthru*/case SP_EXTI_7:
    /*fallthru*/case SP_EXTI_8:
    /*fallthru*/case SP_EXTI_9:
        OPT_SET_VALUE(irq,EXTI9_5_IRQn);
    break;      case SP_EXTI_10:
    /*fallthru*/case SP_EXTI_11:
    /*fallthru*/case SP_EXTI_12:
    /*fallthru*/case SP_EXTI_13:
    /*fallthru*/case SP_EXTI_14:
    /*fallthru*/case SP_EXTI_15:
        OPT_SET_VALUE(irq,EXTI15_10_IRQn);
    break;default:
    break;
    }
    return irq;
}
/**
 * @brief Check if a IRQn is not used by any EXTI. If
 * the option type is not valid or the irqn is not assigned
 * to exti then returns false.
 * 
 * @param oIrq Option of IRQn
 * @return true IRQn is free
 * @return false OIrq is occupied/invalid/not assigned to EXTI
 */
static bool Opt_IRQn_Type_isFree(Opt_IRQn_Type oIrq){
    bool notFree = true;
    if (OPT_ISVALID(oIrq)){
        switch(OPT_GET_VALUE(oIrq)){
        case EXTI0_IRQn:
            notFree = descriptores[SP_EXTI_0].accion;
        break;case EXTI1_IRQn:
            notFree = descriptores[SP_EXTI_1].accion;
        break;case EXTI2_IRQn:
            notFree = descriptores[SP_EXTI_2].accion;
        break;case EXTI3_IRQn:
            notFree = descriptores[SP_EXTI_3].accion;
        break;case EXTI4_IRQn:
            notFree = descriptores[SP_EXTI_4].accion;
        break;case EXTI9_5_IRQn:
            notFree =    descriptores[SP_EXTI_9].accion
                     || descriptores[SP_EXTI_8].accion
                     || descriptores[SP_EXTI_7].accion
                     || descriptores[SP_EXTI_6].accion
                     || descriptores[SP_EXTI_5].accion;
        break;case EXTI15_10_IRQn:
            notFree =   descriptores[SP_EXTI_15].accion
                     || descriptores[SP_EXTI_14].accion
                     || descriptores[SP_EXTI_13].accion
                     || descriptores[SP_EXTI_12].accion
                     || descriptores[SP_EXTI_11].accion
                     || descriptores[SP_EXTI_10].accion;
        break;default:
        break;
        }
    }
    bool const isFree = ! notFree;
    return isFree;
}
static SP_PinExti const *pinDeHandle(SP_HPin hPin){
    return pinesExti + hPin;
}

static SP_DescriptorExti *SP_PinExti_getDescriptorExti(SP_PinExti const *self){
    return descriptores + self->lineaExti;
}

static bool SP_DescriptorExti_esLibre(SP_DescriptorExti *self){
    return !self->accion;
}

static void SP_DescriptorExti_libera(SP_DescriptorExti *self){
    *self = (SP_DescriptorExti){0};
}

static void SP_DescriptorExti_init(SP_DescriptorExti *self,SP_PinExti const *pin, IAccion * accion){
    self->accion = accion;
    self->pin = pin;
}

static void SP_PinExti_configAfio_(SP_PinExti const *self){
    enum {
        BITS_CONFIG_POR_LINEA = 4,
        MASCARA_BITS_CONFIG   = ((1 << BITS_CONFIG_POR_LINEA) - 1),
        LINEAS_POR_REGISTRO   = 16/BITS_CONFIG_POR_LINEA};
    
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    
    size_t const NUM_REGISTRO = self->lineaExti / LINEAS_POR_REGISTRO;
    size_t const OFFSET_REGISTRO = (self->lineaExti % LINEAS_POR_REGISTRO) * BITS_CONFIG_POR_LINEA;
    uint32_t const MASCARA_CFG =  MASCARA_BITS_CONFIG << OFFSET_REGISTRO;
    uint32_t const VALOR_CFG = (MASCARA_BITS_CONFIG & self->puertoExti) << OFFSET_REGISTRO;

    AFIO->EXTICR[NUM_REGISTRO] = (AFIO->EXTICR[NUM_REGISTRO] & ~MASCARA_CFG) | VALOR_CFG;
    
    RCC->APB2ENR &= ~RCC_APB2ENR_AFIOEN;
}

static void SP_PinExti_configExti_(SP_PinExti const *self, SP_Pin_IntFlanco flanco){
    EXTI->PR = 1 << self->lineaExti;
    EXTI->IMR |= 1 << self->lineaExti;
    switch(flanco){
    case SP_PIN_INT_FLANCO_ASCENDENTE:
        EXTI->RTSR |=  (1 << self->lineaExti);
        EXTI->FTSR &= ~(1 << self->lineaExti);
    break;case SP_PIN_INT_FLANCO_DESCENDENTE:
        EXTI->RTSR &= ~(1 << self->lineaExti);
        EXTI->FTSR |=  (1 << self->lineaExti);
    break;case SP_PIN_INT_AMBOS_FLANCOS:
        EXTI->RTSR |=  (1 << self->lineaExti);
        EXTI->FTSR |=  (1 << self->lineaExti);
    break;default:
    break;
    }
}

static void SP_PinExti_liberaExti_(SP_PinExti const *self){
    EXTI->IMR  &= ~(1 << self->lineaExti);
    EXTI->RTSR &= ~(1 << self->lineaExti);
    EXTI->FTSR &= ~(1 << self->lineaExti);
}
static void SP_PinExti_configNvic_(SP_PinExti const *self){
    Opt_IRQn_Type const oIrqn = SP_PinExti_getIrq(self);
    if (OPT_ISVALID(oIrqn)){
        IRQn_Type const irq = OPT_GET_VALUE(oIrqn);
        NVIC_EnableIRQ(irq);
    }
}

static void SP_PinExti_liberaNvic_(SP_PinExti const *self){
    Opt_IRQn_Type const oIrqn = SP_PinExti_getIrq(self);
    if (Opt_IRQn_Type_isFree(oIrqn)){
        IRQn_Type const irq = OPT_GET_VALUE(oIrqn);
        NVIC_DisableIRQ(irq);
    }
}

static void SP_PinExti_configura_(SP_PinExti const *self,SP_Pin_IntFlanco flanco){
    SP_PinExti_configAfio_(self);
    SP_PinExti_configExti_(self,flanco);
    SP_PinExti_configNvic_(self);
}

static void SP_PinExti_libera_(SP_PinExti const *const self){
    SP_PinExti_liberaExti_(self);
    SP_PinExti_liberaNvic_(self);
}

bool SP_Pin_setInterrupcion(SP_HPin hPin,SP_Pin_IntFlanco flanco,IAccion *accion){
    bool configurado = false;
    __disable_irq();
    if (hPin < SP_NUM_PINES){
        SP_PinExti const *const pin = pinDeHandle(hPin);
        SP_DescriptorExti *const desc = SP_PinExti_getDescriptorExti(pin);
        if(SP_DescriptorExti_esLibre(desc)){
            configurado = true;
            SP_DescriptorExti_init(desc,pin,accion);
            SP_PinExti_configura_(pin,flanco);
        }
    }
    __enable_irq();
    return configurado;
}


bool SP_Pin_resetInterrupcion(SP_HPin hPin){
    bool liberado = false;
    __disable_irq();
    if (hPin < SP_NUM_PINES){
        SP_PinExti const *const pin = pinDeHandle(hPin);
        SP_DescriptorExti *const desc = SP_PinExti_getDescriptorExti(pin);
        if (desc && desc->accion && pin == desc->pin){
            liberado = true;
            SP_DescriptorExti_libera(desc);
            SP_PinExti_libera_(pin);
        }
    }
    __enable_irq();
    return liberado;
}

static void procesa_exti(SP_LineaExti const linea){
    uint32_t const mascara = 1 << linea;
    if (EXTI->PR & mascara){
        SP_DescriptorExti const *const d = descriptores + linea;
        EXTI->PR = mascara; // Limpia bandera
        if (d->accion) IAccion_ejecuta(d->accion);
    }
}

void EXTI0_IRQHandler(void){
    procesa_exti(SP_EXTI_0);
}
void EXTI1_IRQHandler(void){
    procesa_exti(SP_EXTI_1);
}
void EXTI2_IRQHandler(void){
    procesa_exti(SP_EXTI_2);
}
void EXTI3_IRQHandler(void){
    procesa_exti(SP_EXTI_3);
}
void EXTI4_IRQHandler(void){
    procesa_exti(SP_EXTI_4);
}
void EXTI9_5_IRQHandler(void){
    procesa_exti(SP_EXTI_9);
    procesa_exti(SP_EXTI_8);
    procesa_exti(SP_EXTI_7);
    procesa_exti(SP_EXTI_6);
    procesa_exti(SP_EXTI_5);
}
void EXTI15_10_IRQHandler(void){
    procesa_exti(SP_EXTI_15);
    procesa_exti(SP_EXTI_14);
    procesa_exti(SP_EXTI_13);
    procesa_exti(SP_EXTI_12);
    procesa_exti(SP_EXTI_11);
    procesa_exti(SP_EXTI_10);
}
