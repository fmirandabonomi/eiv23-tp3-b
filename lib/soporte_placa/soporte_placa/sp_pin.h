#ifndef SP_PIN_H
#define SP_PIN_H
#include <stdbool.h>
#include <i_accion.h>

/**
 * @brief Handles correspondientes a los pines de entrada/salida, para 
 * usar en el parámetro hPin (primer parámetro) de las funciones SP_Pin_xxx
 * 
 */
enum SP_Pines{
    SP_PA0,  ///< WKUP | USART2_CTS | ADC_IN0 | TIM2_CH1 | TIM2_ETR
    SP_PA1,  ///< USART2_RTS | ADC_IN1 | TIM2_CH2
    SP_PA2,  ///< USART2_TX  | ADC_IN2 | TIM2_CH3
    SP_PA3,  ///< USART2_RX  | ADC_IN3 | TIM2_CH4
    SP_PA4,  ///< SPI1_NSS  | ADC_IN4 | USART2_CK
    SP_PA5,  ///< SPI1_SCK  | ADC_IN5
    SP_PA6,  ///< SPI1_MISO | ADC_IN6 | TIM3_CH1 || TIM1_BKIN
    SP_PA7,  ///< SPI1_MOSI | ADC_IN7 | TIM3_CH2 || TIM1_CH1N
    SP_PA8,  ///< (5V) USART1_CK  | TIM1_CH1 | MCO
    SP_PA9,  ///< (5V) USART1_TX  | TIM1_CH2
    SP_PA10, ///< (5V) USART1_RX  | TIM1_CH3
    SP_PA11, ///< (5V) USART1_CTS | TIM1_CH4 | CANRX | USBDM
    SP_PA12, ///< (5V) USART1_RTS | TIM1_ETR | CANTX | USBDP
    /* Reservado para SWD: SP_PA13, ///< (5V) JTMS | SWDIO */
    /* Reservado para SWD: SP_PA14, ///< (5V) JTCK | SWCLK */
    SP_PA15, ///< (5V) JTDI || TIM2_CH1 | TIM2_ETR | SPI1_NSS
    SP_PB0,  ///< ADC_IN8 | TIM3_CH3 || TIM1_CH2N
    SP_PB1,  ///< ADC_IN9 | TIM3_CH4 || TIM1_CH3N
    /* No conectado a pin bluepill, solo a jumper BOOT1: SP_PB2,  ///< (5V) BOOT1 */
    SP_PB3,  ///< (5V) JTDO   || TIM2_CH2 | SPI1_SCK  | TRACESWO
    SP_PB4,  ///< (5V) JNTRST || TIM3_CH1 | SPI1_MISO
    SP_PB5,  ///< I2C1_SMBAI || TIM3_CH2 | SPI1_MOSI
    SP_PB6,  ///< (5V) I2C1_SCL   |  TIM4_CH1 | USART1_TX
    SP_PB7,  ///< (5V) I2C1_SDA   |  TIM4_CH2 | USART1_RX
    SP_PB9,  ///< (5V) TIM4_CH3 || I2C1_SCL | CANRX
    SP_PB8,  ///< (5V) TIM4_CH4 || I2C1_SDA | CANTX
    SP_PB10, ///< (5V) I2C2_SCL   | USART3_TX
    SP_PB11, ///< (5V) I2C2_SDA   | USART3_RX
    SP_PB12, ///< (5V) I2C2_SMBAI | USART3_CK | SPI2_NSS  | TIM1_BKIN
    SP_PB13, ///< (5V) USART3_CTS | SPI2_SCK  | TIM1_CH1N
    SP_PB14, ///< (5V) USART3_RTS | SPI2_MISO | TIM1_CH2N
    SP_PB15, ///< (5V) SPI2_MOSI | TIM1_CH3N
    SP_PC13, ///< TAMPER-RTC
    SP_PC14, ///< OSC32_IN
    SP_PC15, ///< OSC32_OUT
    SP_NUM_PINES
};

enum SP_PinesEspecial{
    SP_PIN_LED = SP_PC13  ///< LED verde integrado en la placa
};

/**
 * @brief Handle que representa un objeto Pin.
 * Toma valores de las constantes SP_Pines
 */
typedef unsigned SP_HPin;

/**
 * @brief Definición de modo de operación de un Pin
 * 
 */
typedef enum SP_Pin_Modo{
    SP_PIN_ENTRADA,          // Entrada flotante
    SP_PIN_ENTRADA_PULLUP,   // Entrada con resistencia pull-up interna
    SP_PIN_ENTRADA_PULLDN,   // Entrada con resistencia pull-down interna
    SP_PIN_SALIDA,           // Salida push-pull
    SP_PIN_SALIDA_OPEN_DRAIN // Salida con drenador abierto
}SP_Pin_Modo;


typedef enum SP_Pin_IntFlanco{
    SP_PIN_INT_FLANCO_ASCENDENTE,
    SP_PIN_INT_FLANCO_DESCENDENTE,
    SP_PIN_INT_AMBOS_FLANCOS
}SP_Pin_IntFlanco;

/**
 * @brief Configura el modo de un pin
 * 
 * @param hPin Handle al objeto Pin
 * @param modo Modo a configurar
 */
void SP_Pin_setModo(SP_HPin hPin,SP_Pin_Modo modo);

/**
 * @brief Lee el buffer de entrada de un Pin
 * 
 * @param hPin Handle al objeto Pin
 * @return true Entrada ALTA
 * @return false Entrada BAJA
 */
bool SP_Pin_read(SP_HPin hPin);

/**
 * @brief Escribe el buffer de salida de un Pin
 * 
 * @param hPin Handle al objeto Pin
 * @param valor True: Salida ALTA. False: Salida BAJA.
 */
void SP_Pin_write(SP_HPin hPin, bool valor);

/**
 * @brief Configura una interrupción por cambio en pin de entrada/salida.
 * Los pines de igual número en distintos puertos comparten una sola
 * línea de interrupción, solo uno puede estar configurado por vez (por ejemplo,
 * no pueden configurarse interrupciones en PA0 y PB0 al mismo tiempo).
 * 
 * La bandera de interrupción es borrada antes de ejecutar la acción. La ejecución
 * es efectuada desde una rutina de servicio de interrupción y por lo tanto
 * es ejecutada en modo HANDLER.
 * 
 * Es necesario eliminar la configuración original con resetInterrupción antes
 * de crear una nueva para la misma línea EXTI
 * 
 * @param hPin Handle al objeto Pin
 * @param flanco Flanco al que la interrupción es sensible
 * @param accion Accion a ejecutar
 * @return true Interrupción configurada
 * @return false Recurso ocupado
 */
bool SP_Pin_setInterrupcion(SP_HPin hPin,SP_Pin_IntFlanco flanco,IAccion *accion);

/**
 * @brief Elimina la configuración de interrupción de un pin y libera los recursos relacionados
 * 
 * @param hPin Handle al objeto Pin
 * @return true Recursos liberados
 * @return false No se configuró interrupción para el pin indicado
 */
bool SP_Pin_resetInterrupcion(SP_HPin hPin);

#endif