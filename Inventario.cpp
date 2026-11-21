#include "Inventario.h"

Inventario::Inventario() {
    cantidad = 0;
    for (int i = 0; i < MAX_CAPACIDAD; i++) {
        chapas[i] = nullptr;
    }
}

Inventario::~Inventario() {
    limpiar();
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

bool Inventario::estaLleno() const { return cantidad >= MAX_CAPACIDAD; }

bool Inventario::contiene(const Chapa* chapa) const {
    if (chapa == nullptr) return false;
    for (int i = 0; i < cantidad; i++) {
        if (*chapas[i] == *chapa) return true;
    }
    return false;
}

int Inventario::getCantidad() const { return cantidad; }
int Inventario::getCapacidad() const { return MAX_CAPACIDAD; }

Chapa* Inventario::getChapa(int indice) const {
    if (indice < 0 || indice >= cantidad) return nullptr;
    return chapas[indice];
}
