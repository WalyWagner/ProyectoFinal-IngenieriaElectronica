// Almacena las muestras de los ultimos ensayos usando memoria estatica.
#ifndef ALMACEN_MEDICIONES_H
#define ALMACEN_MEDICIONES_H

#include <stddef.h>

#include "../configuracion/ConfiguracionMedicion.h"

class AlmacenMediciones {
public:
    AlmacenMediciones();

    void limpiar();
    void prepararNuevoEnsayo();
    bool agregar(unsigned long tiempoMs, long valorCrudo);
    size_t cantidad() const;
    unsigned long tiempoEn(size_t indice) const;
    long valorCrudoEn(size_t indice) const;
    size_t cantidadEnsayos() const;
    unsigned long numeroEnEnsayo(size_t ensayo) const;
    size_t cantidadEnEnsayo(size_t ensayo) const;
    unsigned long tiempoEnEnsayo(size_t ensayo, size_t indice) const;
    long valorCrudoEnEnsayo(size_t ensayo, size_t indice) const;

private:
    void desplazarEnsayos();

    size_t cantidadEnsayosAlmacenados;
    size_t ensayoActual;
    size_t cantidadMuestras;
    unsigned long proximoNumeroEnsayo;
    unsigned long numerosEnsayos[ConfiguracionMedicion::MaximoEnsayos];
    size_t cantidadMuestrasEnsayo[ConfiguracionMedicion::MaximoEnsayos];
    unsigned long tiempos[ConfiguracionMedicion::MaximoEnsayos]
                         [ConfiguracionMedicion::MaximoMuestras];
    long valoresCrudos[ConfiguracionMedicion::MaximoEnsayos]
                      [ConfiguracionMedicion::MaximoMuestras];
};

#endif
