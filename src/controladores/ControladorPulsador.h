// Interfaz de lectura del pulsador fisico con antirrebote.
#ifndef CONTROLADOR_PULSADOR_H
#define CONTROLADOR_PULSADOR_H

#include <stdint.h>

class ControladorPulsador {
public:
    explicit ControladorPulsador(uint8_t pin);

    void iniciar();
    bool fuePresionado();

private:
    uint8_t pin;
};

#endif
