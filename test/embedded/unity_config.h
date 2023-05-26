
#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#ifndef NULL
#ifndef __cplusplus
#define NULL (void*)0
#else
#define NULL 0
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Inicializa el puerto serie para transmitir los resultados
 * de las pruebas.
 * 
 * @param baudrate Símbolos por segundo
 */
void unityOutputStart(unsigned long baudrate);
/**
 * @brief Transmite un caracter por el puerto serie
 * 
 * @param c Caracter a transmitir (recortado a 8 bits)
 */
void unityOutputChar(unsigned int c);
/**
 * @brief Espera que finalice la transmisión en curso
 * 
 */
void unityOutputFlush(void);
/**
 * @brief Detiene y apaga el puerto serie
 * 
 */
void unityOutputComplete(void);

#define UNITY_OUTPUT_START()    unityOutputStart((unsigned long) 115200)
#define UNITY_OUTPUT_CHAR(c)    unityOutputChar(c)
#define UNITY_OUTPUT_FLUSH()    unityOutputFlush()
#define UNITY_OUTPUT_COMPLETE() unityOutputComplete()

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* UNITY_CONFIG_H */

