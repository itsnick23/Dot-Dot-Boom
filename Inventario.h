#pragma once
#include <iostream>
#include "Chapa.h"
using namespace std;

class Inventario {
private:
    Chapa** chapas;
    int cantidad;
    int capacidad;

public:
    Inventario(int capacidad = 5);
    ~Inventario();

    bool agregar(Chapa* chapa);
    bool quitar(int indice);
    void limpiar();

    bool estaLleno() const;
    bool contiene(const Chapa* chapa) const;
    int getCantidad() const;
    int getCapacidad() const;
    Chapa* getChapa(int indice) const;
};
