// Driver local del HX711 para controlar la adquisicion sin librerias externas.
#include "ControladorCeldaCarga.h"

#include <Arduino.h>

ControladorCeldaCarga::ControladorCeldaCarga(uint8_t pinDatos,
                                             uint8_t pinReloj,
                                             uint8_t ganancia)
    : pinDatos(pinDatos),
      pinReloj(pinReloj),
      ganancia(ganancia),
      pulsosGanancia(1),
      desplazamiento(0) {
}

void ControladorCeldaCarga::iniciar() {
    // Configura los pines y selecciona los pulsos de ganancia del HX711.
    pinMode(pinReloj, OUTPUT);
    pinMode(pinDatos, INPUT);
    digitalWrite(pinReloj, LOW);

    // El HX711 selecciona el canal y la ganancia con pulsos posteriores a la lectura.
    switch (ganancia) {
        case 32:
            pulsosGanancia = 2;
            break;
        case 64:
            pulsosGanancia = 3;
            break;
        case 128:
        default:
            pulsosGanancia = 1;
            break;
    }
}

bool ControladorCeldaCarga::estaLista() {
    return digitalRead(pinDatos) == LOW;
}

void ControladorCeldaCarga::tarar() {
    // Promedia lecturas iniciales para eliminar la carga de reposo.
    // La tara promedia diez lecturas para estimar el valor de reposo.
    long suma = 0;
    for (uint8_t muestra = 0; muestra < 10; muestra++) {
        suma += leerValorCrudo();
        delay(0);
    }
    desplazamiento = suma / 10;
}

long ControladorCeldaCarga::leerValor() {
    // Devuelve la lectura compensada por la tara.
    return leerValorCrudo() - desplazamiento;
}

void ControladorCeldaCarga::esperarDisponibilidad() {
    while (!estaLista()) {
        delay(0);
    }
}

long ControladorCeldaCarga::leerValorCrudo() {
    // Lee los 24 bits del HX711 con interrupciones deshabilitadas.
    esperarDisponibilidad();

    // El HX711 entrega 24 bits en tres bytes, comenzando por el mas significativo.
    uint32_t valor = 0;
    noInterrupts();

    for (uint8_t indiceByte = 0; indiceByte < 3; indiceByte++) {
        uint8_t byteLeido = 0;
        for (uint8_t bit = 0; bit < 8; bit++) {
            digitalWrite(pinReloj, HIGH);
            delayMicroseconds(1);
            byteLeido = (byteLeido << 1) | digitalRead(pinDatos);
            digitalWrite(pinReloj, LOW);
            delayMicroseconds(1);
        }
        valor = (valor << 8) | byteLeido;
    }

    // Estos pulsos preparan el canal y la ganancia de la siguiente conversion.
    for (uint8_t pulso = 0; pulso < pulsosGanancia; pulso++) {
        digitalWrite(pinReloj, HIGH);
        delayMicroseconds(1);
        digitalWrite(pinReloj, LOW);
        delayMicroseconds(1);
    }

    interrupts();

    // Extender el signo convierte la lectura de 24 bits a long de 32 bits.
    if ((valor & 0x00800000UL) != 0) {
        valor |= 0xFF000000UL;
    }

    return static_cast<long>(valor);
}
