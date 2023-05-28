#ifndef MAQUINAS_DE_PRUEBA_IMPL_H
#define MAQUINAS_DE_PRUEBA_IMPL_H
 
#include "inc/maquinas_de_prueba.h"
#include "inc/mensajes.h"
#include <me_impl/maquina_estado_impl.h>
#include <me_impl/me_estado_impl.h>
#include <unity.h>
#include <stddef.h>
#include <malloc.h>
#include <interf_impl/container_of.h>

#define EVENTO_ACUSE(estado_,mensaje_) {.mensaje=MSG_EVENTO_PROCESADO,.param.uVal = ((uintptr_t)(estado_)<<8) | (mensaje_) }

#endif
