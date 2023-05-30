#ifndef ACCION_CAPTURA_MILISEGUNDOS
#define ACCION_CAPTURA_MILISEGUNDOS

#include <i_accion.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct CapturaMilisegundos CapturaMilisegundos;

struct CapturaMilisegundos
{
    IAccion accion;
    uint32_t captura;
    bool capturado;
};


void CapturaMilisegundos_init(CapturaMilisegundos *self, uint32_t valorInicial);
IAccion *CapturaMilisegundos_getAccionCaptura(CapturaMilisegundos *self);
bool CapturaMilisegundos_qCapturado(CapturaMilisegundos *self);
uint32_t CapturaMilisegundos_getValor(CapturaMilisegundos *self);

#endif
