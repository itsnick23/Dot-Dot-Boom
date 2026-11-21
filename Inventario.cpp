#include "Inventario.h"

Inventario::Inventario(int cap)
    : chapas(nullptr), cantidad(0), capacidad(cap < 1 ? 5 : cap) {
    chapas = new Chapa*[capacidad];
    for (int i = 0; i < capacidad; i++) {
        chapas[i] = nullptr;
    }
}

Inventario::~Inventario() {
    limpiar();
    delete[] chapas;
    chapas = nullptr;
}

bool Inventario::agregar(Chapa* chapa) {
    if (chapa == nullptr) return false;
    if (estaLleno() || contiene(chapa)) return false;

    chapas[cantidad] = chapa->clonar();
    cantidad++;
    return true;
}

bool Inventario::quitar(int indice) {
    if (indice < 0 || indice >= cantidad) return false;

    delete chapas[indice];
    for (int i = indice; i < cantidad - 1; i++) {
        chapas[i] = chapas[i + 1];
    }
    chapas[cantidad - 1] = nullptr;
    cantidad--;
    return true;
}

void Inventario::limpiar() {
    for (int i = 0; i < cantidad; i++) {
        delete chapas[i];
        chapas[i] = nullptr;
    }
    cantidad = 0;
}

bool Inventario::estaLleno() const { return cantidad >= capacidad; }

bool Inventario::contiene(const Chapa* chapa) const {
    if (chapa == nullptr) return false;
    for (int i = 0; i < cantidad; i++) {
        if (*chapas[i] == *chapa) return true;
    }
    return false;
}

int Inventario::getCantidad() const { return cantidad; }
int Inventario::getCapacidad() const { return capacidad; }

Chapa* Inventario::getChapa(int indice) const {
    if (indice < 0 || indice >= cantidad) return nullptr;
    return chapas[indice];
}
