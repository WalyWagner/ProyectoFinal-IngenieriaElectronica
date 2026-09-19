// Control visual de los estados del ensayo mediante los dos LED.
#include "ControladorLedsEstado.h"

#include <Arduino.h>

ControladorLedsEstado::ControladorLedsEstado(uint8_t pinVerde,
                                             uint8_t pinRojo)
    : pinVerde(pinVerde), pinRojo(pinRojo) {
}

void ControladorLedsEstado::iniciar() {
    // Configura las salidas y deja el equipo en espera.
    pinMode(pinVerde, OUTPUT);
    pinMode(pinRojo, OUTPUT);
    establecerEspera();
}

void ControladorLedsEstado::establecerEspera() {
    // Rojo: el equipo esta listo y espera una orden.
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinRojo, HIGH);
}

void ControladorLedsEstado::establecerMidiendo() {
    // Verde: comienza la ventana efectiva de adquisicion.
    digitalWrite(pinRojo, LOW);
    digitalWrite(pinVerde, HIGH);
}

void ControladorLedsEstado::establecerMedicionFinalizada() {
    // Ambos LED: la adquisicion termino y los resultados estan disponibles.
    digitalWrite(pinVerde, HIGH);
    digitalWrite(pinRojo, HIGH);
}

void ControladorLedsEstado::apagar() {
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinRojo, LOW);
}
