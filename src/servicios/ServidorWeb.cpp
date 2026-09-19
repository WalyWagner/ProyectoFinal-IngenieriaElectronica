// Servicio HTTP que presenta la interfaz y los resultados de los ensayos.
#include "ServidorWeb.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "../configuracion/ConfiguracionWifi.h"

ServidorWeb::ServidorWeb(AlmacenMediciones& mediciones,
                         double factorConversion)
    : servidor(80),
      mediciones(mediciones),
      factorConversion(factorConversion),
            solicitudDeEnsayo(false),
      estadoActual("Iniciando sistema...") {
}

void ServidorWeb::iniciar() {
    // Monta el sistema de archivos que contiene el logo institucional.
    if (!LittleFS.begin(true)) {
        Serial.println("No se pudo montar LittleFS");
    }

    // Crea el punto de acceso local y desactiva el ahorro para favorecer móviles.
    WiFi.mode(WIFI_AP);
    WiFi.setSleep(false);
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1),
                     IPAddress(192, 168, 4, 1),
                     IPAddress(255, 255, 255, 0));
    WiFi.softAP(ConfiguracionWifi::NombreRed, ConfiguracionWifi::ClaveRed,
                1, false, 4);

    delay(500);

    String ip = WiFi.softAPIP().toString();
    Serial.println("\n=== Configuracion WiFi ===");
    Serial.print("Red: ");
    Serial.println(ConfiguracionWifi::NombreRed);
    Serial.print("Contrasena: ");
    Serial.println(ConfiguracionWifi::ClaveRed);
    Serial.print("IP del servidor: ");
    Serial.println(ip);
    Serial.print("URL: http://");
    Serial.println(ip);
    Serial.println("==========================\n");

    // Registra las rutas HTTP de la interfaz y de los datos del instrumento.
    servidor.on("/", HTTP_GET, [this]() {
        atenderPaginaPrincipal();
    });
    servidor.on("/logo.png", HTTP_GET, [this]() {
        File logo = LittleFS.open("/FCEIA-logo.png", "r");
        if (!logo) {
            servidor.send(404, "text/plain", "Logo no disponible");
            return;
        }
        servidor.streamFile(logo, "image/png");
        logo.close();
    });
    servidor.on("/dsi-logo.png", HTTP_GET, [this]() {
        File logo = LittleFS.open("/DSI-logo.png", "r");
        if (!logo) {
            servidor.send(404, "text/plain", "Logo DSI no disponible");
            return;
        }
        servidor.streamFile(logo, "image/png");
        logo.close();
    });
    servidor.on("/graficos", HTTP_GET, [this]() {
        servidor.sendHeader("Cache-Control", "max-age=5");
        servidor.send(200, "text/html; charset=utf-8",
                      construirGraficoImpacto());
    });
    servidor.on("/iniciar", HTTP_POST, [this]() {
        solicitudDeEnsayo = true;
        servidor.sendHeader("Location", "/");
        servidor.send(303);
    });
    servidor.begin();

    Serial.println("Servidor web HTTP iniciado");
}

bool ServidorWeb::haySolicitudDeEnsayo() {
    bool solicitud = solicitudDeEnsayo;
    solicitudDeEnsayo = false;
    return solicitud;
}

void ServidorWeb::atenderClientes() {
    // Atiende conexiones sin intervenir en el bucle crítico de adquisición.
    servidor.handleClient();
}

void ServidorWeb::establecerEstado(const char* estado) {
    // Actualiza el estado que se mostrara en la proxima vista.
    estadoActual = estado;
}

void ServidorWeb::atenderPaginaPrincipal() {
    // Genera la vista completa: identidad, controles, gráficos y créditos.
    String pagina;
    pagina.reserve(12000);
    pagina += "<!doctype html><html lang='es'><head>";
    pagina += "<meta charset='utf-8'>";
    pagina += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    pagina += "<title>Impacto medido</title>";
    // Estilos compactos y responsivos para pantallas de computadora y celular.
    pagina += "<style>body{font-family:Arial;margin:0;background:#eef2f5;color:#17202a}";
    pagina += "main{max-width:1000px;margin:30px auto;background:white;padding:28px;";
    pagina += "box-shadow:0 3px 14px #c7d0d8;border-radius:8px}";
    pagina += "header{border-bottom:3px solid #1677c8;margin-bottom:24px}";
    pagina += "header h1{margin:0;color:#123b5d}header p{margin:6px 0;color:#52606d}";
    pagina += ".identidad{display:flex;gap:18px;align-items:center}.logo{width:150px;";
    pagina += "height:auto;flex:none;object-fit:contain}.creditos{border-top:1px solid #d9e1e8;";
    pagina += "margin-top:34px;padding-top:18px;color:#52606d;line-height:1.7}";
    pagina += ".logo-dsi{width:150px;height:auto;flex:none;object-fit:contain;margin-left:auto}";
    pagina += "@media(max-width:760px){.identidad{flex-wrap:wrap}.logo-dsi{width:150px;";
    pagina += "margin-left:0}}";
    pagina += "svg{width:100%;height:auto;border:1px solid #ccd3da;background:#fff}";
    pagina += "table{width:100%;border-collapse:collapse;margin-top:14px;font-size:13px}";
    pagina += "th,td{padding:7px;border-bottom:1px solid #d9e1e8;text-align:right}";
    pagina += "th{background:#edf3f7;color:#123b5d}.tabla-muestras{overflow-x:auto}";
    pagina += ".dato{display:inline-block;margin-right:24px;margin-bottom:10px}";
    pagina += ".ensayo{margin-top:28px}.ensayo h2{color:#123b5d}";
    pagina += "button{background:#1677c8;color:white;border:0;padding:12px 18px;";
    pagina += "border-radius:5px;font-size:16px;cursor:pointer}button:hover{background:#125f9f}";
    pagina += ".leyenda{color:#52606d}.pico{display:inline-block;width:11px;height:11px;";
    pagina += "background:#d63b3b;border-radius:50%;margin-right:6px}";
    pagina += "h1{margin-top:0}</style></head><body><main>";
    pagina += "<header><div class='identidad'>";
    pagina += "<img class='logo' src='/logo.png' alt='Logo FCEIA'>";
    pagina += "<div><h1>DESARROLLO DE UN INSTRUMENTO MEDIDOR DE FUERZA DE "
              "IMPACTO Y TIEMPO DE RESPUESTA PARA DEPORTES DE COMBATE</h1>";
    pagina += "<p>Walter S. Wagner</p>";
    pagina += "<p>Facultad de Ciencias Exactas, Ingenieria y Agrimensura</p></div>";
    pagina += "<img class='logo-dsi' src='/dsi-logo.png' alt='Departamento de Sistemas e Informatica'>";
    pagina += "</div></header>";
    pagina += "<h2>Analisis de impactos</h2>";
    pagina += "<p><b>Estado:</b> ";
    pagina += estadoActual;
    pagina += "</p><div class='dato'><b>Ventana:</b> 100 ms antes y despues</div>";
    pagina += "<form action='/iniciar' method='post'><button type='submit'>Iniciar ensayo</button></form>";
    pagina += "<div id='graficos'>";
    pagina += construirGraficoImpacto();
    pagina += "</div>";
    pagina += "<footer class='creditos'><b>Director:</b> Prof. Ing. Marquez, Daniel<br>";
    pagina += "<b>Director externo:</b> Ing. Sosa, Ignacio<br>";
    pagina += "<b>Asesor:</b> Ing. Gavilan, Emiliano</footer>";
    pagina += "<script>function actualizarGraficos(){fetch('/graficos')";
    pagina += ".then(function(respuesta){return respuesta.text();})";
    pagina += ".then(function(html){document.getElementById('graficos').innerHTML=html;})";
    pagina += ".catch(function(){});}setInterval(actualizarGraficos,5000);</script>";
    pagina += "</main></body></html>";
    servidor.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
    servidor.send(200, "text/html; charset=utf-8", pagina);
}

String ServidorWeb::construirGraficoImpacto() const {
    if (mediciones.cantidadEnsayos() == 0) {
        return "<p>Aun no hay mediciones disponibles.</p>";
    }

    String graficos;
    graficos.reserve(30000);
    graficos += "<p class='dato'><b>Ensayos conservados:</b> ";
    graficos += mediciones.cantidadEnsayos();
    graficos += " de 5</p>";
    for (size_t ensayo = 0; ensayo < mediciones.cantidadEnsayos(); ensayo++) {
        graficos += construirGraficoEnsayo(ensayo);
    }
    return graficos;
}

String ServidorWeb::construirGraficoEnsayo(size_t ensayo) const {
    // Construye un SVG liviano con ventana temporal, grilla y pico marcado.
    size_t cantidad = mediciones.cantidadEnEnsayo(ensayo);
    if (cantidad == 0) return "";

    const unsigned long margenMs = 100;
    size_t indicePico = 0;
    long valorPico = 0;
    for (size_t indice = 0; indice < cantidad; indice++) {
        long valor = mediciones.valorCrudoEnEnsayo(ensayo, indice);
        long valorAbsoluto = valor < 0 ? -valor : valor;
        if (indice == 0 || valorAbsoluto > valorPico) {
            valorPico = valorAbsoluto;
            indicePico = indice;
        }
    }

    unsigned long tiempoPico = mediciones.tiempoEnEnsayo(ensayo, indicePico);
    unsigned long inicio = tiempoPico > margenMs ? tiempoPico - margenMs : 0;
    unsigned long fin = tiempoPico + margenMs;
    size_t primerIndice = 0;
    size_t ultimoIndice = cantidad - 1;
    while (primerIndice < ultimoIndice &&
           mediciones.tiempoEnEnsayo(ensayo, primerIndice) < inicio) {
        primerIndice++;
    }
    while (ultimoIndice > primerIndice &&
           mediciones.tiempoEnEnsayo(ensayo, ultimoIndice) > fin) {
        ultimoIndice--;
    }

    double minimo = 0;
    double maximo = 0;
    for (size_t indice = primerIndice; indice <= ultimoIndice; indice++) {
        double valor = mediciones.valorCrudoEnEnsayo(ensayo, indice) *
                       factorConversion * 2;
        if (valor < minimo) minimo = valor;
        if (valor > maximo) maximo = valor;
    }
    double amplitud = maximo - minimo;
    double margenVertical = amplitud > 0 ? amplitud * 0.10 : 1.0;
    minimo -= margenVertical;
    maximo += margenVertical;
    if (minimo > 0) minimo = 0;
    if (maximo < 0) maximo = 0;

    const double margenIzquierdo = 70.0;
    const double margenSuperior = 25.0;
    const double anchoUtil = 790.0;
    const double altoUtil = 330.0;
    unsigned long rangoTiempo = mediciones.tiempoEnEnsayo(ensayo, ultimoIndice) -
                                mediciones.tiempoEnEnsayo(ensayo, primerIndice);
    if (rangoTiempo == 0) rangoTiempo = 1;
    double rangoVertical = maximo - minimo;
    String grafico;
    grafico.reserve(6000);
    grafico += "<section class='ensayo'><h2>Golpe ";
    grafico += mediciones.numeroEnEnsayo(ensayo);
    grafico += " - Nro. de muestras: ";
    grafico += cantidad;
    grafico += "</h2><svg viewBox='0 0 900 420' role='img'";
    grafico += " aria-label='Grafico del impacto medido'>";
    for (int linea = 0; linea <= 5; linea++) {
        double y = margenSuperior + altoUtil * linea / 5.0;
        grafico += "<line x1='70' y1='";
        grafico += String(y, 1);
        grafico += "' x2='860' y2='";
        grafico += String(y, 1);
        grafico += "' stroke='#d9e1e8'/>";
    }
    for (int linea = 0; linea <= 5; linea++) {
        double x = margenIzquierdo + anchoUtil * linea / 5.0;
        grafico += "<line x1='";
        grafico += String(x, 1);
        grafico += "' y1='25' x2='";
        grafico += String(x, 1);
        grafico += "' y2='355' stroke='#e8edf1'/>";
    }
    grafico += "<line x1='70' y1='355' x2='860' y2='355' stroke='#52606d'/>";
    grafico += "<line x1='70' y1='25' x2='70' y2='355' stroke='#52606d'/>";
    grafico += "<polyline fill='none' stroke='#1677c8' stroke-width='3' points='";
    for (size_t indice = primerIndice; indice <= ultimoIndice; indice++) {
        double valor = mediciones.valorCrudoEnEnsayo(ensayo, indice) *
                       factorConversion * 2;
        double x = margenIzquierdo +
                   (mediciones.tiempoEnEnsayo(ensayo, indice) -
                    mediciones.tiempoEnEnsayo(ensayo, primerIndice)) * anchoUtil /
                       rangoTiempo;
        double y = margenSuperior + (maximo - valor) * altoUtil / rangoVertical;
        grafico += String(x, 1);
        grafico += ",";
        grafico += String(y, 1);
        grafico += " ";
    }
    double picoMv = mediciones.valorCrudoEnEnsayo(ensayo, indicePico) *
                    factorConversion * 2;
    double fuerzaMaximaN = fabs(picoMv) /
                           ConfiguracionMedicion::SensibilidadCeldaMvPorV *
                           ConfiguracionMedicion::CapacidadCeldaKg *
                           ConfiguracionMedicion::GravedadMs2;
    double picoX = margenIzquierdo +
                   (tiempoPico - mediciones.tiempoEnEnsayo(ensayo, primerIndice)) *
                       anchoUtil / rangoTiempo;
    double picoY = margenSuperior + (maximo - picoMv) * altoUtil / rangoVertical;
    grafico += "'/><circle cx='";
    grafico += String(picoX, 1);
    grafico += "' cy='";
    grafico += String(picoY, 1);
    grafico += "' r='6' fill='#d63b3b'/>";
    grafico += "<line x1='";
    grafico += String(picoX, 1);
    grafico += "' y1='25' x2='";
    grafico += String(picoX, 1);
    grafico += "' y2='355' stroke='#d63b3b' stroke-dasharray='5,5'/>";
    grafico += "<text x='75' y='18' font-size='14'>Fuerza (mV/V)</text>";
    grafico += "<text x='760' y='390' font-size='14'>tiempo (ms)</text>";
    grafico += "<text x='75' y='375' font-size='12'>";
    grafico += mediciones.tiempoEnEnsayo(ensayo, primerIndice);
    grafico += "</text><text x='820' y='375' font-size='12'>";
    grafico += mediciones.tiempoEnEnsayo(ensayo, ultimoIndice);
    grafico += "</text><text x='75' y='40' font-size='12'>";
    grafico += String(maximo, 3);
    grafico += "</text><text x='75' y='350' font-size='12'>";
    grafico += String(minimo, 3);
    grafico += "</text></svg><p class='leyenda'><span class='pico'></span>";
    grafico += "Fuerza maxima: <b>";
    grafico += String(fuerzaMaximaN, 3);
    grafico += " N</b> (";
    grafico += String(picoMv, 3);
    grafico += " mV/V) | Tiempo hasta el golpe: <b>";
    grafico += tiempoPico;
    grafico += " ms</b></p><div class='tabla-muestras'><table><thead><tr>";
    grafico += "<th>Tiempo (ms)</th><th>Lectura cruda</th><th>mV/V</th></tr></thead><tbody>";
    for (size_t indice = primerIndice; indice <= ultimoIndice; indice++) {
        double valorMv = mediciones.valorCrudoEnEnsayo(ensayo, indice) *
                         factorConversion * 2;
        grafico += "<tr><td>";
        grafico += mediciones.tiempoEnEnsayo(ensayo, indice);
        grafico += "</td><td>";
        grafico += mediciones.valorCrudoEnEnsayo(ensayo, indice);
        grafico += "</td><td>";
        grafico += String(valorMv, 3);
        grafico += "</td></tr>";
    }
    grafico += "</tbody></table></div></section>";
    return grafico;
}
