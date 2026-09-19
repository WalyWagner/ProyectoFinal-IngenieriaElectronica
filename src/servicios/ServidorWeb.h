// Interfaz del servicio que publica la pagina y los datos del instrumento.
#ifndef SERVIDOR_WEB_H
#define SERVIDOR_WEB_H

#include <WebServer.h>

#include "../aplicacion/AlmacenMediciones.h"

class ServidorWeb {
public:
    ServidorWeb(AlmacenMediciones& mediciones, double factorConversion);

    void iniciar();
    void atenderClientes();
    bool haySolicitudDeEnsayo();
    void establecerEstado(const char* estado);

private:
    void atenderPaginaPrincipal();
    String construirGraficoImpacto() const;
    String construirGraficoEnsayo(size_t ensayo) const;

    WebServer servidor;
    bool solicitudDeEnsayo;
    AlmacenMediciones& mediciones;
    double factorConversion;
    const char* estadoActual;
};

#endif
