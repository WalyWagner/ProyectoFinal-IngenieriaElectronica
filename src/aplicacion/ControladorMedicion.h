// Maquina de estados que coordina el ciclo completo de un ensayo.
#ifndef CONTROLADOR_MEDICION_H
#define CONTROLADOR_MEDICION_H

#include "AlmacenMediciones.h"
#include "../controladores/ControladorCeldaCarga.h"
#include "../controladores/ControladorLedsEstado.h"
#include "../controladores/ControladorPulsador.h"
#include "../servicios/ServidorWeb.h"

class ControladorMedicion {
public:
    ControladorMedicion(ControladorPulsador& pulsador,
                        ControladorLedsEstado& leds,
                        ControladorCeldaCarga& celda,
                        ServidorWeb& servidorWeb,
                        AlmacenMediciones& mediciones,
                        unsigned long duracionMs);

    void iniciar();
    void actualizar();

private:
    enum class Estado {
        ESPERA,
        ESPERANDO_INICIO,
        MIDIENDO,
        INFORMANDO
    };

    void atenderEspera();
    void atenderEsperaInicio();
    void atenderMedicion();
    void atenderInforme();

    ControladorPulsador& pulsador;
    ControladorLedsEstado& leds;
    ControladorCeldaCarga& celda;
    ServidorWeb& servidorWeb;
    AlmacenMediciones& mediciones;
    unsigned long duracionMs;
    unsigned long inicioMedicionMs;
    unsigned long inicioEnsayoMs;
    Estado estado;
};

#endif
