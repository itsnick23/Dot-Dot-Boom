#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "Partida.h"

// --- BACKEND: ESTRUCTURA DE CONFIGURACIÓN ---
struct ConfiguracionJuego {
    int fps;
    int anchoPantalla;
    int altoPantalla;
    bool pantallaCompleta;
    
    // Variables para Audio
    int volMaster;
    int volMusica;
    int volEfectos;
    bool estereo;

    // Variables para Gameplay
    bool screenShake;
    int intensidadShake;
    std::string idioma;
    std::string dificultad;
};

// --- FUNCIONES AUXILIARES DE TEXTO ---
void centrarTexto(sf::Text& texto, float x, float y) {
    sf::FloatRect rect = texto.getLocalBounds();
    texto.setOrigin({rect.position.x + rect.size.x / 2.0f, rect.position.y + rect.size.y / 2.0f});
    texto.setPosition({x, y});
}

void alinearIzquierda(sf::Text& texto, float x, float y) {
    sf::FloatRect rect = texto.getLocalBounds();
    texto.setOrigin({rect.position.x, rect.position.y + rect.size.y / 2.0f});
    texto.setPosition({x, y});
}

void alinearDerecha(sf::Text& texto, float x, float y) {
    sf::FloatRect rect = texto.getLocalBounds();
    texto.setOrigin({rect.position.x + rect.size.x, rect.position.y + rect.size.y / 2.0f});
    texto.setPosition({x, y});
}

// Formatear dinero con 1 o 2 decimales
std::string formatearDinero(float monto) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << monto;
    return ss.str();
}

// Color según la rareza de la chapa
sf::Color colorRareza(const std::string& rareza) {
    if (rareza == "Legendaria") return sf::Color(255, 215, 0);     // Oro
    if (rareza == "Epica")      return sf::Color(180, 80, 240);    // Violeta
    if (rareza == "Rara")       return sf::Color(70, 180, 255);    // Celeste
    return sf::Color(210, 215, 225);                               // Blanco/Gris
}

// Color del número de minas adyacentes
sf::Color colorNumeroMinas(int minas) {
    switch (minas) {
        case 1: return sf::Color(75, 160, 255);   // Azul
        case 2: return sf::Color(75, 210, 95);    // Verde
        case 3: return sf::Color(255, 75, 75);    // Rojo
        case 4: return sf::Color(170, 95, 255);   // Morado
        case 5: return sf::Color(240, 140, 30);   // Naranja
        case 6: return sf::Color(0, 205, 205);    // Cian
        case 7: return sf::Color(240, 240, 240);  // Blanco
        case 8: return sf::Color(160, 160, 160);  // Gris
        default: return sf::Color::Transparent;
    }
}

// --- FUNCIÓN PARA DIBUJAR BOTONES 3D ---
void dibujarBoton3D(sf::RenderWindow& window, sf::FloatRect rect, bool hover, sf::Color colorBase) {
    float x = rect.position.x; float y = rect.position.y;
    float w = rect.size.x; float h = rect.size.y;

    float sombraOffset = hover ? 6.f : 4.f;
    sf::RectangleShape sombra({w, h});
    sombra.setPosition({x + sombraOffset, y + sombraOffset});
    sombra.setFillColor(sf::Color(10, 10, 15, 140));
    window.draw(sombra);

    sf::RectangleShape cuerpo({w, h});
    cuerpo.setPosition({x, y});
    cuerpo.setFillColor(hover ? sf::Color(
        std::min(255, colorBase.r + 30),
        std::min(255, colorBase.g + 30),
        std::min(255, colorBase.b + 30)
    ) : colorBase);
    window.draw(cuerpo);

    float grosor = 3.f;
    sf::RectangleShape luzSup({w, grosor}); luzSup.setPosition({x, y}); luzSup.setFillColor(sf::Color(255, 255, 255, 70)); window.draw(luzSup);
    sf::RectangleShape luzIzq({grosor, h}); luzIzq.setPosition({x, y}); luzIzq.setFillColor(sf::Color(255, 255, 255, 70)); window.draw(luzIzq);
    sf::RectangleShape sombraInf({w, grosor}); sombraInf.setPosition({x, y + h - grosor}); sombraInf.setFillColor(sf::Color(0, 0, 0, 100)); window.draw(sombraInf);
    sf::RectangleShape sombraDer({grosor, h}); sombraDer.setPosition({x + w - grosor, y}); sombraDer.setFillColor(sf::Color(0, 0, 0, 100)); window.draw(sombraDer);

    sf::RectangleShape borde({w, h});
    borde.setPosition({x, y});
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineThickness(1.5f);
    borde.setOutlineColor(sf::Color(20, 20, 30));
    window.draw(borde);
}

// --- BARRA DE VOLUMEN (OPCIONES) ---
void dibujarBarraVolumen(sf::RenderWindow& window, float x, float y, int valor, int maximo) {
    sf::RectangleShape fondo({200.f, 16.f});
    fondo.setPosition({x, y});
    fondo.setFillColor(sf::Color(40, 45, 60));
    fondo.setOutlineThickness(2.f);
    fondo.setOutlineColor(sf::Color(20, 25, 35));
    window.draw(fondo);

    float porcentaje = static_cast<float>(valor) / maximo;
    sf::RectangleShape relleno({200.f * porcentaje, 16.f});
    relleno.setPosition({x, y});
    relleno.setFillColor(sf::Color(230, 140, 40));
    window.draw(relleno);

    sf::RectangleShape tirador({8.f, 20.f});
    tirador.setPosition({x + (200.f * porcentaje) - 4.f, y - 2.f});
    tirador.setFillColor(sf::Color(200, 200, 200));
    window.draw(tirador);

    for (int i = 1; i < 10; i++) {
        sf::RectangleShape marca({2.f, 4.f});
        marca.setPosition({x + (i * 20.f), y + 18.f});
        marca.setFillColor(sf::Color(100, 100, 100));
        window.draw(marca);
    }
}

// --- BOTONES ON/OFF ---
void dibujarBotonONOFF(sf::RenderWindow& window, float x, float y, bool estado) {
    sf::RectangleShape btnOn({35.f, 22.f});
    btnOn.setPosition({x, y});
    btnOn.setFillColor(estado ? sf::Color(60, 160, 60) : sf::Color(40, 80, 40));
    btnOn.setOutlineThickness(1.f);
    btnOn.setOutlineColor(sf::Color::Black);
    window.draw(btnOn);

    sf::RectangleShape btnOff({35.f, 22.f});
    btnOff.setPosition({x + 38.f, y});
    btnOff.setFillColor(!estado ? sf::Color(180, 50, 50) : sf::Color(90, 30, 30));
    btnOff.setOutlineThickness(1.f);
    btnOff.setOutlineColor(sf::Color::Black);
    window.draw(btnOff);
}

// --- ESTRUCTURA Y CÁLCULO UNIFICADO DE LA GEOMETRÍA DEL TABLERO ---
struct GeometriaTablero {
    int filas;
    int columnas;
    float cellSize;
    float cellPad;
    float gridW;
    float gridH;
    float startX;
    float startY;

    // Retorna true y calcula f, c si las coordenadas (mx, my) caen dentro de una casilla válida
    bool obtenerCelda(float mx, float my, int& fOut, int& cOut) const {
        if (mx < startX || mx >= startX + gridW || my < startY || my >= startY + gridH) {
            return false;
        }
        int c = static_cast<int>((mx - startX) / cellSize);
        int f = static_cast<int>((my - startY) / cellSize);
        if (f >= 0 && f < filas && c >= 0 && c < columnas) {
            float localX = mx - (startX + c * cellSize);
            float localY = my - (startY + f * cellSize);
            if (localX <= cellSize - cellPad && localY <= cellSize - cellPad) {
                fOut = f;
                cOut = c;
                return true;
            }
        }
        return false;
    }
};

GeometriaTablero calcularGeometriaTablero(const Tablero& tablero) {
    GeometriaTablero g;
    g.filas = tablero.getFilas();
    g.columnas = tablero.getColumnas();
    g.cellPad = 3.f;
    float maxW = 780.f;
    float maxH = 430.f;
    g.cellSize = std::floor(std::min(maxW / g.columnas, maxH / g.filas));
    if (g.cellSize < 16.f) g.cellSize = 16.f;
    g.gridW = g.columnas * g.cellSize;
    g.gridH = g.filas * g.cellSize;
    g.startX = 220.f + (maxW - g.gridW) / 2.0f;
    g.startY = 70.f + (maxH - g.gridH) / 2.0f;
    return g;
}

int main() {
    ConfiguracionJuego config;
    config.fps = 60; config.anchoPantalla = 1024; config.altoPantalla = 576;
    config.volMaster = 8; config.volMusica = 6; config.volEfectos = 9; config.estereo = true;
    config.screenShake = true; config.intensidadShake = 6;
    config.idioma = "Spanish"; config.dificultad = "Normal";

    sf::State estadoVentana = sf::State::Windowed; 
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(config.anchoPantalla), static_cast<unsigned int>(config.altoPantalla)}), "Dot Dot Boom", sf::Style::Default, estadoVentana);
    window.setFramerateLimit(config.fps);

    sf::Font fontTitulo;
    if (!fontTitulo.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        if (!fontTitulo.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            return -1;
        }
    }

    // --- INSTANCIA DEL MOTOR DE JUEGO (BACKEND) ---
    Partida partida;
    bool partidaActiva = false;
    bool modoZapato = false;
    std::string mensajeEstado = "Haz clic en una casilla para comenzar.";
    sf::Color colorMensaje = sf::Color(200, 220, 255);
    float temporizadorMensaje = 0.0f;
    float temporizadorShake = 0.0f;
    std::string nombreJugadorInput = "Soldado";
    bool puntajeGuardado = false;

    // Estados del juego:
    // 0: Menú Principal
    // 1: Juego en Tablero
    // 2: Opciones
    // 3: Tienda Táctica
    // 4: Fin de Juego / Victoria
    // 5: Ranking
    int estadoActual = 0; 
    int pestanaActual = 0; // 0: Graficos, 1: Audio, 2: Controles, 3: Gameplay
    sf::Clock reloj;
    sf::Clock deltaClock;

    // Botones del Menú Principal
    sf::FloatRect btnJugar({412.f, 220.f}, {200.f, 50.f});
    sf::FloatRect btnCargar({412.f, 280.f}, {200.f, 50.f});
    sf::FloatRect btnRanking({412.f, 340.f}, {200.f, 50.f});
    sf::FloatRect btnOpciones({412.f, 400.f}, {200.f, 50.f});
    sf::FloatRect btnSalir({412.f, 460.f}, {200.f, 50.f});

    // Pestañas Opciones
    float pestanaX = 50.f; float pestanaY = 150.f; float pestanaAlto = 80.f; float pestanaAncho = 160.f;
    sf::FloatRect tabGraficos({pestanaX, pestanaY}, {pestanaAncho, pestanaAlto});
    sf::FloatRect tabAudio({pestanaX, pestanaY + 85.f}, {pestanaAncho, pestanaAlto});
    sf::FloatRect tabControles({pestanaX, pestanaY + 170.f}, {pestanaAncho, pestanaAlto});
    sf::FloatRect tabGameplay({pestanaX, pestanaY + 255.f}, {pestanaAncho, pestanaAlto});
    sf::FloatRect btnVolver({412.f, 480.f}, {200.f, 50.f});

    // ==========================================================
    // ⚙️ PANEL DE CONTROL DE OPCIONES
    // ==========================================================
    float POS_TEXTO_FPS_X = 300.f;
    float POS_NUMERO_FPS_X = 520.f;
    float POS_BOTON_FPS_MENOS_X = 450.f; 
    float POS_BOTON_FPS_MAS_X = 570.f;   
    float FILA_FPS_Y = 250.f;        

    float POS_TEXTO_RES_X = 215.f;     
    float POS_NUMERO_RES_X = 534.f;    
    float POS_BOTON_RES_MENOS_X = 465.f;   
    float POS_BOTON_RES_MAS_X = 685.f;     
    float FILA_RES_Y = 365.f;          

    float AUDIO_TEXTO_X = 260.f;
    float AUDIO_BARRA_X = 520.f;
    float AUDIO_NUMERO_X = 540.f;
    float AUDIO_MASTER_Y = 150.f;
    float AUDIO_MUSIC_Y = 210.f;
    float AUDIO_SFX_Y = 270.f;
    float AUDIO_ESTEREO_Y = 330.f;

    // --- BUCLE PRINCIPAL ---
    while (window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();
        if (temporizadorMensaje > 0.0f) temporizadorMensaje -= dt;
        if (temporizadorShake > 0.0f) temporizadorShake -= dt;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // --- ENTRADA DE TEXTO (Para guardar nombre en Fin de Juego) ---
            if (estadoActual == 4 && !puntajeGuardado) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    char32_t c = textEvent->unicode;
                    if (c == 8) { // Backspace
                        if (!nombreJugadorInput.empty()) nombreJugadorInput.pop_back();
                    } else if (c >= 32 && c < 127 && nombreJugadorInput.length() < 12) {
                        nombreJugadorInput += static_cast<char>(c);
                    }
                }
            }

            // --- TECLAS RÁPIDAS ---
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (estadoActual == 1) { // En Juego
                    if (keyEvent->code == sf::Keyboard::Key::Z) {
                        modoZapato = !modoZapato;
                    } else if (keyEvent->code == sf::Keyboard::Key::T && partida.puedeAbrirTienda()) {
                        partida.abrirTienda();
                        estadoActual = 3;
                    } else if (keyEvent->code == sf::Keyboard::Key::Escape) {
                        estadoActual = 0; // Pausa / Menú
                    }
                } else if (estadoActual == 3) { // En Tienda
                    if (keyEvent->code == sf::Keyboard::Key::Escape || keyEvent->code == sf::Keyboard::Key::T) {
                        partida.cerrarTienda();
                        estadoActual = 1;
                    }
                } else if (estadoActual == 2 || estadoActual == 5) {
                    if (keyEvent->code == sf::Keyboard::Key::Escape) {
                        estadoActual = 0;
                    }
                }
            }

            // --- CLICS DEL RATÓN ---
            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mousePos = window.mapPixelToCoords(mouseButton->position);

                // ==========================================
                // 0: MENÚ PRINCIPAL
                // ==========================================
                if (estadoActual == 0) {
                    if (mouseButton->button == sf::Mouse::Button::Left) {
                        if (btnJugar.contains(mousePos)) {
                            if (!partidaActiva) {
                                partida.nuevaPartida();
                                partidaActiva = true;
                            }
                            modoZapato = false;
                            mensajeEstado = "Haz clic en una casilla para revelar el terreno.";
                            colorMensaje = sf::Color(200, 220, 255);
                            estadoActual = 1;
                        }
                        else if (btnCargar.contains(mousePos)) {
                            if (partida.cargarPartida("partida.txt")) {
                                partidaActiva = true;
                                modoZapato = false;
                                mensajeEstado = "Partida cargada exitosamente.";
                                colorMensaje = sf::Color(100, 255, 120);
                                estadoActual = 1;
                            } else {
                                mensajeEstado = "No se encontro partida.txt guardada.";
                                colorMensaje = sf::Color(255, 120, 120);
                                temporizadorMensaje = 2.5f;
                            }
                        }
                        else if (btnRanking.contains(mousePos)) {
                            estadoActual = 5;
                        }
                        else if (btnOpciones.contains(mousePos)) {
                            estadoActual = 2;
                        }
                        else if (btnSalir.contains(mousePos)) {
                            window.close();
                        }
                    }
                }

                // ==========================================
                // 1: EN JUEGO (TABLERO)
                // ==========================================
                else if (estadoActual == 1) {
                    // Botones HUD
                    sf::FloatRect btnZapatoHUD({20.f, 75.f}, {170.f, 40.f});
                    sf::FloatRect btnTiendaHUD({20.f, 125.f}, {170.f, 40.f});
                    sf::FloatRect btnGuardarHUD({20.f, 470.f}, {170.f, 35.f});
                    sf::FloatRect btnMenuHUD({20.f, 515.f}, {170.f, 35.f});

                    if (mouseButton->button == sf::Mouse::Button::Left) {
                        if (btnZapatoHUD.contains(mousePos)) {
                            modoZapato = !modoZapato;
                        }
                        else if (btnTiendaHUD.contains(mousePos) && partida.puedeAbrirTienda()) {
                            partida.abrirTienda();
                            estadoActual = 3;
                        }
                        else if (btnGuardarHUD.contains(mousePos)) {
                            if (partida.guardarPartida("partida.txt")) {
                                mensajeEstado = "Partida guardada en partida.txt";
                                colorMensaje = sf::Color(100, 255, 120);
                            } else {
                                mensajeEstado = "Error al guardar partida.";
                                colorMensaje = sf::Color(255, 100, 100);
                            }
                            temporizadorMensaje = 3.0f;
                        }
                        else if (btnMenuHUD.contains(mousePos)) {
                            estadoActual = 0; // Vuelve al menú
                        }
                    }

                    // Ronda Superada: Modal
                    if (partida.rondaSuperada() && !partida.partidaGanada()) {
                        sf::FloatRect btnSigRonda({412.f, 320.f}, {200.f, 55.f});
                        if (mouseButton->button == sf::Mouse::Button::Left && btnSigRonda.contains(mousePos)) {
                            partida.cobrarRonda();
                            modoZapato = false;
                            mensajeEstado = "Ronda " + std::to_string(partida.getRonda()) + " iniciada. ¡Suerte!";
                            colorMensaje = sf::Color(220, 240, 255);
                        }
                    } else if (!partida.estaTerminada()) {
                        // Clics sobre el tablero de celdas utilizando la misma geometría unificada
                        GeometriaTablero geo = calcularGeometriaTablero(partida.getTablero());
                        int f = -1, c = -1;
                        if (geo.obtenerCelda(mousePos.x, mousePos.y, f, c)) {
                            if (mouseButton->button == sf::Mouse::Button::Left) {
                                if (modoZapato) {
                                    if (partida.getZapatos() > 0) {
                                        int res = partida.usarZapato(f, c);
                                        if (res == 1) {
                                            mensajeEstado = "¡ZAPATO DESACTIVÓ UNA MINA!";
                                            colorMensaje = sf::Color(255, 215, 0);
                                        } else if (res == 0) {
                                            mensajeEstado = "Zona limpia revelada con el zapato.";
                                            colorMensaje = sf::Color(100, 255, 120);
                                        } else {
                                            mensajeEstado = "No se pudo usar el zapato en esta celda.";
                                            colorMensaje = sf::Color(255, 150, 150);
                                        }
                                        modoZapato = false;
                                    } else {
                                        mensajeEstado = "No te quedan zapatos disponibles.";
                                        colorMensaje = sf::Color(255, 100, 100);
                                        modoZapato = false;
                                    }
                                } else {
                                    int vidasAntes = partida.getVidas();
                                    bool ok = partida.jugarCelda(f, c);
                                    if (ok) {
                                        if (partida.getVidas() < vidasAntes) {
                                            mensajeEstado = "¡BOOM! Has pisado una mina. Vidas: " + std::to_string(partida.getVidas());
                                            colorMensaje = sf::Color(255, 80, 80);
                                            if (config.screenShake) temporizadorShake = 0.35f;
                                        } else {
                                            mensajeEstado = "Casilla segura descubierta.";
                                            colorMensaje = sf::Color(180, 220, 255);
                                        }
                                    }
                                }
                                // Verificar si la partida terminó
                                if (partida.estaTerminada()) {
                                    estadoActual = 4;
                                    puntajeGuardado = false;
                                }
                            } else if (mouseButton->button == sf::Mouse::Button::Right) {
                                partida.marcarCelda(f, c);
                            }
                        }
                    }
                }

                // ==========================================
                // 3: TIENDA TÁCTICA (COMPRA Y VENTA)
                // ==========================================
                else if (estadoActual == 3) {
                    if (mouseButton->button == sf::Mouse::Button::Left) {
                        // Botón Comprar Zapato (Columna Izquierda)
                        sf::FloatRect btnCompZapato({465.f, 142.f}, {115.f, 36.f});
                        if (btnCompZapato.contains(mousePos)) {
                            if (partida.comprarZapato()) {
                                mensajeEstado = "¡Zapato comprado!";
                                colorMensaje = sf::Color(100, 255, 100);
                            } else {
                                mensajeEstado = "No se pudo comprar (fondos insuficientes o limite).";
                                colorMensaje = sf::Color(255, 100, 100);
                            }
                        }

                        // Botones Comprar Chapas (Columna Izquierda, 3 slots)
                        for (int i = 0; i < partida.getCantTienda(); i++) {
                            Chapa* ch = partida.getChapaTienda(i);
                            if (ch == nullptr) continue;
                            float cardY = 195.f + i * 85.f;
                            sf::FloatRect btnCompChapa({465.f, cardY + 20.f}, {115.f, 36.f});
                            if (btnCompChapa.contains(mousePos)) {
                                if (partida.comprarChapa(ch->getId())) {
                                    mensajeEstado = "¡Compraste " + ch->getNombre() + "!";
                                    colorMensaje = sf::Color(255, 215, 0);
                                } else {
                                    mensajeEstado = "No tienes dinero suficiente o tu inventario esta lleno.";
                                    colorMensaje = sf::Color(255, 100, 100);
                                }
                            }
                        }

                        // Botones Vender Chapas del Inventario (Columna Derecha, sincronizados exactamente con el dibujo)
                        for (int i = 0; i < partida.getCantChapas(); i++) {
                            Chapa* ch = partida.getChapaEquipada(i);
                            if (ch == nullptr) continue;
                            float invY = 135.f + i * 62.f;
                            sf::FloatRect btnVenderChapa({815.f, invY + 11.f}, {105.f, 32.f});
                            if (btnVenderChapa.contains(mousePos)) {
                                std::string nom = ch->getNombre();
                                float val = ch->getCosto() * 0.5f;
                                if (partida.venderChapa(i)) {
                                    mensajeEstado = "Vendiste " + nom + " por $" + formatearDinero(val);
                                    colorMensaje = sf::Color(100, 255, 120);
                                    break;
                                }
                            }
                        }

                        // Botón Salir de la Tienda
                        sf::FloatRect btnSalirTienda({412.f, 498.f}, {200.f, 42.f});
                        if (btnSalirTienda.contains(mousePos)) {
                            partida.cerrarTienda();
                            estadoActual = 1;
                        }
                    }
                }

                // ==========================================
                // 4: FIN DE JUEGO / VICTORIA
                // ==========================================
                else if (estadoActual == 4) {
                    if (mouseButton->button == sf::Mouse::Button::Left) {
                        sf::FloatRect btnGuardarRec({540.f, 340.f}, {160.f, 45.f});
                        sf::FloatRect btnReintentar({310.f, 430.f}, {180.f, 50.f});
                        sf::FloatRect btnIrMenu({530.f, 430.f}, {180.f, 50.f});

                        if (!puntajeGuardado && btnGuardarRec.contains(mousePos)) {
                            std::string nom = nombreJugadorInput.empty() ? "Soldado" : nombreJugadorInput;
                            partida.guardarPuntaje("scores.txt", nom);
                            partida.borrarPartida("partida.txt");
                            puntajeGuardado = true;
                            mensajeEstado = "Puntaje registrado con exito.";
                            colorMensaje = sf::Color(100, 255, 120);
                        }
                        else if (btnReintentar.contains(mousePos)) {
                            partida.nuevaPartida();
                            partidaActiva = true;
                            modoZapato = false;
                            mensajeEstado = "Nueva partida iniciada.";
                            colorMensaje = sf::Color(200, 220, 255);
                            estadoActual = 1;
                        }
                        else if (btnIrMenu.contains(mousePos)) {
                            partidaActiva = false;
                            estadoActual = 0;
                        }
                    }
                }

                // ==========================================
                // 5: RANKING
                // ==========================================
                else if (estadoActual == 5) {
                    sf::FloatRect btnVolverRank({412.f, 490.f}, {200.f, 45.f});
                    if (mouseButton->button == sf::Mouse::Button::Left && btnVolverRank.contains(mousePos)) {
                        estadoActual = 0;
                    }
                }

                // ==========================================
                // 2: OPCIONES
                // ==========================================
                else if (estadoActual == 2) {
                    if (mouseButton->button == sf::Mouse::Button::Left) {
                        if (tabGraficos.contains(mousePos)) pestanaActual = 0;
                        if (tabAudio.contains(mousePos)) pestanaActual = 1;
                        if (tabControles.contains(mousePos)) pestanaActual = 2;
                        if (tabGameplay.contains(mousePos)) pestanaActual = 3;
                        if (btnVolver.contains(mousePos)) estadoActual = 0;

                        // Pestaña Gráficos
                        if (pestanaActual == 0) {
                            sf::FloatRect clickFPSMenos({POS_BOTON_FPS_MENOS_X, FILA_FPS_Y - 25.f}, {50.f, 50.f});
                            sf::FloatRect clickFPSMas({POS_BOTON_FPS_MAS_X, FILA_FPS_Y - 25.f}, {50.f, 50.f});
                            sf::FloatRect clickResMenos({POS_BOTON_RES_MENOS_X, FILA_RES_Y - 25.f}, {50.f, 50.f});
                            sf::FloatRect clickResMas({POS_BOTON_RES_MAS_X, FILA_RES_Y - 25.f}, {50.f, 50.f});

                            if (clickFPSMas.contains(mousePos) && config.fps < 240) { config.fps += 30; window.setFramerateLimit(config.fps); }
                            if (clickFPSMenos.contains(mousePos) && config.fps > 30) { config.fps -= 30; window.setFramerateLimit(config.fps); }
                            
                            if (clickResMas.contains(mousePos) || clickResMenos.contains(mousePos)) {
                                if (config.anchoPantalla == 1024) { config.anchoPantalla = 1280; config.altoPantalla = 720; }
                                else if (config.anchoPantalla == 1280) { config.anchoPantalla = 1600; config.altoPantalla = 900; }
                                else { config.anchoPantalla = 1024; config.altoPantalla = 576; }
                                
                                sf::State nuevoEstado = config.pantallaCompleta ? sf::State::Fullscreen : sf::State::Windowed;
                                window.create(sf::VideoMode({static_cast<unsigned int>(config.anchoPantalla), static_cast<unsigned int>(config.altoPantalla)}), "Dot Dot Boom", sf::Style::Default, nuevoEstado);
                                window.setFramerateLimit(config.fps);
                            }
                        }
                        // Pestaña Audio
                        else if (pestanaActual == 1) {
                            if (mousePos.x > AUDIO_BARRA_X && mousePos.x < (AUDIO_BARRA_X + 200.f)) {
                                int valor = static_cast<int>((mousePos.x - AUDIO_BARRA_X) / 20.f);
                                if (mousePos.y > (AUDIO_MASTER_Y - 15.f) && mousePos.y < (AUDIO_MASTER_Y + 15.f)) config.volMaster = std::clamp(valor, 0, 10);
                                if (mousePos.y > (AUDIO_MUSIC_Y - 15.f) && mousePos.y < (AUDIO_MUSIC_Y + 15.f)) config.volMusica = std::clamp(valor, 0, 10);
                                if (mousePos.y > (AUDIO_SFX_Y - 15.f) && mousePos.y < (AUDIO_SFX_Y + 15.f)) config.volEfectos = std::clamp(valor, 0, 10);
                            }
                            if (mousePos.y > (AUDIO_ESTEREO_Y - 15.f) && mousePos.y < (AUDIO_ESTEREO_Y + 15.f)) {
                                if (mousePos.x > AUDIO_BARRA_X && mousePos.x < (AUDIO_BARRA_X + 35.f)) config.estereo = true;
                                if (mousePos.x > (AUDIO_BARRA_X + 38.f) && mousePos.x < (AUDIO_BARRA_X + 73.f)) config.estereo = false;
                            }
                        }
                        // Pestaña Gameplay
                        else if (pestanaActual == 3) {
                            if (mousePos.y > 130.f && mousePos.y < 160.f) {
                                if (mousePos.x > 480.f && mousePos.x < 515.f) config.screenShake = true;
                                if (mousePos.x > 518.f && mousePos.x < 553.f) config.screenShake = false;
                            }
                            if (mousePos.x > 320.f && mousePos.x < 520.f && mousePos.y > 180.f && mousePos.y < 210.f) {
                                int valor = static_cast<int>((mousePos.x - 320.f) / 20.f);
                                config.intensidadShake = std::clamp(valor, 0, 10);
                            }
                        }
                    }
                }
            }
        }

        // ==========================================
        // RENDERIZADO VISUAL
        // ==========================================
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        window.clear(sf::Color(22, 18, 30));

        // ------------------------------------------
        // ESTADO 0: MENÚ PRINCIPAL
        // ------------------------------------------
        if (estadoActual == 0) {
            float tiempoActual = reloj.getElapsedTime().asSeconds();
            sf::Text titulo(fontTitulo, "DOT DOT BOOM", 64);
            titulo.setFillColor(sf::Color(250, 205, 90));
            titulo.setOutlineColor(sf::Color(50, 25, 10));
            titulo.setOutlineThickness(4.f);
            centrarTexto(titulo, 512.f, 120.f + std::sin(tiempoActual * 2.f) * 6.f);
            window.draw(titulo);

            sf::Text subtitulo(fontTitulo, "ROGUE-MINESWEEPER EXPERIMENTAL", 18);
            subtitulo.setFillColor(sf::Color(160, 175, 200));
            centrarTexto(subtitulo, 512.f, 168.f);
            window.draw(subtitulo);

            // Botón Jugar / Continuar
            bool hoverJ = btnJugar.contains(mousePos);
            dibujarBoton3D(window, btnJugar, hoverJ, sf::Color(50, 150, 80));
            sf::Text txtJugar(fontTitulo, partidaActiva ? "CONTINUAR" : "NUEVA PARTIDA", 24);
            centrarTexto(txtJugar, 512.f, btnJugar.position.y + 25.f);
            window.draw(txtJugar);

            // Botón Cargar
            bool hoverC = btnCargar.contains(mousePos);
            dibujarBoton3D(window, btnCargar, hoverC, sf::Color(50, 100, 160));
            sf::Text txtCargar(fontTitulo, "CARGAR PARTIDA", 22);
            centrarTexto(txtCargar, 512.f, btnCargar.position.y + 25.f);
            window.draw(txtCargar);

            // Botón Ranking
            bool hoverR = btnRanking.contains(mousePos);
            dibujarBoton3D(window, btnRanking, hoverR, sf::Color(150, 120, 40));
            sf::Text txtRanking(fontTitulo, "RANKING / RECORDS", 22);
            centrarTexto(txtRanking, 512.f, btnRanking.position.y + 25.f);
            window.draw(txtRanking);

            // Botón Opciones
            bool hoverO = btnOpciones.contains(mousePos);
            dibujarBoton3D(window, btnOpciones, hoverO, sf::Color(70, 75, 120));
            sf::Text txtOpciones(fontTitulo, "OPCIONES", 22);
            centrarTexto(txtOpciones, 512.f, btnOpciones.position.y + 25.f);
            window.draw(txtOpciones);

            // Botón Salir
            bool hoverS = btnSalir.contains(mousePos);
            dibujarBoton3D(window, btnSalir, hoverS, sf::Color(160, 50, 50));
            sf::Text txtSalir(fontTitulo, "SALIR", 22);
            centrarTexto(txtSalir, 512.f, btnSalir.position.y + 25.f);
            window.draw(txtSalir);

            if (temporizadorMensaje > 0.0f) {
                sf::Text notif(fontTitulo, mensajeEstado, 18);
                notif.setFillColor(colorMensaje);
                centrarTexto(notif, 512.f, 530.f);
                window.draw(notif);
            }
        }

        // ------------------------------------------
        // ESTADO 1: EN JUEGO (TABLERO Y HUD)
        // ------------------------------------------
        else if (estadoActual == 1) {
            // Fondo del panel superior HUD
            sf::RectangleShape panelHUD({1024.f, 55.f});
            panelHUD.setPosition({0.f, 0.f});
            panelHUD.setFillColor(sf::Color(15, 18, 28));
            panelHUD.setOutlineThickness(2.f);
            panelHUD.setOutlineColor(sf::Color(45, 55, 80));
            window.draw(panelHUD);

            // 1. Ronda
            sf::Text txtRonda(fontTitulo, "RONDA " + std::to_string(partida.getRonda()) + "/" + std::to_string(partida.getMaxRondas()), 22);
            txtRonda.setFillColor(sf::Color(255, 215, 0));
            alinearIzquierda(txtRonda, 25.f, 28.f);
            window.draw(txtRonda);

            // 2. Vidas
            std::string vidasStr = "VIDAS: ";
            for (int v = 0; v < partida.getVidas(); v++) vidasStr += "<3 ";
            sf::Text txtVidas(fontTitulo, vidasStr, 22);
            txtVidas.setFillColor(partida.getVidas() > 1 ? sf::Color(255, 80, 80) : sf::Color(255, 30, 30));
            alinearIzquierda(txtVidas, 230.f, 28.f);
            window.draw(txtVidas);

            // 3. Dinero
            sf::Text txtDinero(fontTitulo, "$" + formatearDinero(partida.getDinero()), 22);
            txtDinero.setFillColor(sf::Color(90, 240, 120));
            alinearIzquierda(txtDinero, 450.f, 28.f);
            window.draw(txtDinero);

            // 4. Zapatos
            sf::Text txtZapatos(fontTitulo, "ZAPATOS: " + std::to_string(partida.getZapatos()), 22);
            txtZapatos.setFillColor(sf::Color(230, 160, 50));
            alinearIzquierda(txtZapatos, 620.f, 28.f);
            window.draw(txtZapatos);

            // 5. Puntos
            sf::Text txtPts(fontTitulo, "PTS: " + std::to_string(partida.getPuntajeTotal()), 22);
            txtPts.setFillColor(sf::Color(240, 240, 240));
            alinearDerecha(txtPts, 995.f, 28.f);
            window.draw(txtPts);

            // Panel Izquierdo de Acciones
            sf::RectangleShape panelLateral({190.f, 505.f});
            panelLateral.setPosition({10.f, 65.f});
            panelLateral.setFillColor(sf::Color(18, 22, 34, 220));
            panelLateral.setOutlineThickness(1.5f);
            panelLateral.setOutlineColor(sf::Color(40, 50, 70));
            window.draw(panelLateral);

            // Botón Modo Zapato
            sf::FloatRect btnZapatoHUD({20.f, 75.f}, {170.f, 40.f});
            bool hoverZap = btnZapatoHUD.contains(mousePos);
            sf::Color colorBtnZap = modoZapato ? sf::Color(220, 140, 20) : (partida.getZapatos() > 0 ? sf::Color(60, 70, 90) : sf::Color(40, 45, 55));
            dibujarBoton3D(window, btnZapatoHUD, hoverZap, colorBtnZap);
            sf::Text txtBtnZap(fontTitulo, modoZapato ? "[Z] ZAPATO ACTIVO" : "[Z] USAR ZAPATO", 15);
            txtBtnZap.setFillColor(modoZapato ? sf::Color::Black : (partida.getZapatos() > 0 ? sf::Color::White : sf::Color(140, 140, 140)));
            centrarTexto(txtBtnZap, 105.f, 95.f);
            window.draw(txtBtnZap);

            // Botón Tienda
            sf::FloatRect btnTiendaHUD({20.f, 125.f}, {170.f, 40.f});
            bool tiendaLista = partida.puedeAbrirTienda();
            bool hoverT = btnTiendaHUD.contains(mousePos);
            sf::Color colorBtnTienda = tiendaLista ? sf::Color(40, 150, 70) : sf::Color(45, 50, 65);
            dibujarBoton3D(window, btnTiendaHUD, hoverT, colorBtnTienda);
            std::string strTienda = tiendaLista ? "[T] ABRIR TIENDA" : "TIENDA: " + std::to_string(partida.getContadorTienda()) + "/5";
            sf::Text txtBtnT(fontTitulo, strTienda, 16);
            txtBtnT.setFillColor(tiendaLista ? sf::Color::White : sf::Color(160, 160, 160));
            centrarTexto(txtBtnT, 105.f, 145.f);
            window.draw(txtBtnT);

            // Mini lista de chapas equipadas en el panel lateral
            sf::Text txtTChapas(fontTitulo, "-- CHAPAS (MAX 5) --", 14);
            txtTChapas.setFillColor(sf::Color(180, 190, 210));
            centrarTexto(txtTChapas, 105.f, 185.f);
            window.draw(txtTChapas);

            int hoverChapaIdx = -1;
            for (int i = 0; i < 5; i++) {
                sf::FloatRect slotRect({20.f, 205.f + i * 48.f}, {170.f, 42.f});
                bool hoverSlot = slotRect.contains(mousePos);
                if (hoverSlot) hoverChapaIdx = i;

                if (i < partida.getCantChapas()) {
                    Chapa* ch = partida.getChapaEquipada(i);
                    sf::Color cRareza = colorRareza(ch->getRareza());
                    dibujarBoton3D(window, slotRect, hoverSlot, sf::Color(30, 35, 50));
                    
                    sf::Text tNomChapa(fontTitulo, ch->getNombre(), 13);
                    tNomChapa.setFillColor(cRareza);
                    centrarTexto(tNomChapa, 105.f, 220.f + i * 48.f);
                    window.draw(tNomChapa);

                    sf::Text tRar(fontTitulo, ch->getRareza(), 10);
                    tRar.setFillColor(sf::Color(140, 150, 170));
                    centrarTexto(tRar, 105.f, 235.f + i * 48.f);
                    window.draw(tRar);
                } else {
                    sf::RectangleShape vacio({170.f, 42.f});
                    vacio.setPosition({20.f, 205.f + i * 48.f});
                    vacio.setFillColor(sf::Color(25, 28, 40, 150));
                    vacio.setOutlineThickness(1.f);
                    vacio.setOutlineColor(sf::Color(40, 45, 60));
                    window.draw(vacio);

                    sf::Text tVacio(fontTitulo, "[Vacio]", 12);
                    tVacio.setFillColor(sf::Color(80, 85, 100));
                    centrarTexto(tVacio, 105.f, 226.f + i * 48.f);
                    window.draw(tVacio);
                }
            }

            // Botones inferiores del panel lateral
            sf::FloatRect btnGuardarHUD({20.f, 470.f}, {170.f, 35.f});
            bool hoverG = btnGuardarHUD.contains(mousePos);
            dibujarBoton3D(window, btnGuardarHUD, hoverG, sf::Color(50, 80, 120));
            sf::Text txtBtnG(fontTitulo, "GUARDAR PARTIDA", 13);
            centrarTexto(txtBtnG, 105.f, 487.f);
            window.draw(txtBtnG);

            sf::FloatRect btnMenuHUD({20.f, 515.f}, {170.f, 35.f});
            bool hoverM = btnMenuHUD.contains(mousePos);
            dibujarBoton3D(window, btnMenuHUD, hoverM, sf::Color(120, 50, 60));
            sf::Text txtBtnM(fontTitulo, "[ESC] MENU PPAL", 13);
            centrarTexto(txtBtnM, 105.f, 532.f);
            window.draw(txtBtnM);

            // ==========================================
            // DIBUJAR EL TABLERO CENTRAL
            // ==========================================
            const Tablero& tablero = partida.getTablero();
            GeometriaTablero geo = calcularGeometriaTablero(tablero);
            float startX = geo.startX;
            float startY = geo.startY;

            // Efecto Screen Shake al pisar mina
            if (temporizadorShake > 0.0f) {
                startX += ((rand() % 100) / 50.f - 1.f) * config.intensidadShake;
                startY += ((rand() % 100) / 50.f - 1.f) * config.intensidadShake;
            }

            // Fondo marco del tablero
            sf::RectangleShape marcoTablero({geo.gridW + 16.f, geo.gridH + 16.f});
            marcoTablero.setPosition({startX - 8.f, startY - 8.f});
            marcoTablero.setFillColor(sf::Color(12, 14, 22));
            marcoTablero.setOutlineThickness(3.f);
            marcoTablero.setOutlineColor(modoZapato ? sf::Color(220, 160, 40) : sf::Color(50, 65, 95));
            window.draw(marcoTablero);

            int hoverFila = -1, hoverCol = -1;
            geo.obtenerCelda(mousePos.x, mousePos.y, hoverFila, hoverCol);

            for (int f = 0; f < geo.filas; f++) {
                for (int c = 0; c < geo.columnas; c++) {
                    const Celda& cel = tablero.getCelda(f, c);
                    sf::FloatRect cRect({startX + c * geo.cellSize, startY + f * geo.cellSize}, {geo.cellSize - geo.cellPad, geo.cellSize - geo.cellPad});
                    bool hoverC = (f == hoverFila && c == hoverCol);

                    if (!cel.estaRevelada()) {
                        if (cel.estaMarcada()) {
                            // Bandera
                            sf::RectangleShape bg({geo.cellSize - geo.cellPad, geo.cellSize - geo.cellPad});
                            bg.setPosition(cRect.position);
                            bg.setFillColor(hoverC ? sf::Color(200, 140, 30) : sf::Color(170, 110, 20));
                            bg.setOutlineThickness(1.f);
                            bg.setOutlineColor(sf::Color(255, 215, 0));
                            window.draw(bg);

                            sf::Text txtP(fontTitulo, "P", static_cast<unsigned int>(geo.cellSize * 0.65f));
                            txtP.setFillColor(sf::Color::White);
                            centrarTexto(txtP, cRect.position.x + cRect.size.x / 2.f, cRect.position.y + cRect.size.y / 2.f);
                            window.draw(txtP);
                        } else {
                            // Casilla Oculta 3D
                            sf::Color baseC = hoverC ? (modoZapato ? sf::Color(140, 110, 50) : sf::Color(75, 85, 110)) : sf::Color(45, 52, 70);
                            dibujarBoton3D(window, cRect, hoverC, baseC);
                        }
                    } else {
                        // Casilla Revelada
                        if (cel.esMina()) {
                            sf::RectangleShape bgMina({geo.cellSize - geo.cellPad, geo.cellSize - geo.cellPad});
                            bgMina.setPosition(cRect.position);
                            bgMina.setFillColor(sf::Color(200, 40, 40));
                            window.draw(bgMina);

                            sf::Text txtM(fontTitulo, "*", static_cast<unsigned int>(geo.cellSize * 0.9f));
                            txtM.setFillColor(sf::Color::White);
                            centrarTexto(txtM, cRect.position.x + cRect.size.x / 2.f, cRect.position.y + cRect.size.y / 2.f);
                            window.draw(txtM);
                        } else {
                            sf::RectangleShape bgSegura({geo.cellSize - geo.cellPad, geo.cellSize - geo.cellPad});
                            bgSegura.setPosition(cRect.position);
                            bgSegura.setFillColor(sf::Color(25, 30, 42));
                            bgSegura.setOutlineThickness(1.f);
                            bgSegura.setOutlineColor(sf::Color(35, 42, 58));
                            window.draw(bgSegura);

                            int mc = cel.getMinasCerca();
                            if (mc > 0) {
                                sf::Text txtNum(fontTitulo, std::to_string(mc), static_cast<unsigned int>(geo.cellSize * 0.65f));
                                txtNum.setFillColor(colorNumeroMinas(mc));
                                centrarTexto(txtNum, cRect.position.x + cRect.size.x / 2.f, cRect.position.y + cRect.size.y / 2.f);
                                window.draw(txtNum);
                            }
                        }
                    }
                }
            }

            // Barra inferior de notificaciones
            sf::Text txtNotif(fontTitulo, mensajeEstado, 18);
            txtNotif.setFillColor(colorMensaje);
            centrarTexto(txtNotif, 610.f, 540.f);
            window.draw(txtNotif);

            // Tooltip si el cursor pasa sobre una chapa en el HUD lateral
            if (hoverChapaIdx >= 0 && hoverChapaIdx < partida.getCantChapas()) {
                Chapa* ch = partida.getChapaEquipada(hoverChapaIdx);
                sf::RectangleShape tipBg({380.f, 100.f});
                tipBg.setPosition({220.f, 205.f + hoverChapaIdx * 48.f});
                tipBg.setFillColor(sf::Color(15, 20, 30, 245));
                tipBg.setOutlineThickness(2.f);
                tipBg.setOutlineColor(colorRareza(ch->getRareza()));
                window.draw(tipBg);

                sf::Text tipNom(fontTitulo, ch->getNombre() + " (" + ch->getRareza() + ")", 16);
                tipNom.setFillColor(colorRareza(ch->getRareza()));
                alinearIzquierda(tipNom, 235.f, tipBg.getPosition().y + 22.f);
                window.draw(tipNom);

                sf::Text tipDesc(fontTitulo, ch->getDescripcion(), 14);
                tipDesc.setFillColor(sf::Color(220, 225, 240));
                alinearIzquierda(tipDesc, 235.f, tipBg.getPosition().y + 50.f);
                window.draw(tipDesc);

                sf::Text tipVal(fontTitulo, "Valor de reventa: $" + formatearDinero(ch->getCosto() * 0.5f), 13);
                tipVal.setFillColor(sf::Color(120, 220, 140));
                alinearIzquierda(tipVal, 235.f, tipBg.getPosition().y + 78.f);
                window.draw(tipVal);
            }

            // Modal de Ronda Superada
            if (partida.rondaSuperada() && !partida.partidaGanada()) {
                sf::RectangleShape oscurecer({1024.f, 576.f});
                oscurecer.setFillColor(sf::Color(0, 0, 0, 160));
                window.draw(oscurecer);

                sf::RectangleShape modal({480.f, 250.f});
                modal.setPosition({272.f, 163.f});
                modal.setFillColor(sf::Color(20, 30, 45));
                modal.setOutlineThickness(3.f);
                modal.setOutlineColor(sf::Color(70, 180, 100));
                window.draw(modal);

                sf::Text tFeliz(fontTitulo, "!RONDA SUPERADA!", 32);
                tFeliz.setFillColor(sf::Color(100, 255, 130));
                centrarTexto(tFeliz, 512.f, 210.f);
                window.draw(tFeliz);

                sf::Text tBono(fontTitulo, "Bono por superar ronda: +" + std::to_string(partida.getRonda() * 100) + " PTS", 20);
                tBono.setFillColor(sf::Color::White);
                centrarTexto(tBono, 512.f, 260.f);
                window.draw(tBono);

                sf::FloatRect btnSigRonda({412.f, 320.f}, {200.f, 55.f});
                bool hoverSR = btnSigRonda.contains(mousePos);
                dibujarBoton3D(window, btnSigRonda, hoverSR, sf::Color(50, 160, 80));
                sf::Text txtSR(fontTitulo, "SIGUIENTE RONDA", 18);
                centrarTexto(txtSR, 512.f, 347.f);
                window.draw(txtSR);
            }
        }

        // ------------------------------------------
        // ESTADO 3: TIENDA TÁCTICA (COMPRA Y VENTA)
        // ------------------------------------------
        else if (estadoActual == 3) {
            sf::RectangleShape tiendaFondo({910.f, 520.f});
            tiendaFondo.setPosition({57.f, 28.f});
            tiendaFondo.setFillColor(sf::Color(18, 24, 38));
            tiendaFondo.setOutlineThickness(3.f);
            tiendaFondo.setOutlineColor(sf::Color(50, 150, 90));
            window.draw(tiendaFondo);

            sf::Text tTienda(fontTitulo, "TIENDA TACTICA DE SUMINISTROS", 28);
            tTienda.setFillColor(sf::Color(255, 215, 0));
            centrarTexto(tTienda, 512.f, 58.f);
            window.draw(tTienda);

            sf::Text tSaldo(fontTitulo, "Tus Monedas: $" + formatearDinero(partida.getDinero()), 20);
            tSaldo.setFillColor(sf::Color(90, 240, 120));
            centrarTexto(tSaldo, 512.f, 88.f);
            window.draw(tSaldo);

            // Cabeceras de Columna
            sf::Text tColVenta(fontTitulo, "-- ARTICULOS EN VENTA --", 16);
            tColVenta.setFillColor(sf::Color(200, 215, 240));
            centrarTexto(tColVenta, 340.f, 118.f);
            window.draw(tColVenta);

            sf::Text tColInv(fontTitulo, "-- TU INVENTARIO (" + std::to_string(partida.getCantChapas()) + "/5) --", 16);
            tColInv.setFillColor(sf::Color(200, 215, 240));
            centrarTexto(tColInv, 765.f, 118.f);
            window.draw(tColInv);

            // ====================================================
            // COLUMNA IZQUIERDA: ARTÍCULOS EN VENTA
            // ====================================================
            // Oferta de Zapato
            sf::RectangleShape boxZapato({500.f, 50.f});
            boxZapato.setPosition({80.f, 135.f});
            boxZapato.setFillColor(sf::Color(28, 35, 52));
            boxZapato.setOutlineThickness(1.5f);
            boxZapato.setOutlineColor(sf::Color(220, 160, 40));
            window.draw(boxZapato);

            sf::Text tZInfo(fontTitulo, "Zapato Tactico [Quedan: " + std::to_string(partida.getZapatosRestantesTienda()) + "/5]", 16);
            tZInfo.setFillColor(sf::Color(240, 240, 240));
            alinearIzquierda(tZInfo, 95.f, 160.f);
            window.draw(tZInfo);

            sf::Text tZCosto(fontTitulo, "$" + formatearDinero(static_cast<float>(partida.getCostoZapato())), 18);
            tZCosto.setFillColor(sf::Color(90, 240, 120));
            alinearDerecha(tZCosto, 455.f, 160.f);
            window.draw(tZCosto);

            sf::FloatRect btnCompZapato({465.f, 142.f}, {105.f, 36.f});
            bool puedeCompZ = partida.getDinero() >= partida.getCostoZapato() && partida.getZapatosRestantesTienda() > 0;
            dibujarBoton3D(window, btnCompZapato, btnCompZapato.contains(mousePos), puedeCompZ ? sf::Color(60, 140, 70) : sf::Color(50, 55, 65));
            sf::Text tBtnCZ(fontTitulo, "COMPRAR", 14);
            centrarTexto(tBtnCZ, 517.f, 160.f);
            window.draw(tBtnCZ);

            // Ofertas de Chapas (3 tarjetas)
            for (int i = 0; i < partida.getCantTienda(); i++) {
                Chapa* ch = partida.getChapaTienda(i);
                float cardY = 195.f + i * 85.f;

                sf::RectangleShape boxChapa({500.f, 75.f});
                boxChapa.setPosition({80.f, cardY});
                boxChapa.setFillColor(sf::Color(25, 32, 48));
                boxChapa.setOutlineThickness(1.5f);
                boxChapa.setOutlineColor(ch != nullptr ? colorRareza(ch->getRareza()) : sf::Color(60, 60, 70));
                window.draw(boxChapa);

                if (ch != nullptr) {
                    sf::Text tNom(fontTitulo, ch->getNombre() + " [" + ch->getRareza() + "]", 16);
                    tNom.setFillColor(colorRareza(ch->getRareza()));
                    alinearIzquierda(tNom, 95.f, cardY + 20.f);
                    window.draw(tNom);

                    sf::Text tDesc(fontTitulo, ch->getDescripcion(), 12);
                    tDesc.setFillColor(sf::Color(200, 210, 230));
                    alinearIzquierda(tDesc, 95.f, cardY + 50.f);
                    window.draw(tDesc);

                    sf::Text tPrecio(fontTitulo, "$" + formatearDinero(static_cast<float>(ch->getCosto())), 18);
                    tPrecio.setFillColor(sf::Color(90, 240, 120));
                    alinearDerecha(tPrecio, 455.f, cardY + 38.f);
                    window.draw(tPrecio);

                    sf::FloatRect btnCompChapa({465.f, cardY + 20.f}, {105.f, 36.f});
                    bool puedeComp = partida.getDinero() >= ch->getCosto() && !partida.getInventario().estaLleno();
                    dibujarBoton3D(window, btnCompChapa, btnCompChapa.contains(mousePos), puedeComp ? sf::Color(60, 140, 70) : sf::Color(50, 55, 65));
                    sf::Text tBtnC(fontTitulo, "COMPRAR", 14);
                    centrarTexto(tBtnC, 517.f, cardY + 38.f);
                    window.draw(tBtnC);
                } else {
                    sf::Text tAgotado(fontTitulo, "(Articulo Adquirido)", 16);
                    tAgotado.setFillColor(sf::Color(100, 110, 130));
                    centrarTexto(tAgotado, 330.f, cardY + 37.f);
                    window.draw(tAgotado);
                }
            }

            // ====================================================
            // COLUMNA DERECHA: TU INVENTARIO (VENTA)
            // ====================================================
            for (int i = 0; i < 5; i++) {
                float invY = 135.f + i * 62.f;
                sf::RectangleShape boxInv({325.f, 54.f});
                boxInv.setPosition({605.f, invY});

                if (i < partida.getCantChapas()) {
                    Chapa* ch = partida.getChapaEquipada(i);
                    boxInv.setFillColor(sf::Color(28, 35, 52));
                    boxInv.setOutlineThickness(1.5f);
                    boxInv.setOutlineColor(colorRareza(ch->getRareza()));
                    window.draw(boxInv);

                    sf::Text tNomInv(fontTitulo, ch->getNombre(), 14);
                    tNomInv.setFillColor(colorRareza(ch->getRareza()));
                    alinearIzquierda(tNomInv, 618.f, invY + 18.f);
                    window.draw(tNomInv);

                    sf::Text tRarInv(fontTitulo, ch->getRareza(), 11);
                    tRarInv.setFillColor(sf::Color(150, 160, 180));
                    alinearIzquierda(tRarInv, 618.f, invY + 36.f);
                    window.draw(tRarInv);

                    // Botón Vender (+50% del costo)
                    float reembolso = ch->getCosto() * 0.5f;
                    sf::FloatRect btnVenderChapa({815.f, invY + 11.f}, {105.f, 32.f});
                    bool hoverV = btnVenderChapa.contains(mousePos);
                    dibujarBoton3D(window, btnVenderChapa, hoverV, sf::Color(150, 60, 50));
                    sf::Text tBtnV(fontTitulo, "+$" + formatearDinero(reembolso), 13);
                    centrarTexto(tBtnV, 867.f, invY + 27.f);
                    window.draw(tBtnV);
                } else {
                    boxInv.setFillColor(sf::Color(22, 26, 38, 120));
                    boxInv.setOutlineThickness(1.f);
                    boxInv.setOutlineColor(sf::Color(45, 50, 65));
                    window.draw(boxInv);

                    sf::Text tVacio(fontTitulo, "[Ranura Vacia]", 13);
                    tVacio.setFillColor(sf::Color(90, 100, 120));
                    centrarTexto(tVacio, 767.f, invY + 27.f);
                    window.draw(tVacio);
                }
            }

            // Notificación inferior
            sf::Text tNotif(fontTitulo, mensajeEstado, 16);
            tNotif.setFillColor(colorMensaje);
            centrarTexto(tNotif, 512.f, 468.f);
            window.draw(tNotif);

            // Botón Salir de la Tienda
            sf::FloatRect btnSalirTienda({412.f, 498.f}, {200.f, 42.f});
            dibujarBoton3D(window, btnSalirTienda, btnSalirTienda.contains(mousePos), sf::Color(140, 50, 60));
            sf::Text tBtnST(fontTitulo, "VOLVER AL JUEGO", 17);
            centrarTexto(tBtnST, 512.f, 519.f);
            window.draw(tBtnST);
        }

        // ------------------------------------------
        // ESTADO 4: FIN DE JUEGO / VICTORIA
        // ------------------------------------------
        else if (estadoActual == 4) {
            bool victoria = partida.partidaGanada();

            sf::Text tFin(fontTitulo, victoria ? "!VICTORIA TOTAL!" : "!FIN DE LA PARTIDA!", 42);
            tFin.setFillColor(victoria ? sf::Color(255, 215, 0) : sf::Color(255, 60, 60));
            centrarTexto(tFin, 512.f, 100.f);
            window.draw(tFin);

            std::string sub = victoria ? "Has superado con exito las 10 rondas de combate." : "Has caido en combate en la Ronda " + std::to_string(partida.getRonda()) + ".";
            sf::Text tSub(fontTitulo, sub, 20);
            tSub.setFillColor(sf::Color(200, 210, 230));
            centrarTexto(tSub, 512.f, 155.f);
            window.draw(tSub);

            sf::Text tRes(fontTitulo, "Puntaje Final: " + std::to_string(partida.getPuntajeTotal()) + " PTS   |   Monedas: $" + formatearDinero(partida.getDinero()), 22);
            tRes.setFillColor(sf::Color(100, 240, 140));
            centrarTexto(tRes, 512.f, 210.f);
            window.draw(tRes);

            // Caja de Entrada de Nombre para el Ranking
            sf::Text tPide(fontTitulo, "Ingresa tu nombre para el Registro de Honor:", 18);
            tPide.setFillColor(sf::Color(220, 220, 220));
            centrarTexto(tPide, 512.f, 290.f);
            window.draw(tPide);

            sf::RectangleShape boxNombre({200.f, 45.f});
            boxNombre.setPosition({320.f, 340.f});
            boxNombre.setFillColor(sf::Color(30, 35, 55));
            boxNombre.setOutlineThickness(2.f);
            boxNombre.setOutlineColor(puntajeGuardado ? sf::Color(80, 160, 90) : sf::Color(255, 215, 0));
            window.draw(boxNombre);

            sf::Text txtNomIn(fontTitulo, nombreJugadorInput + (puntajeGuardado ? "" : "_"), 20);
            txtNomIn.setFillColor(sf::Color::White);
            centrarTexto(txtNomIn, 420.f, 362.f);
            window.draw(txtNomIn);

            sf::FloatRect btnGuardarRec({540.f, 340.f}, {160.f, 45.f});
            dibujarBoton3D(window, btnGuardarRec, btnGuardarRec.contains(mousePos), puntajeGuardado ? sf::Color(50, 60, 70) : sf::Color(50, 140, 80));
            sf::Text txtBtnGR(fontTitulo, puntajeGuardado ? "GUARDADO" : "GUARDAR", 18);
            centrarTexto(txtBtnGR, 620.f, 362.f);
            window.draw(txtBtnGR);

            // Botones Reintentar y Menú
            sf::FloatRect btnReintentar({310.f, 430.f}, {180.f, 50.f});
            dibujarBoton3D(window, btnReintentar, btnReintentar.contains(mousePos), sf::Color(50, 120, 180));
            sf::Text txtReint(fontTitulo, "JUGAR DE NUEVO", 18);
            centrarTexto(txtReint, 400.f, 455.f);
            window.draw(txtReint);

            sf::FloatRect btnIrMenu({530.f, 430.f}, {180.f, 50.f});
            dibujarBoton3D(window, btnIrMenu, btnIrMenu.contains(mousePos), sf::Color(140, 50, 60));
            sf::Text txtMenu(fontTitulo, "MENU PRINCIPAL", 18);
            centrarTexto(txtMenu, 620.f, 455.f);
            window.draw(txtMenu);
        }

        // ------------------------------------------
        // ESTADO 5: RANKING
        // ------------------------------------------
        else if (estadoActual == 5) {
            sf::RectangleShape rankFondo({740.f, 460.f});
            rankFondo.setPosition({142.f, 40.f});
            rankFondo.setFillColor(sf::Color(16, 22, 35));
            rankFondo.setOutlineThickness(3.f);
            rankFondo.setOutlineColor(sf::Color(220, 180, 40));
            window.draw(rankFondo);

            sf::Text tTitRank(fontTitulo, "SALON DE LA FAMA - TOP REGISTROS", 28);
            tTitRank.setFillColor(sf::Color(255, 215, 0));
            centrarTexto(tTitRank, 512.f, 75.f);
            window.draw(tTitRank);

            RegistroPuntaje lista[10];
            int cantReg = partida.cargarPuntajes("scores.txt", lista, 10);

            if (cantReg == 0) {
                sf::Text tVacio(fontTitulo, "Aun no hay registros en scores.txt.\nCompleta una partida para ingresar al salon.", 20);
                tVacio.setFillColor(sf::Color(150, 160, 180));
                centrarTexto(tVacio, 512.f, 250.f);
                window.draw(tVacio);
            } else {
                for (int i = 0; i < cantReg; i++) {
                    float yR = 120.f + i * 34.f;
                    sf::Text tPuesto(fontTitulo, "#" + std::to_string(i + 1), 18);
                    tPuesto.setFillColor(i == 0 ? sf::Color(255, 215, 0) : (i == 1 ? sf::Color(200, 200, 210) : sf::Color(170, 130, 80)));
                    alinearIzquierda(tPuesto, 180.f, yR);
                    window.draw(tPuesto);

                    sf::Text tNom(fontTitulo, lista[i].nombre, 18);
                    tNom.setFillColor(sf::Color::White);
                    alinearIzquierda(tNom, 270.f, yR);
                    window.draw(tNom);

                    sf::Text tRonda(fontTitulo, "Ronda " + std::to_string(lista[i].ronda), 18);
                    tRonda.setFillColor(sf::Color(140, 200, 255));
                    alinearIzquierda(tRonda, 540.f, yR);
                    window.draw(tRonda);

                    sf::Text tPunt(fontTitulo, std::to_string(lista[i].puntaje) + " pts", 18);
                    tPunt.setFillColor(sf::Color(100, 255, 120));
                    alinearDerecha(tPunt, 820.f, yR);
                    window.draw(tPunt);
                }
            }

            sf::FloatRect btnVolverRank({412.f, 490.f}, {200.f, 45.f});
            dibujarBoton3D(window, btnVolverRank, btnVolverRank.contains(mousePos), sf::Color(140, 50, 60));
            sf::Text tVR(fontTitulo, "VOLVER", 20);
            centrarTexto(tVR, 512.f, 512.f);
            window.draw(tVR);
        }

        // ------------------------------------------
        // ESTADO 2: OPCIONES
        // ------------------------------------------
        else if (estadoActual == 2) {
            sf::RectangleShape panelFondo({730.f, 460.f});
            panelFondo.setPosition({220.f, 50.f});
            panelFondo.setFillColor(sf::Color(15, 25, 40));
            panelFondo.setOutlineColor(sf::Color(60, 150, 200));
            panelFondo.setOutlineThickness(4.f);
            window.draw(panelFondo);

            // Pestañas laterales
            sf::Text txtTabGraficos(fontTitulo, "GRAFICOS", 20);
            sf::Text txtTabAudio(fontTitulo, "AUDIO", 20);
            sf::Text txtTabControles(fontTitulo, "CONTROLES", 20);
            sf::Text txtTabGameplay(fontTitulo, "GAMEPLAY", 20);

            dibujarBoton3D(window, tabGraficos, pestanaActual == 0, sf::Color(40, 60, 90)); centrarTexto(txtTabGraficos, pestanaX + 80.f, pestanaY + 40.f); window.draw(txtTabGraficos);
            dibujarBoton3D(window, tabAudio, pestanaActual == 1, sf::Color(40, 60, 90)); centrarTexto(txtTabAudio, pestanaX + 80.f, pestanaY + 125.f); window.draw(txtTabAudio);
            dibujarBoton3D(window, tabControles, pestanaActual == 2, sf::Color(40, 60, 90)); centrarTexto(txtTabControles, pestanaX + 80.f, pestanaY + 210.f); window.draw(txtTabControles);
            dibujarBoton3D(window, tabGameplay, pestanaActual == 3, sf::Color(40, 60, 90)); centrarTexto(txtTabGameplay, pestanaX + 80.f, pestanaY + 295.f); window.draw(txtTabGameplay);

            sf::Text txtTituloOpciones(fontTitulo, "", 36);
            txtTituloOpciones.setFillColor(sf::Color::White);

            if (pestanaActual == 0) {
                txtTituloOpciones.setString("AJUSTES GRAFICOS"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);
                
                sf::Text txtFPSFijo(fontTitulo, "MAX FPS: ", 25); txtFPSFijo.setFillColor(sf::Color(200, 200, 200));
                sf::Text txtFPSNumero(fontTitulo, std::to_string(config.fps), 25); txtFPSNumero.setFillColor(sf::Color(255, 255, 150));
                alinearIzquierda(txtFPSFijo, POS_TEXTO_FPS_X, FILA_FPS_Y); window.draw(txtFPSFijo);
                alinearIzquierda(txtFPSNumero, POS_NUMERO_FPS_X, FILA_FPS_Y); window.draw(txtFPSNumero);

                sf::FloatRect r1({POS_BOTON_FPS_MENOS_X, FILA_FPS_Y - 25.f}, {50.f, 50.f});
                sf::FloatRect r2({POS_BOTON_FPS_MAS_X, FILA_FPS_Y - 25.f}, {50.f, 50.f});
                dibujarBoton3D(window, r1, r1.contains(mousePos), sf::Color(80, 80, 80));
                sf::Text tMenos(fontTitulo, "-", 25); centrarTexto(tMenos, POS_BOTON_FPS_MENOS_X + 25.f, FILA_FPS_Y); window.draw(tMenos);
                dibujarBoton3D(window, r2, r2.contains(mousePos), sf::Color(80, 80, 80));
                sf::Text tMas(fontTitulo, "+", 25); centrarTexto(tMas, POS_BOTON_FPS_MAS_X + 25.f, FILA_FPS_Y); window.draw(tMas);

                sf::Text txtResFijo(fontTitulo, "RESOLUCION: ", 25); txtResFijo.setFillColor(sf::Color(200, 200, 200));
                sf::Text txtResNumero(fontTitulo, std::to_string(config.anchoPantalla) + "x" + std::to_string(config.altoPantalla), 25); txtResNumero.setFillColor(sf::Color(255, 255, 150));
                alinearIzquierda(txtResFijo, POS_TEXTO_RES_X, FILA_RES_Y); window.draw(txtResFijo);
                alinearIzquierda(txtResNumero, POS_NUMERO_RES_X, FILA_RES_Y); window.draw(txtResNumero);

                sf::FloatRect r3({POS_BOTON_RES_MENOS_X, FILA_RES_Y - 25.f}, {50.f, 50.f});
                sf::FloatRect r4({POS_BOTON_RES_MAS_X, FILA_RES_Y - 25.f}, {50.f, 50.f});
                dibujarBoton3D(window, r3, r3.contains(mousePos), sf::Color(80, 80, 80));
                sf::Text tResMenos(fontTitulo, "<", 25); centrarTexto(tResMenos, POS_BOTON_RES_MENOS_X + 25.f, FILA_RES_Y); window.draw(tResMenos);
                dibujarBoton3D(window, r4, r4.contains(mousePos), sf::Color(80, 80, 80));
                sf::Text tResMas(fontTitulo, ">", 25); centrarTexto(tResMas, POS_BOTON_RES_MAS_X + 25.f, FILA_RES_Y); window.draw(tResMas);
            }
            else if (pestanaActual == 1) {
                txtTituloOpciones.setString("AJUSTES DE AUDIO"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);

                sf::Text tAudioMaster(fontTitulo, "Volumen General", 22); tAudioMaster.setFillColor(sf::Color::White);
                alinearIzquierda(tAudioMaster, AUDIO_TEXTO_X, AUDIO_MASTER_Y); window.draw(tAudioMaster);
                dibujarBarraVolumen(window, AUDIO_BARRA_X, AUDIO_MASTER_Y - 10.f, config.volMaster, 10);
                sf::Text v1(fontTitulo, std::to_string(config.volMaster) + "/10", 22); alinearIzquierda(v1, AUDIO_NUMERO_X, AUDIO_MASTER_Y); window.draw(v1);

                sf::Text tAudioMusic(fontTitulo, "Volumen Musica", 22); tAudioMusic.setFillColor(sf::Color::White);
                alinearIzquierda(tAudioMusic, AUDIO_TEXTO_X, AUDIO_MUSIC_Y); window.draw(tAudioMusic);
                dibujarBarraVolumen(window, AUDIO_BARRA_X, AUDIO_MUSIC_Y - 10.f, config.volMusica, 10);
                sf::Text v2(fontTitulo, std::to_string(config.volMusica) + "/10", 22); alinearIzquierda(v2, AUDIO_NUMERO_X, AUDIO_MUSIC_Y); window.draw(v2);

                sf::Text tAudioSFX(fontTitulo, "Efectos Sonoros", 22); tAudioSFX.setFillColor(sf::Color::White);
                alinearIzquierda(tAudioSFX, AUDIO_TEXTO_X, AUDIO_SFX_Y); window.draw(tAudioSFX);
                dibujarBarraVolumen(window, AUDIO_BARRA_X, AUDIO_SFX_Y - 10.f, config.volEfectos, 10);
                sf::Text v3(fontTitulo, std::to_string(config.volEfectos) + "/10", 22); alinearIzquierda(v3, AUDIO_NUMERO_X, AUDIO_SFX_Y); window.draw(v3);

                sf::Text tAudioStereo(fontTitulo, "Salida Estereo", 22); tAudioStereo.setFillColor(sf::Color::White);
                alinearIzquierda(tAudioStereo, AUDIO_TEXTO_X, AUDIO_ESTEREO_Y); window.draw(tAudioStereo);
                dibujarBotonONOFF(window, AUDIO_BARRA_X, AUDIO_ESTEREO_Y - 11.f, config.estereo);
            }
            else if (pestanaActual == 2) {
                txtTituloOpciones.setString("AJUSTES DE CONTROLES"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);
                
                sf::Text c1(fontTitulo, "Clic Izquierdo : Revelar Casilla", 20); c1.setFillColor(sf::Color::White); alinearIzquierda(c1, 300.f, 160.f); window.draw(c1);
                sf::Text c2(fontTitulo, "Clic Derecho   : Alternar Bandera (P)", 20); c2.setFillColor(sf::Color::White); alinearIzquierda(c2, 300.f, 210.f); window.draw(c2);
                sf::Text c3(fontTitulo, "Tecla [Z]       : Activar/Usar Zapato", 20); c3.setFillColor(sf::Color::White); alinearIzquierda(c3, 300.f, 260.f); window.draw(c3);
                sf::Text c4(fontTitulo, "Tecla [T]       : Abrir Tienda Tactica", 20); c4.setFillColor(sf::Color::White); alinearIzquierda(c4, 300.f, 310.f); window.draw(c4);
                sf::Text c5(fontTitulo, "Tecla [ESC]     : Menu / Pausa", 20); c5.setFillColor(sf::Color::White); alinearIzquierda(c5, 300.f, 360.f); window.draw(c5);
            }
            else if (pestanaActual == 3) {
                txtTituloOpciones.setString("AJUSTES DE GAMEPLAY"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);

                sf::Text tGameplayShake(fontTitulo, "Screen Shake", 22); tGameplayShake.setFillColor(sf::Color::White);
                alinearIzquierda(tGameplayShake, 260.f, 150.f); window.draw(tGameplayShake);
                dibujarBotonONOFF(window, 480.f, 140.f, config.screenShake);

                sf::Text tGameplayIntensidad(fontTitulo, "Intensidad Shake", 22); tGameplayIntensidad.setFillColor(sf::Color::White);
                alinearIzquierda(tGameplayIntensidad, 260.f, 205.f); window.draw(tGameplayIntensidad);
                dibujarBarraVolumen(window, 320.f, 195.f, config.intensidadShake, 10);
                sf::Text vs(fontTitulo, std::to_string(config.intensidadShake) + "/10", 22); alinearIzquierda(vs, 540.f, 205.f); window.draw(vs);

                sf::Text tGameplayIdioma(fontTitulo, "Idioma", 22); tGameplayIdioma.setFillColor(sf::Color::White);
                alinearIzquierda(tGameplayIdioma, 260.f, 265.f); window.draw(tGameplayIdioma);
                sf::RectangleShape d1({150.f, 30.f}); d1.setPosition({400.f, 250.f}); d1.setFillColor(sf::Color(50, 80, 120)); d1.setOutlineThickness(1.5f); d1.setOutlineColor(sf::Color(20, 30, 40)); window.draw(d1);
                sf::Text t1(fontTitulo, config.idioma, 18); centrarTexto(t1, 475.f, 265.f); window.draw(t1);

                sf::Text tGameplayDificultad(fontTitulo, "Dificultad", 22); tGameplayDificultad.setFillColor(sf::Color::White);
                alinearIzquierda(tGameplayDificultad, 260.f, 325.f); window.draw(tGameplayDificultad);
                sf::RectangleShape d2({150.f, 30.f}); d2.setPosition({400.f, 310.f}); d2.setFillColor(sf::Color(50, 80, 120)); d2.setOutlineThickness(1.5f); d2.setOutlineColor(sf::Color(20, 30, 40)); window.draw(d2);
                sf::Text t2(fontTitulo, config.dificultad, 18); centrarTexto(t2, 475.f, 325.f); window.draw(t2);
            }

            // Botón Volver
            dibujarBoton3D(window, btnVolver, btnVolver.contains(mousePos), sf::Color(160, 50, 50));
            sf::Text txtVolver(fontTitulo, "VOLVER", 22);
            centrarTexto(txtVolver, 512.f, btnVolver.position.y + 25.f);
            window.draw(txtVolver);
        }

        window.display();
    }
    return 0;
}
