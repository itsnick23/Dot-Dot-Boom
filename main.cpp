#include <SFML/Graphics.hpp>
#include "Jugador.h"
#include <iostream> 

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Dot Dot Boom");
    window.setFramerateLimit(60);

    Jugador miJugador;

    sf::CircleShape formaJugador(30.f);
    formaJugador.setFillColor(sf::Color::Green);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // --- CONEXIÓN 1: ENTRADA ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            miJugador.moverIzquierda();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            miJugador.moverDerecha();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            miJugador.moverArriba();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            miJugador.moverAbajo();


        formaJugador.setPosition({miJugador.getX(), miJugador.getY()});

        window.clear(sf::Color::Black);
        window.draw(formaJugador);
        window.display();

        std::cout << "X: " << miJugador.getX() << " | Y: " << miJugador.getY() << std::endl;
    }
    return 0;
}
