# Proyecto Final de Ingenieria Electronica

## Medidor de fuerza de impacto y tiempo de respuesta para deportes de combate

**Autor:** Walter Sebastian Wagner  
**Departamento:** Departamento de Sistemas e Informatica  
**Institucion:** Facultad de Ciencias Exactas, Ingenieria y Agrimensura  

Firmware y recursos web del prototipo de Walter Sebastian Wagner. El sistema adquiere una celda de carga mediante un HX711, detecta el pico de impacto y ofrece una interfaz web local para analizar los ensayos.

## Caracteristicas principales

- ESP32 DOIT DevKit V1 con framework Arduino.
- Adquisicion mediante HX711, canal A y ganancia 128.
- Ventana de medicion de 3000 ms y hasta 500 muestras por ensayo.
- Conservacion en memoria de los ultimos cinco ensayos, hasta 500 muestras cada uno.
- Deteccion del pico de impacto y calculo de fuerza estimada en newtons.
- Indicacion de estado mediante LED rojo y verde.
- Inicio por pulsador fisico o desde la interfaz web.
- Punto de acceso Wi-Fi local y servidor HTTP en `192.168.4.1`.
- Interfaz con graficos SVG, tabla de muestras y logos en LittleFS.

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

Los parametros de hardware, adquisicion, conversion y Wi-Fi se encuentran en `src/configuracion/`.

## Conexion de hardware

| Funcion | GPIO |
| --- | ---: |
| Datos HX711 | 16 |
| Reloj HX711 | 4 |
| LED verde | 22 |
| LED rojo | 23 |
| Pulsador | 32 |

El pulsador usa `INPUT_PULLUP` y es activo en nivel bajo. La celda configurada es de 500 kg y 2 mV/V; estos valores deben coincidir con el hardware y la calibracion.

## Compilar, cargar y usar

1. Abrir esta carpeta con Visual Studio Code y PlatformIO.
2. Revisar pines y parametros de conversion.
3. Cambiar la red y la contrasena en `src/configuracion/ConfiguracionWifi.h` antes de distribuir el firmware.

```text
pio run
pio run --target upload
pio run --target uploadfs
```

`uploadfs` carga los logos de `data/` en LittleFS. Para usar el equipo, conectar el dispositivo a la red `ProyectoFinal`, abrir `http://192.168.4.1`, iniciar un ensayo desde la web o el pulsador y aplicar el impacto. La IP se informa por puerto serie.

## Interpretacion y limites

La interfaz muestra lectura cruda, senal en mV/V, instante del pico y fuerza estimada en N. Los ensayos se almacenan en memoria volatil; al reiniciar la placa se pierden y, al superar cinco, se descarta el mas antiguo. La fuerza requiere calibracion y validacion contra un instrumento de referencia.

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

## Autor e institucion

- **Autor:** Walter Sebastian Wagner
- **Correo:** WAGNE@FCEIA.UNR.EDU.AR
- **Departamento:** Departamento de Sistemas e Informatica
- **Institucion:** Facultad de Ciencias Exactas, Ingenieria y Agrimensura

## Derechos y licencia

Este repositorio es publico para permitir la consulta del proyecto, pero todos los derechos sobre el codigo, la documentacion y los recursos originales pertenecen a Walter Sebastian Wagner. La autoria no se transfiere por la publicacion del repositorio.

La licencia aplicable se encuentra en [LICENSE](LICENSE). Se prohiben la comercializacion, la venta, el sublicenciamiento, la redistribucion y la creacion de productos o servicios derivados con fines comerciales sin autorizacion previa y por escrito del autor. Para solicitar permiso, escribir a WAGNE@FCEIA.UNR.EDU.AR.

## Descargo de responsabilidad

El proyecto se proporciona "tal cual", sin garantias expresas ni implicitas. El autor no se hace responsable por danos, perdidas, fallas, lesiones o cualquier otro problema que pueda producirse por el uso, la modificacion, la instalacion o la distribucion del proyecto. Toda utilizacion se realiza bajo responsabilidad exclusiva de quien la lleve a cabo.