#include "Celda.h"

Celda::Celda()
    : tieneMina(false), revelada(false), marcada(false), minasCerca(0) {
}

bool Celda::esMina() const { return tieneMina; }
bool Celda::estaRevelada() const { return revelada; }
bool Celda::estaMarcada() const { return marcada; }
int Celda::getMinasCerca() const { return minasCerca; }

void Celda::colocarMina() {
    tieneMina = true;
}

void Celda::setMinasCerca(int cantidad) {
    if (cantidad < 0) cantidad = 0;
    if (cantidad > 8) cantidad = 8;
    minasCerca = cantidad;
}

bool Celda::revelar() {
    if (revelada || marcada) return false;
    revelada = true;
    return true;
}

void Celda::marcar() {
    if (revelada) return;
    marcada = true;
}

void Celda::alternarMarca() {
    if (revelada) return;
    marcada = !marcada;
}

void Celda::reiniciar() {
    tieneMina = false;
    revelada = false;
    marcada = false;
    minasCerca = 0;
}
