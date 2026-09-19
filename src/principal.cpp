// Punto de entrada y composicion de las capas del instrumento.
#include <Arduino.h>

#include "aplicacion/ControladorMedicion.h"
#include "configuracion/ConfiguracionMedicion.h"
#include "servicios/ServidorWeb.h"

namespace {

// Composicion de las capas: el archivo principal no contiene logica de negocio.
ControladorPulsador pulsador(ConfiguracionMedicion::PinPulsador);
ControladorLedsEstado leds(ConfiguracionMedicion::PinLedVerde,
                           ConfiguracionMedicion::PinLedRojo);
ControladorCeldaCarga celda(ConfiguracionMedicion::PinDatosCelda,
                            ConfiguracionMedicion::PinRelojCelda,
                            ConfiguracionMedicion::GananciaCelda);
AlmacenMediciones mediciones;
ServidorWeb servidorWeb(mediciones,
                        ConfiguracionMedicion::FactorMilivoltiosPorVoltio);
ControladorMedicion sistema(pulsador, leds, celda, servidorWeb, mediciones,
                            ConfiguracionMedicion::DuracionMedicionMs);

}

void setup() {
    sistema.iniciar();
}

void loop() {
    sistema.actualizar();
}
