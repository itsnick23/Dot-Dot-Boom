#include "Tablero.h"
#include <random>
#include <ctime>

Tablero::Tablero(int filas, int columnas, int minas)
    : filas(0), columnas(0), totalMinas(0), celdasSegurasRestantes(0), minasColocadas(false), celdas(nullptr) {
    configurar(filas, columnas, minas);
}

Tablero::~Tablero() {
    liberar();
}

void Tablero::liberar() {
    if (celdas != nullptr) {
        for (int i = 0; i < filas; i++) {
            delete[] celdas[i];
        }
        delete[] celdas;
        celdas = nullptr;
    }
}

void Tablero::configurar(int f, int c, int minas) {
    liberar();

    filas = (f < 4) ? 4 : f;
    columnas = (c < 4) ? 4 : c;
    int maxMinas = (filas * columnas) - 9;
    if (maxMinas < 1) maxMinas = 1;
    totalMinas = (minas < 1) ? 1 : (minas > maxMinas ? maxMinas : minas);

    celdas = new Celda*[filas];
    for (int i = 0; i < filas; i++) {
        celdas[i] = new Celda[columnas];
    }
    reiniciar();
}

void Tablero::reiniciar() {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            celdas[i][j].reiniciar();
        }
    }
    minasColocadas = false;
    celdasSegurasRestantes = (filas * columnas) - totalMinas;
}

bool Tablero::esValida(int fila, int col) const {
    return fila >= 0 && fila < filas && col >= 0 && col < columnas;
}

bool Tablero::tableroConexo(int inicioFila, int inicioCol) const {
    int total = filas * columnas;
    bool* visitado = new bool[total];
    Coord* cola = new Coord[total];
    for (int i = 0; i < total; i++) visitado[i] = false;

    int frente = 0, fondo = 0;
    cola[fondo++] = {inicioFila, inicioCol};
    visitado[inicioFila * columnas + inicioCol] = true;

    int alcanzables = 0;
    while (frente < fondo) {
        Coord actual = cola[frente++];
        alcanzables++;

        for (int df = -1; df <= 1; df++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (df == 0 && dc == 0) continue;
                int nf = actual.fila + df;
                int nc = actual.col + dc;
                if (!esValida(nf, nc)) continue;
                int idx = nf * columnas + nc;
                if (!visitado[idx] && !celdas[nf][nc].esMina()) {
                    visitado[idx] = true;
                    cola[fondo++] = {nf, nc};
                }
            }
        }
    }

    int segurasActuales = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (!celdas[i][j].esMina()) segurasActuales++;
        }
    }

    delete[] visitado;
    delete[] cola;
    return alcanzables == segurasActuales;
}

void Tablero::colocarMinas(int seguraFila, int seguraCol) {
    random_device rd;
    mt19937 gen(rd() ^ (unsigned int)time(nullptr));
    uniform_int_distribution<int> distFila(0, filas - 1);
    uniform_int_distribution<int> distCol(0, columnas - 1);

    int puestas = 0;
    int intentos = 0;
    int maxIntentos = filas * columnas * 20;

    while (puestas < totalMinas && intentos < maxIntentos) {
        intentos++;
        int f = distFila(gen);
        int c = distCol(gen);

        int df = f - seguraFila;
        int dc = c - seguraCol;
        if (df < 0) df = -df;
        if (dc < 0) dc = -dc;

        // que el primer clic nunca sea mina
        if (df <= 1 && dc <= 1) continue;
        if (celdas[f][c].esMina()) continue;

        celdas[f][c].colocarMina();
        if (!tableroConexo(seguraFila, seguraCol)) {
            // si encierra celdas seguras, la quitamos y probamos otro lugar
            celdas[f][c].reiniciar();
            continue;
        }
        puestas++;
    }

    totalMinas = puestas;
    celdasSegurasRestantes = (filas * columnas) - totalMinas;
    contarMinasVecinas();
    minasColocadas = true;
}

void Tablero::contarMinasVecinas() {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (celdas[i][j].esMina()) continue;
            int conteo = 0;
            for (int df = -1; df <= 1; df++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (df == 0 && dc == 0) continue;
                    int nf = i + df;
                    int nc = j + dc;
                    if (esValida(nf, nc) && celdas[nf][nc].esMina()) {
                        conteo++;
                    }
                }
            }
            celdas[i][j].setMinasCerca(conteo);
        }
    }
}

int Tablero::expandirCascada(int fila, int col, int& celdasConNumero) {
    int abiertas = 0;
    for (int df = -1; df <= 1; df++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (df == 0 && dc == 0) continue;
            int nf = fila + df;
            int nc = col + dc;
            if (!esValida(nf, nc)) continue;
            if (celdas[nf][nc].esMina() || celdas[nf][nc].estaRevelada() || celdas[nf][nc].estaMarcada()) continue;

            if (celdas[nf][nc].revelar()) {
                abiertas++;
                celdasSegurasRestantes--;
                if (celdas[nf][nc].getMinasCerca() > 0) {
                    celdasConNumero++;
                } else {
                    abiertas += expandirCascada(nf, nc, celdasConNumero);
                }
            }
        }
    }
    return abiertas;
}

int Tablero::destapar(int fila, int col, int& ultimaMinaCerca, int& celdasCascada, int& cascadaConNumero) {
    ultimaMinaCerca = 0;
    celdasCascada = 0;
    cascadaConNumero = 0;
    if (!esValida(fila, col)) return 0;

    if (!minasColocadas) {
        colocarMinas(fila, col);
    }

    if (!celdas[fila][col].revelar()) return 0;

    if (celdas[fila][col].esMina()) {
        return -1;
    }

    celdasSegurasRestantes--;
    ultimaMinaCerca = celdas[fila][col].getMinasCerca();

    if (ultimaMinaCerca == 0) {
        celdasCascada = expandirCascada(fila, col, cascadaConNumero);
        return 1 + celdasCascada;
    }

    return 1;
}

int Tablero::destapar(Coord pos, int& ultimaMinaCerca, int& celdasCascada, int& cascadaConNumero) {
    return destapar(pos.fila, pos.col, ultimaMinaCerca, celdasCascada, cascadaConNumero);
}

int Tablero::tirarZapato(int fila, int col, int& minasCerca, int& segurasReveladas, int& segurasConNumero) {
    minasCerca = 0;
    segurasReveladas = 0;
    segurasConNumero = 0;
    if (!esValida(fila, col)) return -1;
    if (celdas[fila][col].estaRevelada()) return -1;

    if (!minasColocadas) {
        colocarMinas(fila, col);
    }

    if (celdas[fila][col].esMina()) {
        for (int df = -1; df <= 1; df++) {
            for (int dc = -1; dc <= 1; dc++) {
                int nf = fila + df;
                int nc = col + dc;
                if (!esValida(nf, nc) || celdas[nf][nc].estaRevelada()) continue;

                if (celdas[nf][nc].estaMarcada()) {
                    celdas[nf][nc].alternarMarca();
                }
                if (celdas[nf][nc].revelar() && !celdas[nf][nc].esMina()) {
                    celdasSegurasRestantes--;
                    segurasReveladas++;
                    if (celdas[nf][nc].getMinasCerca() > 0) {
                        segurasConNumero++;
                    }
                }
            }
        }
        return 1;
    }

    // si estaba marcada, la desmarca para poder revelarla
    if (celdas[fila][col].estaMarcada()) {
        celdas[fila][col].alternarMarca();
    }
    celdas[fila][col].revelar();
    celdasSegurasRestantes--;
    segurasReveladas = 1;
    minasCerca = celdas[fila][col].getMinasCerca();
    if (minasCerca > 0) segurasConNumero = 1;
    return 0;
}

void Tablero::alternarBandera(int fila, int col) {
    if (!esValida(fila, col)) return;
    celdas[fila][col].alternarMarca();
}

bool Tablero::estaCompletado() const { return celdasSegurasRestantes <= 0; }
int Tablero::getFilas() const { return filas; }
int Tablero::getColumnas() const { return columnas; }
int Tablero::getTotalMinas() const { return totalMinas; }
int Tablero::getCeldasSegurasRestantes() const { return celdasSegurasRestantes; }

const Celda& Tablero::getCelda(int fila, int col) const {
    if (!esValida(fila, col)) {
        static Celda vacia;
        return vacia;
    }
    return celdas[fila][col];
}
