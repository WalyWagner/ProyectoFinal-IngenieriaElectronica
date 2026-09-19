// Interfaz del controlador que representa los estados mediante LED.
#ifndef CONTROLADOR_LEDS_ESTADO_H
#define CONTROLADOR_LEDS_ESTADO_H

#include <stdint.h>

class ControladorLedsEstado {
public:
    ControladorLedsEstado(uint8_t pinVerde, uint8_t pinRojo);

    void iniciar();
    void establecerEspera();
    void establecerMidiendo();
    void establecerMedicionFinalizada();
    void apagar();

private:
    uint8_t pinVerde;
    uint8_t pinRojo;
};

#endif
