#pragma once

class Jugador {
private:
    float x;
    float y;
    float velocidad;

public:
    Jugador();
    void moverArriba();
    void moverAbajo();
    void moverIzquierda();
    void moverDerecha();
    
    float getX();
    float getY();
};