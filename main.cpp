#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> // Para std::clamp

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

// --- FUNCIÓN PARA DIBUJAR BOTONES 3D ---
void dibujarBoton3D(sf::RenderWindow& window, sf::FloatRect rect, bool hover, sf::Color colorBase) {
    float x = rect.position.x; float y = rect.position.y;
    float w = rect.size.x; float h = rect.size.y;

    float sombraOffset = hover ? 12.f : 8.f;
    sf::RectangleShape sombra({w, h});
    sombra.setPosition({x + sombraOffset, y + sombraOffset});
    sombra.setFillColor(sf::Color(10, 10, 15, 150));
    window.draw(sombra);

    sf::RectangleShape cuerpo({w, h});
    cuerpo.setPosition({x, y});
    cuerpo.setFillColor(colorBase);
    window.draw(cuerpo);

    float grosor = 4.f;
    sf::RectangleShape luzSup({w, grosor}); luzSup.setPosition({x, y}); luzSup.setFillColor(sf::Color(255, 255, 255, 80)); window.draw(luzSup);
    sf::RectangleShape luzIzq({grosor, h}); luzIzq.setPosition({x, y}); luzIzq.setFillColor(sf::Color(255, 255, 255, 80)); window.draw(luzIzq);
    sf::RectangleShape sombraInf({w, grosor}); sombraInf.setPosition({x, y + h - grosor}); sombraInf.setFillColor(sf::Color(0, 0, 0, 120)); window.draw(sombraInf);
    sf::RectangleShape sombraDer({grosor, h}); sombraDer.setPosition({x + w - grosor, y}); sombraDer.setFillColor(sf::Color(0, 0, 0, 120)); window.draw(sombraDer);

    sf::RectangleShape borde({w, h});
    borde.setPosition({x, y});
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineThickness(2.f);
    borde.setOutlineColor(sf::Color(20, 20, 30));
    window.draw(borde);
}

// --- FUNCIÓN PARA DIBUJAR UNA BARRA DE VOLUMEN ---
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
    relleno.setFillColor(sf::Color(230, 140, 40)); // Naranja
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

// --- FUNCIÓN PARA DIBUJAR LOS BOTONES ON/OFF ---
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

// --- FUNCIONES DE TEXTO ---
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
    if (!fontTitulo.openFromFile("C:/Windows/Fonts/consola.ttf")) return -1;

    int estadoActual = 0; 
    int pestanaActual = 0; // 0: Graficos, 1: Audio, 2: Controles, 3: Gameplay
    sf::Clock reloj;

    // ==========================================================
    // ⚙️ PANEL DE CONTROL (Edita estos números)
    // ==========================================================
    
    // --- FILA DE FPS (Gráficos) ---
    float POS_TEXTO_FPS_X = 300.f;       // (200 + 50)
    float POS_NUMERO_FPS_X = 520.f;      // (425 + 50) -> Recuerda mover también el número
    float POS_BOTON_FPS_MENOS_X = 450.f; 
    float POS_BOTON_FPS_MAS_X = 570.f;   
    float FILA_FPS_Y = 250.f;        

    // --- FILA DE RESOLUCIÓN (Gráficos) ---
    float POS_TEXTO_RES_X = 215.f;     
    float POS_NUMERO_RES_X = 534.f;    
    float POS_BOTON_RES_MENOS_X = 465.f;   
    float POS_BOTON_RES_MAS_X = 685.f;     
    float FILA_RES_Y = 365.f;          

    // --- PANEL DE AUDIO ---
    // Eje X (Mover a la derecha/izquierda)
    float AUDIO_TEXTO_X = 260.f;       // Textos "Master Volume", etc.
    float AUDIO_BARRA_X = 520.f;       // Inicio de la barra naranja
    float AUDIO_NUMERO_X = 540.f;      // Números "8/10", "6/10", etc.

    // Eje Y (Mover arriba/abajo)
    float AUDIO_MASTER_Y = 150.f;      // Altura del volumen Master
    float AUDIO_MUSIC_Y = 210.f;       // Altura del volumen Música
    float AUDIO_SFX_Y = 270.f;         // Altura del volumen Efectos
    float AUDIO_ESTEREO_Y = 330.f;     // Altura del botón Estéreo
    // ==========================================================

    // --- TEXTOS DEL MENÚ PRINCIPAL ---
    sf::Text titulo(fontTitulo, "DOT DOT BOOM", 70);
    titulo.setFillColor(sf::Color(240, 200, 100));
    titulo.setOutlineColor(sf::Color(60, 30, 10));
    titulo.setOutlineThickness(5);
    centrarTexto(titulo, 512.f, 150.f);

    sf::Text txtJugar(fontTitulo, "JUGAR", 30); sf::Text txtOpciones(fontTitulo, "OPCIONES", 30); sf::Text txtSalir(fontTitulo, "SALIR", 30);
    sf::FloatRect btnJugar({412.f, 250.f}, {200.f, 60.f}); sf::FloatRect btnOpciones({412.f, 340.f}, {200.f, 60.f}); sf::FloatRect btnSalir({412.f, 430.f}, {200.f, 60.f});

    // --- TEXTOS DE OPCIONES ---
    sf::Text txtTituloOpciones(fontTitulo, "", 40); txtTituloOpciones.setFillColor(sf::Color::White); centrarTexto(txtTituloOpciones, 585.f, 85.f);

    sf::Text txtFPSFijo(fontTitulo, "MAX FPS: ", 30); txtFPSFijo.setFillColor(sf::Color(200, 200, 200));
    sf::Text txtFPSNumero(fontTitulo, "", 30); txtFPSNumero.setFillColor(sf::Color(255, 255, 150)); 
    sf::Text txtResFijo(fontTitulo, "RESOLUCION: ", 30); txtResFijo.setFillColor(sf::Color(200, 200, 200));
    sf::Text txtResNumero(fontTitulo, "", 30); txtResNumero.setFillColor(sf::Color(255, 255, 150));
    sf::Text tMenos(fontTitulo, "-", 30); sf::Text tMas(fontTitulo, "+", 30);
    sf::Text tResMenos(fontTitulo, "<", 30); sf::Text tResMas(fontTitulo, ">", 30);

    sf::FloatRect btnVolver({412.f, 480.f}, {200.f, 60.f}); sf::Text txtVolver(fontTitulo, "VOLVER", 30);

    // --- BOTONES DE PESTAÑAS ---
    float pestanaX = 50.f; float pestanaY = 150.f; float pestanaAlto = 80.f; float pestanaAncho = 160.f;
    sf::FloatRect tabGraficos({pestanaX, pestanaY}, {pestanaAncho, pestanaAlto});
    sf::FloatRect tabAudio({pestanaX, pestanaY + 85.f}, {pestanaAncho, pestanaAlto});
    sf::FloatRect tabControles({pestanaX, pestanaY + 170.f}, {pestanaAncho, pestanaAlto});
    sf::FloatRect tabGameplay({pestanaX, pestanaY + 255.f}, {pestanaAncho, pestanaAlto});

    sf::Text txtTabGraficos(fontTitulo, "GRAFICOS", 20); sf::Text txtTabAudio(fontTitulo, "AUDIO", 20);
    sf::Text txtTabControles(fontTitulo, "CONTROLES", 20); sf::Text txtTabGameplay(fontTitulo, "GAMEPLAY", 20);

    // --- TEXTOS DE LA PESTAÑA AUDIO ---
    sf::Text tAudioMaster(fontTitulo, "Master Volume", 25); tAudioMaster.setFillColor(sf::Color::White);
    sf::Text tAudioMusic(fontTitulo, "Music Volume", 25); tAudioMusic.setFillColor(sf::Color::White);
    sf::Text tAudioSFX(fontTitulo, "Sound Effects Volume", 25); tAudioSFX.setFillColor(sf::Color::White);
    sf::Text tAudioStereo(fontTitulo, "Stereo Output", 25); tAudioStereo.setFillColor(sf::Color::White);

    // --- TEXTOS DE LA PESTAÑA GAMEPLAY ---
    sf::Text tGameplayShake(fontTitulo, "Screen Shake", 25); tGameplayShake.setFillColor(sf::Color::White);
    sf::Text tGameplayIntensidad(fontTitulo, "Screen Shake Intensity", 25); tGameplayIntensidad.setFillColor(sf::Color::White);
    sf::Text tGameplayIdioma(fontTitulo, "Language", 25); tGameplayIdioma.setFillColor(sf::Color::White);
    sf::Text tGameplayDificultad(fontTitulo, "Game Difficulty", 25); tGameplayDificultad.setFillColor(sf::Color::White);

    // --- BUCLE PRINCIPAL ---
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(mouseButton->position);

                    if (estadoActual == 0) {
                        if (btnJugar.contains(mousePos)) estadoActual = 1;
                        if (btnOpciones.contains(mousePos)) estadoActual = 2;
                        if (btnSalir.contains(mousePos)) window.close();
                    }
                    else if (estadoActual == 2) {
                        if (tabGraficos.contains(mousePos)) pestanaActual = 0;
                        if (tabAudio.contains(mousePos)) pestanaActual = 1;
                        if (tabControles.contains(mousePos)) pestanaActual = 2;
                        if (tabGameplay.contains(mousePos)) pestanaActual = 3;
                        if (btnVolver.contains(mousePos)) estadoActual = 0;

                        // --- LÓGICA GRÁFICOS ---
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
                        
                        // --- LÓGICA AUDIO ---
                        if (pestanaActual == 1) {
                            // Calculamos la zona de clic basándonos en el Panel de Control de Audio
                            if (mousePos.x > AUDIO_BARRA_X && mousePos.x < (AUDIO_BARRA_X + 200.f)) {
                                int valor = static_cast<int>((mousePos.x - AUDIO_BARRA_X) / 20.f);
                                
                                // Master
                                if (mousePos.y > (AUDIO_MASTER_Y - 15.f) && mousePos.y < (AUDIO_MASTER_Y + 15.f)) 
                                    config.volMaster = std::clamp(valor, 0, 10);
                                
                                // Música
                                if (mousePos.y > (AUDIO_MUSIC_Y - 15.f) && mousePos.y < (AUDIO_MUSIC_Y + 15.f)) 
                                    config.volMusica = std::clamp(valor, 0, 10);
                                
                                // Efectos
                                if (mousePos.y > (AUDIO_SFX_Y - 15.f) && mousePos.y < (AUDIO_SFX_Y + 15.f)) 
                                    config.volEfectos = std::clamp(valor, 0, 10);
                            }
                            // Botón Estéreo
                            if (mousePos.y > (AUDIO_ESTEREO_Y - 15.f) && mousePos.y < (AUDIO_ESTEREO_Y + 15.f)) {
                                if (mousePos.x > AUDIO_BARRA_X && mousePos.x < (AUDIO_BARRA_X + 35.f)) config.estereo = true;
                                if (mousePos.x > (AUDIO_BARRA_X + 38.f) && mousePos.x < (AUDIO_BARRA_X + 73.f)) config.estereo = false;
                            }
                        }

                        // --- LÓGICA GAMEPLAY ---
                        if (pestanaActual == 3) {
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

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        window.clear(sf::Color(25, 20, 35));

        if (estadoActual == 0) {
            // --- MENÚ PRINCIPAL ---
            float tiempoActual = reloj.getElapsedTime().asSeconds();
            titulo.setPosition({512.f, 150.f + std::sin(tiempoActual * 2.f) * 5.f}); window.draw(titulo);

            bool hoverJ = btnJugar.contains(mousePos); bool hoverO = btnOpciones.contains(mousePos); bool hoverS = btnSalir.contains(mousePos);
            dibujarBoton3D(window, btnJugar, hoverJ, sf::Color(60, 150, 80)); centrarTexto(txtJugar, 512.f, btnJugar.position.y + 30.f + (hoverJ ? 2.f : 0.f)); window.draw(txtJugar);
            dibujarBoton3D(window, btnOpciones, hoverO, sf::Color(60, 100, 180)); centrarTexto(txtOpciones, 512.f, btnOpciones.position.y + 30.f + (hoverO ? 2.f : 0.f)); window.draw(txtOpciones);
            dibujarBoton3D(window, btnSalir, hoverS, sf::Color(180, 60, 60)); centrarTexto(txtSalir, 512.f, btnSalir.position.y + 30.f + (hoverS ? 2.f : 0.f)); window.draw(txtSalir);
        }
        else if (estadoActual == 2) {
            // --- PANEL DE FONDO ---
            sf::RectangleShape panelFondo({730.f, 460.f}); panelFondo.setPosition({220.f, 50.f}); panelFondo.setFillColor(sf::Color(15, 25, 40)); panelFondo.setOutlineColor(sf::Color(60, 150, 200)); panelFondo.setOutlineThickness(4.f); window.draw(panelFondo);

            // Pestañas laterales
            dibujarBoton3D(window, tabGraficos, pestanaActual == 0, sf::Color(40, 60, 90)); centrarTexto(txtTabGraficos, pestanaX + 80.f, pestanaY + 40.f); window.draw(txtTabGraficos);
            dibujarBoton3D(window, tabAudio, pestanaActual == 1, sf::Color(40, 60, 90)); centrarTexto(txtTabAudio, pestanaX + 80.f, pestanaY + 125.f); window.draw(txtTabAudio);
            dibujarBoton3D(window, tabControles, pestanaActual == 2, sf::Color(40, 60, 90)); centrarTexto(txtTabControles, pestanaX + 80.f, pestanaY + 210.f); window.draw(txtTabControles);
            dibujarBoton3D(window, tabGameplay, pestanaActual == 3, sf::Color(40, 60, 90)); centrarTexto(txtTabGameplay, pestanaX + 80.f, pestanaY + 295.f); window.draw(txtTabGameplay);

            // --- CONTENIDO SEGÚN PESTAÑA ---
            if (pestanaActual == 0) {
                // GRÁFICOS
                txtTituloOpciones.setString("GRAPHICS SETTINGS"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);
                alinearIzquierda(txtFPSFijo, POS_TEXTO_FPS_X, FILA_FPS_Y); window.draw(txtFPSFijo); txtFPSNumero.setString(std::to_string(config.fps)); alinearIzquierda(txtFPSNumero, POS_NUMERO_FPS_X, FILA_FPS_Y); window.draw(txtFPSNumero);
                sf::FloatRect r1({POS_BOTON_FPS_MENOS_X, FILA_FPS_Y - 25.f}, {50.f, 50.f}); dibujarBoton3D(window, r1, r1.contains(mousePos), sf::Color(80, 80, 80)); centrarTexto(tMenos, POS_BOTON_FPS_MENOS_X + 25.f, FILA_FPS_Y); window.draw(tMenos);
                sf::FloatRect r2({POS_BOTON_FPS_MAS_X, FILA_FPS_Y - 25.f}, {50.f, 50.f}); dibujarBoton3D(window, r2, r2.contains(mousePos), sf::Color(80, 80, 80)); centrarTexto(tMas, POS_BOTON_FPS_MAS_X + 25.f, FILA_FPS_Y); window.draw(tMas);
                alinearIzquierda(txtResFijo, POS_TEXTO_RES_X, FILA_RES_Y); window.draw(txtResFijo); txtResNumero.setString(std::to_string(config.anchoPantalla) + "x" + std::to_string(config.altoPantalla)); alinearIzquierda(txtResNumero, POS_NUMERO_RES_X, FILA_RES_Y); window.draw(txtResNumero);
                sf::FloatRect r3({POS_BOTON_RES_MENOS_X, FILA_RES_Y - 25.f}, {50.f, 50.f}); dibujarBoton3D(window, r3, r3.contains(mousePos), sf::Color(80, 80, 80)); centrarTexto(tResMenos, POS_BOTON_RES_MENOS_X + 25.f, FILA_RES_Y); window.draw(tResMenos);
                sf::FloatRect r4({POS_BOTON_RES_MAS_X, FILA_RES_Y - 25.f}, {50.f, 50.f}); dibujarBoton3D(window, r4, r4.contains(mousePos), sf::Color(80, 80, 80)); centrarTexto(tResMas, POS_BOTON_RES_MAS_X + 25.f, FILA_RES_Y); window.draw(tResMas);
            }
            else if (pestanaActual == 1) {
                // AUDIO (Usando el Panel de Control)
                txtTituloOpciones.setString("AUDIO SETTINGS"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);

                alinearIzquierda(tAudioMaster, AUDIO_TEXTO_X, AUDIO_MASTER_Y); window.draw(tAudioMaster);
                dibujarBarraVolumen(window, AUDIO_BARRA_X, AUDIO_MASTER_Y - 10.f, config.volMaster, 10);
                sf::Text v1(fontTitulo, std::to_string(config.volMaster) + "/10", 25); alinearIzquierda(v1, AUDIO_NUMERO_X, AUDIO_MASTER_Y); window.draw(v1);

                alinearIzquierda(tAudioMusic, AUDIO_TEXTO_X, AUDIO_MUSIC_Y); window.draw(tAudioMusic);
                dibujarBarraVolumen(window, AUDIO_BARRA_X, AUDIO_MUSIC_Y - 10.f, config.volMusica, 10);
                sf::Text v2(fontTitulo, std::to_string(config.volMusica) + "/10", 25); alinearIzquierda(v2, AUDIO_NUMERO_X, AUDIO_MUSIC_Y); window.draw(v2);

                alinearIzquierda(tAudioSFX, AUDIO_TEXTO_X, AUDIO_SFX_Y); window.draw(tAudioSFX);
                dibujarBarraVolumen(window, AUDIO_BARRA_X, AUDIO_SFX_Y - 10.f, config.volEfectos, 10);
                sf::Text v3(fontTitulo, std::to_string(config.volEfectos) + "/10", 25); alinearIzquierda(v3, AUDIO_NUMERO_X, AUDIO_SFX_Y); window.draw(v3);

                alinearIzquierda(tAudioStereo, AUDIO_TEXTO_X, AUDIO_ESTEREO_Y); window.draw(tAudioStereo);
                dibujarBotonONOFF(window, AUDIO_BARRA_X, AUDIO_ESTEREO_Y - 11.f, config.estereo);
            }
            else if (pestanaActual == 2) {
                txtTituloOpciones.setString("CONTROL SETTINGS"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);
                sf::Text t(fontTitulo, "Presiona una tecla para reasignar...", 25); t.setFillColor(sf::Color(150, 150, 150)); centrarTexto(t, 585.f, 250.f); window.draw(t);
            }
            else if (pestanaActual == 3) {
                // GAMEPLAY
                txtTituloOpciones.setString("GAMEPLAY SETTINGS"); centrarTexto(txtTituloOpciones, 585.f, 85.f); window.draw(txtTituloOpciones);

                alinearIzquierda(tGameplayShake, 260.f, 140.f); window.draw(tGameplayShake);
                dibujarBotonONOFF(window, 480.f, 130.f, config.screenShake);

                alinearIzquierda(tGameplayIntensidad, 260.f, 190.f); window.draw(tGameplayIntensidad);
                dibujarBarraVolumen(window, 320.f, 180.f, config.intensidadShake, 10);
                sf::Text vs(fontTitulo, std::to_string(config.intensidadShake) + "/10", 25); alinearIzquierda(vs, 540.f, 190.f); window.draw(vs);

                alinearIzquierda(tGameplayIdioma, 260.f, 250.f); window.draw(tGameplayIdioma);
                sf::RectangleShape d1({150.f, 25.f}); d1.setPosition({400.f, 240.f}); d1.setFillColor(sf::Color(50, 80, 120)); d1.setOutlineThickness(2.f); d1.setOutlineColor(sf::Color(20, 30, 40)); window.draw(d1);
                sf::Text t1(fontTitulo, config.idioma, 20); centrarTexto(t1, 475.f, 252.f); window.draw(t1);

                alinearIzquierda(tGameplayDificultad, 260.f, 310.f); window.draw(tGameplayDificultad);
                sf::RectangleShape d2({150.f, 25.f}); d2.setPosition({400.f, 300.f}); d2.setFillColor(sf::Color(50, 80, 120)); d2.setOutlineThickness(2.f); d2.setOutlineColor(sf::Color(20, 30, 40)); window.draw(d2);
                sf::Text t2(fontTitulo, config.dificultad, 20); centrarTexto(t2, 475.f, 312.f); window.draw(t2);
            }

            // Botón Volver
            bool hoverV = btnVolver.contains(mousePos);
            dibujarBoton3D(window, btnVolver, hoverV, sf::Color(180, 60, 60)); centrarTexto(txtVolver, 512.f, btnVolver.position.y + 30.f + (hoverV ? 2.f : 0.f)); window.draw(txtVolver);
        }
        else if (estadoActual == 1) {
            sf::Text textoJuego(fontTitulo, "EL JUEGO EMPIEZA AQUI", 40); textoJuego.setFillColor(sf::Color::White); centrarTexto(textoJuego, 512.f, 288.f); window.draw(textoJuego);
        }

        window.display();
    }
    return 0;
}