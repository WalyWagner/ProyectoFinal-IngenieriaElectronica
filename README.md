# Proyecto Final de Ingenieria Electronica

## Desarrollo de un instrumento medidor de fuerza de impacto y tiempo de respuesta para deportes de combate

**Autor:** Walter Sebastian Wagner  
**Departamento:** Departamento de Sistemas e Informatica  
**Institucion:** Facultad de Ciencias Exactas, Ingenieria y Agrimensura  

Firmware y recursos web del prototipo desarrollado para el Proyecto Final de Ingenieria Electronica. El sistema adquiere la senal de una celda de carga mediante un HX711, conserva los ensayos realizados y ofrece una interfaz web local para iniciar mediciones y analizar sus resultados.

> **Estado:** prototipo en desarrollo. Los valores deben interpretarse como resultados experimentales hasta completar la calibracion, validacion y caracterizacion metrologica.

## Caracteristicas

- ESP32 DOIT DevKit V1 con framework Arduino.
- Adquisicion mediante HX711, canal A y ganancia 128.
- Ventana de medicion de 3000 ms y hasta 500 muestras por ensayo.
- Conservacion en memoria de los ultimos cinco ensayos.
- Deteccion del pico de impacto y calculo de fuerza estimada en newtons.
- Indicacion de estado mediante LED rojo y verde.
- Inicio por pulsador fisico o desde la interfaz web.
- Punto de acceso Wi-Fi local y servidor HTTP en `192.168.4.1`.
- Interfaz con graficos SVG, tabla de muestras y logos institucionales en LittleFS.

## Arquitectura

```text
principal.cpp
  ControladorMedicion       Orquesta el ciclo de adquisicion
  ControladorPulsador       Lee el pulsador con antirrebote
  ControladorCeldaCarga     Lee el HX711 y la celda de carga
  AlmacenMediciones         Conserva tiempos y valores
  ControladorLedsEstado     Representa el estado del equipo
  ServidorWeb               Publica la interfaz y los resultados
```

Los parametros de hardware, adquisicion y conversion se encuentran en `src/configuracion/ConfiguracionMedicion.h`. La configuracion del punto de acceso se encuentra en `src/configuracion/ConfiguracionWifi.h`.

## Conexion de hardware

| Funcion | GPIO |
| --- | ---: |
| Datos HX711 | 16 |
| Reloj HX711 | 4 |
| LED verde | 22 |
| LED rojo | 23 |
| Pulsador | 32 |

El pulsador usa `INPUT_PULLUP` y es activo en nivel bajo. La configuracion actual contempla una celda de 500 kg y sensibilidad de 2 mV/V. Estos valores deben coincidir con el hardware instalado y con la calibracion realizada.

## Preparacion y compilacion

1. Instalar Visual Studio Code con PlatformIO.
2. Abrir esta carpeta como proyecto PlatformIO.
3. Revisar pines, sensibilidad y parametros de conversion.
4. Cambiar la red y la contrasena en `src/configuracion/ConfiguracionWifi.h` antes de distribuir el firmware.

```text
pio run
pio run --target upload
pio run --target uploadfs
```

`uploadfs` carga los logos de `data/` en LittleFS. El puerto serie informa la red creada y la direccion IP al iniciar la placa.

## Uso

1. Encender el ESP32 y esperar la inicializacion.
2. Conectar el dispositivo de consulta a la red Wi-Fi `ProyectoFinal`.
3. Abrir `http://192.168.4.1`.
4. Iniciar un ensayo desde la pagina o mediante el pulsador.
5. Aplicar el impacto durante la ventana de adquisicion.
6. Consultar pico, tiempo, grafico y muestras registradas.

El equipo conserva cinco ensayos en memoria volatil. Cuando se supera ese limite descarta el mas antiguo. Al reiniciar la placa, las mediciones se pierden.

## Interpretacion y limites

La interfaz muestra lectura cruda, senal en mV/V, instante del pico y fuerza estimada en N. La estimacion depende de la sensibilidad, capacidad, ganancia y calibracion configuradas. No debe utilizarse como medicion certificada sin validacion contra patrones o instrumentos de referencia.

## Estructura

```text
src/                 Firmware del instrumento
src/aplicacion/      Orquestacion y almacenamiento
src/configuracion/   Parametros de hardware y Wi-Fi
src/controladores/   Pulsador, celda de carga y LEDs
src/servicios/       Servidor web y visualizacion
data/                Logos cargados en LittleFS
platformio.ini       Configuracion de compilacion
```

## Autor y estado del repositorio

Proyecto Final de Ingenieria Electronica de Walter Sebastian Wagner, Departamento de Sistemas e Informatica, Facultad de Ciencias Exactas, Ingenieria y Agrimensura.

El repositorio se publicara inicialmente como **privado**. No se ha definido una licencia de distribucion.