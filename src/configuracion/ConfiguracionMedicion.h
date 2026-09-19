// Parametros de hardware, adquisicion y conversion del instrumento.
#ifndef CONFIGURACION_MEDICION_H
#define CONFIGURACION_MEDICION_H

#include <stddef.h>
#include <stdint.h>

namespace ConfiguracionMedicion {

// Pines conectados a cada elemento del sistema.
constexpr uint8_t PinDatosCelda = 16;
constexpr uint8_t PinRelojCelda = 4;
constexpr uint8_t PinLedVerde = 22;
constexpr uint8_t PinLedRojo = 23;
constexpr uint8_t PinPulsador = 32;

// Limites de la adquisicion. El buffer es estatico para evitar usar heap.
constexpr unsigned long DuracionMedicionMs = 3000;
constexpr size_t MaximoMuestras = 500;
constexpr size_t MaximoEnsayos = 5;
constexpr unsigned long EsperaMinimaMs = 2000;
constexpr unsigned long EsperaMaximaMs = 4000;
constexpr double CapacidadCeldaKg = 500.0;
constexpr double SensibilidadCeldaMvPorV = 2.0;
constexpr double GravedadMs2 = 9.80665;

// El canal A del HX711 trabaja con ganancia 128.
constexpr uint8_t GananciaCelda = 128;
constexpr double FactorMilivoltiosPorVoltio =
    1000.0 / (16777216.0 * GananciaCelda);

}

#endif
