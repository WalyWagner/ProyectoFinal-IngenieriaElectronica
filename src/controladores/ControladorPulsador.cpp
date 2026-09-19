// Lectura del pulsador fisico con antirrebote y deteccion de pulsacion unica.
#include "ControladorPulsador.h"

#include <Arduino.h>

ControladorPulsador::ControladorPulsador(uint8_t pin) : pin(pin) {
}

void ControladorPulsador::iniciar() {
    pinMode(pin, INPUT_PULLUP);
}

bool ControladorPulsador::fuePresionado() {
    // El pulsador es activo en nivel bajo y necesita antirrebote.
    if (digitalRead(pin) != LOW) {
        return false;
    }

    delay(30);
    if (digitalRead(pin) != LOW) {
        return false;
    }

    // Esperar la liberacion evita disparar varias mediciones por una pulsacion.
    while (digitalRead(pin) == LOW) {
        delay(5);
    }

    return true;
}
