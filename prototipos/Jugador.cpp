#include "Jugador.h"

Jugador::Jugador()
    : x(400.0f), y(300.0f), velocidad(5.0f) {
}

void Jugador::moverArriba() { y -= velocidad; }
void Jugador::moverAbajo() { y += velocidad; }
void Jugador::moverIzquierda() { x -= velocidad; }
void Jugador::moverDerecha() { x += velocidad; }

float Jugador::getX() { return x; }
float Jugador::getY() { return y; }