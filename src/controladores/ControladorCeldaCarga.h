// Interfaz del controlador de la celda de carga conectada al HX711.
#ifndef CONTROLADOR_CELDA_CARGA_H
#define CONTROLADOR_CELDA_CARGA_H

#include <stdint.h>

class ControladorCeldaCarga {
public:
    ControladorCeldaCarga(uint8_t pinDatos, uint8_t pinReloj,
                          uint8_t ganancia);

    void iniciar();
    bool estaLista();
    void tarar();
    long leerValor();

private:
    long leerValorCrudo();
    void esperarDisponibilidad();

    uint8_t pinDatos;
    uint8_t pinReloj;
    uint8_t ganancia;
    uint8_t pulsosGanancia;
    long desplazamiento;
};

#endif
