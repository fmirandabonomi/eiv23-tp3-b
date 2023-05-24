#include <soporte_placa.h>
#include <maquina_estado.h>

#define LUZ_ON 0
#define LUZ_OFF 1
#define PULSADOR_ACTIVO 0
#define PULSADOR_NORMAL 1


int main(void){
    SP_init();
    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA_PULLUP);
    SP_Pin_setModo(SP_PIN_LED,SP_PIN_SALIDA);
    SP_Pin_write(SP_PIN_LED,LUZ_OFF);
    for (;;){
        while(SP_Pin_read(SP_PB9) != PULSADOR_ACTIVO);
        SP_Pin_write(SP_PIN_LED,LUZ_ON);
        SP_Tiempo_delay(60000);
        SP_Pin_write(SP_PIN_LED,LUZ_OFF);
    }
    return 0;
}