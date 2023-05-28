#ifndef TEST_ASSERT_ARRAY_EVENTO_EQUAL_H
#define TEST_ASSERT_ARRAY_EVENTO_EQUAL_H
#include <unity.h>
#include <me_pub/me_evento.h>
#include "mensajes.h"
#define EVENTO_ACUSE(estado_,mensaje_) {.mensaje=MSG_EVENTO_PROCESADO,.param.uVal = ((uintptr_t)(estado_)<<8) | (mensaje_) }

#define TEST_ASSERT_EQUAL_EVENTO_ARRAY(expected,actual,num_elements) CustomAssertEqualEventoArray(expected,actual,num_elements,__LINE__)

static inline void CustomAssertEqualEventoArray(Evento const *expected, 
                                                Evento const *actual, 
                                                uint32_t const num_elements,
                                                UNITY_LINE_TYPE const lineNumber)
{
    for(uint32_t i=0;i<num_elements;++i){
        Evento const * const expected_i = expected + i;
        Evento const * const actual_i = actual + i;
        
        if(expected_i->mensaje == actual_i->mensaje && expected_i->param.uVal == actual_i->param.uVal)
            continue;
        
        static char const miembroMensaje[]=".mensaje ";
        static char const miembroParam[]  =".param   ";
        static char message[] = " At index[1234567890].mensaje ";
        enum{
            offset = sizeof(" At index[")-1,
            offset2= sizeof(" At index[1234567890]")-1,
            Nnr = offset2 - offset - 1,
            Nmb = sizeof(".mensaje ")
        };
        for (uint32_t k=0,a=i;k<Nnr;++k,a/=10){
            message[offset + 9 - k] = (char)(a % 10 + '0');
        }
        if(expected_i->mensaje != actual_i->mensaje){
            for (unsigned k=0;k<Nmb;++k){
                message[offset2+k]=miembroMensaje[k];
            }
            UNITY_TEST_ASSERT_EQUAL_UINT(expected_i->mensaje,actual_i->mensaje,lineNumber,message);
        }else{
            for (unsigned k=0;k<Nmb;++k){
                message[offset2+k]=miembroParam[k];
            }
            UNITY_TEST_ASSERT_EQUAL_PTR(expected_i->param.pVal,actual_i->param.pVal,lineNumber,message);
        }
    }
}

#endif
