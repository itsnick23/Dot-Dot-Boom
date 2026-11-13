#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <vector>
#include <random>
#include <string>

// --- ESTRUCTURAS DE EFECTOS ---
struct Libelula {
    sf::CircleShape cuerpo;
    sf::RectangleShape alas;
    float angulo;
    float velocidad;
    float radioVuelo;
    sf::Vector2f centro;
};

struct Chispa {
    sf::CircleShape forma;
    float velocidadY;
    float velocidadX;
    float vida;
    float vidaMaxima;
};

int main() {
    sf::RenderWindow window(sf::VideoMode({1024, 576}), "High-Res Pixel Art - Campfire");
    window.setFramerateLimit(120);

    sf::Font fontUI;
    if (!fontUI.openFromFile("C:/Windows/Fonts/arial.ttf")) return -1;
    sf::Font fontTitulo;
    if (!fontTitulo.openFromFile("C:/Windows/Fonts/consola.ttf")) return -1;

    // --- TEXTOS ---
    sf::Text titulo(fontTitulo, "DOT DOT BOOM", 60);
    titulo.setFillColor(sf::Color(255, 240, 220));
    titulo.setOutlineColor(sf::Color(50, 20, 30));
    titulo.setOutlineThickness(4);
    sf::FloatRect tRect = titulo.getLocalBounds();
    titulo.setOrigin({tRect.position.x + tRect.size.x / 2.0f, tRect.position.y + tRect.size.y / 2.0f});
    titulo.setPosition({512.f, 100.f});

    sf::Text textoBoton(fontUI, "PRESIONA ENTER", 24);
    textoBoton.setFillColor(sf::Color(255, 220, 150));
    sf::FloatRect bRect = textoBoton.getLocalBounds();
    textoBoton.setOrigin({bRect.position.x + bRect.size.x / 2.0f, bRect.position.y + bRect.size.y / 2.0f});
    textoBoton.setPosition({512.f, 520.f});

    // --- PARTÍCULAS DE FUEGO ---
    std::vector<Chispa> chispas;
    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> disX(-10.f, 10.f);
    std::uniform_real_distribution<float> disY(-3.f, -1.f);
    
    // --- LIBÉLULAS ---
    std::vector<Libelula> libelulas;
    for (int i = 0; i < 5; i++) {
        Libelula l;
        l.cuerpo.setRadius(2.f);
        l.cuerpo.setFillColor(sf::Color(150, 255, 255, 200)); // Cian brillante
        l.alas.setSize({6.f, 2.f});
        l.alas.setFillColor(sf::Color(255, 255, 255, 100)); // Blancas transparentes
        l.angulo = rand() % 360;
        l.velocidad = (rand() % 100) / 1000.f + 0.01f;
        l.radioVuelo = 50.f + (rand() % 100);
        l.centro = sf::Vector2f(300.f + (rand() % 400), 300.f + (rand() % 200));
        libelulas.push_back(l);
    }

    float tiempoGlobal = 0.f;
    bool enMenu = true;

    // --- BUCLE PRINCIPAL ---
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
                
            if (enMenu) {
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->scancode == sf::Keyboard::Scancode::Enter) enMenu = false;
                }
            }
        }

        tiempoGlobal += 0.016f;

        window.clear();

        if (enMenu) {
            // ==========================================
            // 1. CIELO DEGRADADO (Rosa a Azul Oscuro)
            // ==========================================
            for (int i = 0; i < 576; i += 4) {
                sf::RectangleShape franja({1024.f, 4.f});
                franja.setPosition({0.f, (float)i});
                float ratio = i / 576.f;
                // Mezcla de colores (Magenta/Púrpura arriba -> Azul profundo abajo)
                int r = 80 + (1 - ratio) * 100;
                int g = 20 + (1 - ratio) * 30;
                int b = 100 + ratio * 80;
                franja.setFillColor(sf::Color(r, g, b));
                window.draw(franja);
            }

            // ==========================================
            // 2. MONTAÑAS Y CASTILLOS (Siluetas)
            // ==========================================
            // Capa lejana (Púrpura oscuro)
            sf::CircleShape montana1(300.f); montana1.setPointCount(3);
            montana1.setFillColor(sf::Color(40, 20, 60)); montana1.setPosition({-100.f, 100.f});
            window.draw(montana1);
            montana1.setPosition({600.f, 50.f}); window.draw(montana1);

            // Capa media (Azul muy oscuro)
            sf::RectangleShape castillo({80.f, 200.f});
            castillo.setFillColor(sf::Color(20, 15, 40));
            castillo.setPosition({150.f, 200.f}); window.draw(castillo);
            castillo.setPosition({800.f, 150.f}); window.draw(castillo);

            // ==========================================
            // 3. AGUA / TIERRA (Verde Cian Brillante)
            // ==========================================
            sf::RectangleShape agua({1024.f, 250.f});
            agua.setPosition({0.f, 326.f});
            agua.setFillColor(sf::Color(80, 200, 180));
            window.draw(agua);

            // Efecto de ondas en el agua
            for(int i=0; i<10; i++) {
                sf::RectangleShape onda({50.f + rand()%100, 2.f});
                onda.setPosition({(float)(rand()%1024), 340.f + (float)(rand()%200)});
                onda.setFillColor(sf::Color(120, 240, 220, 100));
                window.draw(onda);
            }

            // ==========================================
            // 4. SUELO DE PIEDRA (Donde está la fogata)
            // ==========================================
            sf::RectangleShape suelo({1024.f, 100.f});
            suelo.setPosition({0.f, 476.f});
            suelo.setFillColor(sf::Color(100, 80, 120)); // Púrpura grisáceo
            window.draw(suelo);

            // Ruinas a la derecha
            sf::RectangleShape ruina({100.f, 150.f});
            ruina.setFillColor(sf::Color(40, 30, 60));
            ruina.setPosition({700.f, 350.f}); window.draw(ruina);
            ruina.setPosition({750.f, 280.f}); window.draw(ruina);
            ruina.setPosition({800.f, 380.f}); window.draw(ruina);

            // ==========================================
            // 5. LA FOGATA Y EL PERSONAJE
            // ==========================================
            sf::Vector2f posFogata(550.f, 490.f);

            // Luz de la fogata (Círculo gigante semitransparente)
            sf::CircleShape luz(150.f);
            luz.setOrigin({150.f, 150.f});
            luz.setPosition(posFogata);
            luz.setFillColor(sf::Color(255, 150, 50, 40)); // Naranja brillante
            window.draw(luz);

            // Personaje (Sentado)
            sf::RectangleShape cuerpo({16.f, 20.f});
            cuerpo.setFillColor(sf::Color(200, 50, 50)); // Túnica roja
            cuerpo.setPosition({500.f, 490.f});
            window.draw(cuerpo);
            sf::CircleShape cabeza(6.f);
            cabeza.setFillColor(sf::Color(255, 200, 150));
            cabeza.setPosition({505.f, 475.f});
            window.draw(cabeza);

            // El Fuego (Capas de triángulos/rectángulos animados)
            float llamaTam = 15.f + std::sin(tiempoGlobal * 15.f) * 5.f;
            sf::CircleShape fuego1(llamaTam);
            fuego1.setPointCount(3);
            fuego1.setFillColor(sf::Color(255, 50, 0)); // Rojo
            fuego1.setPosition({posFogata.x - llamaTam, posFogata.y - llamaTam*2});
            window.draw(fuego1);

            sf::CircleShape fuego2(llamaTam * 0.7f);
            fuego2.setPointCount(3);
            fuego2.setFillColor(sf::Color(255, 200, 0)); // Amarillo
            fuego2.setPosition({posFogata.x - llamaTam*0.7f, posFogata.y - llamaTam*1.5f});
            window.draw(fuego2);

            // ==========================================
            // 6. PARTÍCULAS DE FUEGO (Chispas)
            // ==========================================
            for (size_t i = 0; i < chispas.size(); i++) {
                chispas[i].forma.move({chispas[i].velocidadX, chispas[i].velocidadY});
                chispas[i].vida -= 0.02f;
                
                // Efecto de desvanecimiento
                sf::Color c = chispas[i].forma.getFillColor();
                c.a = static_cast<std::uint8_t>(chispas[i].vida * 255);
                chispas[i].forma.setFillColor(c);

                if (chispas[i].vida <= 0) {
                    chispas.erase(chispas.begin() + i);
                    i--;
                } else {
                    window.draw(chispas[i].forma);
                }
            }

            // Generar nuevas chispas
            if (rand() % 3 == 0) {
                Chispa c;
                c.forma.setRadius(1.5f + (rand()%2));
                c.forma.setFillColor(sf::Color(255, 200, 50, 255));
                c.forma.setPosition({posFogata.x + disX(gen), posFogata.y - 10.f});
                c.velocidadX = disX(gen) * 0.5f;
                c.velocidadY = disY(gen);
                c.vida = 1.0f;
                c.vidaMaxima = 1.0f;
                chispas.push_back(c);
            }

            // ==========================================
            // 7. LIBÉLULAS VOLANDO
            // ==========================================
            for (auto& l : libelulas) {
                l.angulo += l.velocidad;
                float x = l.centro.x + std::cos(l.angulo) * l.radioVuelo;
                float y = l.centro.y + std::sin(l.angulo * 2.f) * (l.radioVuelo * 0.5f); // Movimiento en 8
                
                l.cuerpo.setPosition({x, y});
                window.draw(l.cuerpo);

                // Alas (Aleteo rápido)
                float aleteo = std::sin(tiempoGlobal * 50.f) * 2.f;
                l.alas.setPosition({x - 6.f, y - 2.f + aleteo});
                window.draw(l.alas);
            }

            // ==========================================
            // 8. INTERFAZ GRÁFICA
            // ==========================================
            // Sombra del texto
            sf::Text tSombra = titulo;
            tSombra.setFillColor(sf::Color(0, 0, 0, 150));
            tSombra.setPosition({titulo.getPosition().x + 3.f, titulo.getPosition().y + 3.f});
            window.draw(tSombra);
            
            // Animación del texto (Flotando)
            titulo.setPosition({512.f, 100.f + std::sin(tiempoGlobal * 2.f) * 5.f});
            window.draw(titulo);

            // Texto de presionar Enter (Parpadeante)
            if (std::fmod(tiempoGlobal, 1.0f) < 0.7f) {
                window.draw(textoBoton);
            }

        } else {
            // --- PANTALLA DEL JUEGO ---
            window.clear(sf::Color(20, 20, 30));
            
            sf::Text textoJuego(fontUI, "AQUI EMPIEZA TU JUEGO", 40);
            textoJuego.setFillColor(sf::Color::White);
            sf::FloatRect jRect = textoJuego.getLocalBounds();
            textoJuego.setOrigin({jRect.position.x + jRect.size.x / 2.0f, jRect.position.y + jRect.size.y / 2.0f});
            textoJuego.setPosition({512.f, 288.f});
            window.draw(textoJuego);
        }

        window.display();
    }
    return 0;
}