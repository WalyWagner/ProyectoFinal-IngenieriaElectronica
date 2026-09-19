// Maquina de estados que coordina pulsador, espera, adquisicion y resultados.
#include "ControladorMedicion.h"

#include <Arduino.h>
#include <math.h>

#include "../configuracion/ConfiguracionMedicion.h"

ControladorMedicion::ControladorMedicion(
    ControladorPulsador& pulsador, ControladorLedsEstado& leds,
    ControladorCeldaCarga& celda, ServidorWeb& servidorWeb,
    AlmacenMediciones& mediciones,
    unsigned long duracionMs)
    : pulsador(pulsador),
      leds(leds),
      celda(celda),
    servidorWeb(servidorWeb),
    mediciones(mediciones),
      duracionMs(duracionMs),
      inicioMedicionMs(0),
    inicioEnsayoMs(0),
      estado(Estado::ESPERA) {
}

void ControladorMedicion::iniciar() {
    Serial.begin(115200);
    delay(100);
    randomSeed(micros());

    servidorWeb.iniciar();
    leds.iniciar();
    pulsador.iniciar();
    celda.iniciar();

    servidorWeb.establecerEstado("Esperando HX711...");
    while (!celda.estaLista()) {
        servidorWeb.atenderClientes();
        delay(200);
    }
    servidorWeb.establecerEstado(
        "HX711 listo. Presione el boton para tarar y medir.");
}

void ControladorMedicion::actualizar() {
    servidorWeb.atenderClientes();

    // Cada estado tiene una responsabilidad y una transicion explicita.
    switch (estado) {
        case Estado::ESPERA:
            atenderEspera();
            break;
        case Estado::ESPERANDO_INICIO:
            atenderEsperaInicio();
            break;
        case Estado::MIDIENDO:
            atenderMedicion();
            break;
        case Estado::INFORMANDO:
            atenderInforme();
            break;
    }
}

void ControladorMedicion::atenderEspera() {
    // Ambas interfaces generan la misma solicitud de inicio.
    bool solicitudFisica = pulsador.fuePresionado();
    bool solicitudWeb = servidorWeb.haySolicitudDeEnsayo();
    if (!solicitudFisica && !solicitudWeb) {
        return;
    }

    if (!celda.estaLista()) {
        servidorWeb.establecerEstado("HX711 no encontrado.");
        return;
    }

    celda.tarar();
    unsigned long espera = random(ConfiguracionMedicion::EsperaMinimaMs,
                                  ConfiguracionMedicion::EsperaMaximaMs + 1);
    inicioEnsayoMs = millis() + espera;
    servidorWeb.establecerEstado("Preparando ensayo...");
    estado = Estado::ESPERANDO_INICIO;
}

void ControladorMedicion::atenderEsperaInicio() {
    // Durante esta espera no se mide: el LED verde aun permanece apagado.
    if (static_cast<long>(millis() - inicioEnsayoMs) < 0) {
        return;
    }

    mediciones.prepararNuevoEnsayo();
    leds.establecerMidiendo();
    servidorWeb.establecerEstado("Midiendo...");
    inicioMedicionMs = millis();
    estado = Estado::MIDIENDO;
}

void ControladorMedicion::atenderMedicion() {
    // La red queda fuera de este bucle para maximizar la velocidad del HX711.
    // Durante la adquisicion se prioriza exclusivamente la lectura del HX711.
    // Las muestras se conservan en memoria para mostrarlas al finalizar.
    while ((millis() - inicioMedicionMs) < duracionMs &&
           mediciones.cantidad() < ConfiguracionMedicion::MaximoMuestras) {
        long valorCrudo = celda.leerValor();
        unsigned long tiempoMs = millis() - inicioMedicionMs;
        mediciones.agregar(tiempoMs, valorCrudo);
    }

    leds.establecerMedicionFinalizada();
    delay(300);
    leds.apagar();
    servidorWeb.establecerEstado(
        "Medicion finalizada. Resultados disponibles.");
    long valorPico = 0;
    unsigned long tiempoPico = 0;
    for (size_t indice = 0; indice < mediciones.cantidad(); indice++) {
        long valor = mediciones.valorCrudoEn(indice);
        if (indice == 0 || labs(valor) > labs(valorPico)) {
            valorPico = valor;
            tiempoPico = mediciones.tiempoEn(indice);
        }
    }
    double picoMv = valorPico *
                    ConfiguracionMedicion::FactorMilivoltiosPorVoltio * 2;
    double fuerzaN = fabs(picoMv) /
                     ConfiguracionMedicion::SensibilidadCeldaMvPorV *
                     ConfiguracionMedicion::CapacidadCeldaKg *
                     ConfiguracionMedicion::GravedadMs2;
    servidorWeb.atenderClientes();
    estado = Estado::INFORMANDO;
}

void ControladorMedicion::atenderInforme() {
    leds.establecerEspera();
    servidorWeb.establecerEstado("Listo. Presione el boton para repetir.");
    estado = Estado::ESPERA;
}
