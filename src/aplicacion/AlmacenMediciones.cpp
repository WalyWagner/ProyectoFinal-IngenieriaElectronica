// Implementacion del almacenamiento estatico de los ensayos.
#include "AlmacenMediciones.h"

#include <string.h>

AlmacenMediciones::AlmacenMediciones()
        : cantidadEnsayosAlmacenados(0),
            ensayoActual(0),
            cantidadMuestras(0),
            proximoNumeroEnsayo(1) {
}

void AlmacenMediciones::limpiar() {
    // Reinicia la cantidad de muestras del ensayo actual.
    cantidadMuestras = 0;
}

void AlmacenMediciones::prepararNuevoEnsayo() {
    // Cuando se alcanza el limite, se conserva una ventana de los ensayos mas recientes.
    if (cantidadEnsayosAlmacenados >= ConfiguracionMedicion::MaximoEnsayos) {
        desplazarEnsayos();
        ensayoActual = ConfiguracionMedicion::MaximoEnsayos - 1;
    } else {
        ensayoActual = cantidadEnsayosAlmacenados;
        cantidadEnsayosAlmacenados++;
    }
    cantidadMuestras = 0;
    cantidadMuestrasEnsayo[ensayoActual] = 0;
    numerosEnsayos[ensayoActual] = proximoNumeroEnsayo++;
}

void AlmacenMediciones::desplazarEnsayos() {
    // Descarta el ensayo mas antiguo cuando se alcanza el limite configurado.
    const size_t cantidadRestante = ConfiguracionMedicion::MaximoEnsayos - 1;
    memmove(tiempos, tiempos[1],
            cantidadRestante * ConfiguracionMedicion::MaximoMuestras *
                sizeof(tiempos[0][0]));
    memmove(valoresCrudos, valoresCrudos[1],
            cantidadRestante * ConfiguracionMedicion::MaximoMuestras *
                sizeof(valoresCrudos[0][0]));
        memmove(cantidadMuestrasEnsayo, cantidadMuestrasEnsayo + 1,
            cantidadRestante * sizeof(cantidadMuestrasEnsayo[0]));
            memmove(numerosEnsayos, numerosEnsayos + 1,
                cantidadRestante * sizeof(numerosEnsayos[0]));
}

bool AlmacenMediciones::agregar(unsigned long tiempoMs, long valorCrudo) {
    // Las muestras se agregan al ensayo actual sin reservar memoria dinamica.
    if (cantidadMuestras >= ConfiguracionMedicion::MaximoMuestras) {
        return false;
    }

    tiempos[ensayoActual][cantidadMuestras] = tiempoMs;
    valoresCrudos[ensayoActual][cantidadMuestras] = valorCrudo;
    cantidadMuestras++;
    cantidadMuestrasEnsayo[ensayoActual] = cantidadMuestras;
    return true;
}

size_t AlmacenMediciones::cantidad() const {
    // Devuelve cuantas muestras tiene el ensayo actualmente activo.
    return cantidadMuestras;
}

unsigned long AlmacenMediciones::tiempoEn(size_t indice) const {
    return tiempos[ensayoActual][indice];
}

long AlmacenMediciones::valorCrudoEn(size_t indice) const {
    return valoresCrudos[ensayoActual][indice];
}

size_t AlmacenMediciones::cantidadEnsayos() const {
    return cantidadEnsayosAlmacenados;
}

unsigned long AlmacenMediciones::numeroEnEnsayo(size_t ensayo) const {
    if (ensayo >= cantidadEnsayosAlmacenados) return 0;
    return numerosEnsayos[ensayo];
}

size_t AlmacenMediciones::cantidadEnEnsayo(size_t ensayo) const {
    if (ensayo >= cantidadEnsayosAlmacenados) return 0;
    return cantidadMuestrasEnsayo[ensayo];
}

unsigned long AlmacenMediciones::tiempoEnEnsayo(size_t ensayo,
                                                size_t indice) const {
    return tiempos[ensayo][indice];
}

long AlmacenMediciones::valorCrudoEnEnsayo(size_t ensayo,
                                           size_t indice) const {
    return valoresCrudos[ensayo][indice];
}
