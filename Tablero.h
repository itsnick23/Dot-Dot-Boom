#pragma once
#include <iostream>
#include "Celda.h"
using namespace std;

struct Coord {
    int fila;
    int col;
};

class Tablero {
private:
    int filas;
    int columnas;
    int totalMinas;
    int celdasSegurasRestantes;
    bool minasColocadas;
    Celda** celdas;

    void liberar();
    bool esValida(int fila, int col) const;
    bool tableroConexo(int inicioFila, int inicioCol) const;
    void colocarMinas(int seguraFila, int seguraCol);
    void contarMinasVecinas();
    int expandirCascada(int fila, int col, int& sumaMinasCerca);

public:
    Tablero(int filas = 8, int columnas = 8, int minas = 10);
    ~Tablero();

    void configurar(int filas, int columnas, int minas);
    void reiniciar();

    // devuelve cuantas celdas abrio, -1 si hay mina, 0 si no hizo nada
    int destapar(int fila, int col, int& ultimaMinaCerca, int& celdasCascada);
    int destapar(Coord pos, int& ultimaMinaCerca, int& celdasCascada);
    // -1 si no se pudo tirar, 0 si no habia mina (revela solo esa), >=1 si habia mina (revela 3x3 y devuelve cuantas seguras abrio + 1)
    int tirarZapato(int fila, int col, int& minasCerca, int& segurasReveladas);

    void alternarBandera(int fila, int col);

    bool estaCompletado() const;
    int getFilas() const;
    int getColumnas() const;
    int getTotalMinas() const;
    int getCeldasSegurasRestantes() const;
    const Celda& getCelda(int fila, int col) const;
};
