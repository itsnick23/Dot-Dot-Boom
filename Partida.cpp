#include "Partida.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

Partida::Partida(const string& dif)
    : vidas(3), maxVidas(3), ronda(1), maxRondas(10), fichas(0), mult(1.0f),
      puntajeRonda(0), metaRonda(150), puntajeTotal(0), dinero(0), zapatos(1), costoZapato(3),
      contadorTienda(0), tiendaDisponible(false), enTienda(false), dificultad(dif),
      chapasEquipadas(nullptr), cantChapas(0), maxChapas(5),
      catalogoChapas(nullptr), cantCatalogo(7), cantTienda(3) {
    chapasEquipadas = new Chapa*[maxChapas];
    for (int i = 0; i < maxChapas; i++) chapasEquipadas[i] = nullptr;
    for (int i = 0; i < 3; i++) tiendaActual[i] = nullptr;

    inicializarCatalogo();
    nuevaPartida(dif);
}

Partida::~Partida() {
    limpiarChapas();
    delete[] chapasEquipadas;
    chapasEquipadas = nullptr;

    if (catalogoChapas != nullptr) {
        for (int i = 0; i < cantCatalogo; i++) {
            delete catalogoChapas[i];
        }
        delete[] catalogoChapas;
        catalogoChapas = nullptr;
    }
}

void Partida::inicializarCatalogo() {
    catalogoChapas = new Chapa*[cantCatalogo];
    catalogoChapas[0] = new ChapaArtilleria();
    catalogoChapas[1] = new ChapaBlindaje();
    catalogoChapas[2] = new ChapaReconocimiento();
    catalogoChapas[3] = new ChapaFrancotirador();
    catalogoChapas[4] = new ChapaMortero();
    catalogoChapas[5] = new ChapaVeterano();
    catalogoChapas[6] = new ChapaComandante();
}

void Partida::generarTienda() {
    int elegidas = 0;
    while (elegidas < cantTienda) {
        int idx = rand() % cantCatalogo;
        Chapa* candidata = catalogoChapas[idx];

        bool repetida = false;
        for (int i = 0; i < elegidas; i++) {
            if (*tiendaActual[i] == *candidata) {
                repetida = true;
                break;
            }
        }
        if (repetida) continue;

        tiendaActual[elegidas] = candidata;
        elegidas++;
    }
}

void Partida::limpiarChapas() {
    for (int i = 0; i < cantChapas; i++) {
        delete chapasEquipadas[i];
        chapasEquipadas[i] = nullptr;
    }
    cantChapas = 0;
}

void Partida::nuevaPartida(const string& dif) {
    dificultad = dif;
    vidas = maxVidas;
    ronda = 1;
    puntajeTotal = 0;
    dinero = 0;
    zapatos = 1;
    contadorTienda = 0;
    tiendaDisponible = false;
    enTienda = false;
    limpiarChapas();
    iniciarSiguienteRonda();
}

void Partida::iniciarSiguienteRonda() {
    fichas = 0;
    mult = 1.0f;
    puntajeRonda = 0;
    metaRonda = 120 + (ronda - 1) * 90;

    int minas = 7 + (ronda - 1) * 2;
    int tam = 5 + (ronda - 1);
    if (dificultad == "Dificil") {
        minas += 2;
    } else if (dificultad == "Facil" && minas > 5) {
        minas -= 2;
    }

    tablero.configurar(tam, tam, minas);
    for (int i = 0; i < cantChapas; i++) {
        chapasEquipadas[i]->reiniciarRonda();
    }
}

bool Partida::jugarCelda(int fila, int col) {
    if (estaTerminada() || enTienda) return false;

    int minasCerca = 0;
    int cascada = 0;
    int resultado = tablero.destapar(fila, col, minasCerca, cascada);

    if (resultado == 0) return false;

    if (resultado == -1) {
        bool bloqueada = false;
        for (int i = 0; i < cantChapas; i++) {
            if (chapasEquipadas[i]->alPisarMina(vidas)) {
                bloqueada = true;
                break;
            }
        }
        if (!bloqueada && vidas > 0) {
            vidas--;
        }
        return true;
    }

    int fichasGanadas = 10 + (minasCerca * 5);
    fichas += fichasGanadas;

    for (int i = 0; i < cantChapas; i++) {
        chapasEquipadas[i]->alDestapar(fichas, mult, minasCerca, false);
        for (int j = 0; j < cascada; j++) {
            chapasEquipadas[i]->alDestapar(fichas, mult, 0, true);
        }
    }

    if (cascada > 0) {
        fichas += cascada * 5;
    }

    int resueltas = 1 + cascada;
    dinero += resueltas;
    if (!tiendaDisponible) {
        contadorTienda += resueltas;
        if (contadorTienda >= 5) {
            tiendaDisponible = true;
        }
    }

    puntajeRonda = static_cast<int>(fichas * mult + 0.5f);
    return true;
}

int Partida::usarZapato(int fila, int col) {
    if (estaTerminada() || enTienda || zapatos <= 0) return -1;

    int minasCerca = 0;
    int res = tablero.tirarZapato(fila, col, minasCerca);
    if (res == -1) return -1;

    zapatos--;

    if (res == 0) {
        dinero += 1;
        if (!tiendaDisponible) {
            contadorTienda += 1;
            if (contadorTienda >= 5) {
                tiendaDisponible = true;
            }
        }
        int fichasGanadas = 10 + (minasCerca * 5);
        fichas += fichasGanadas;
        for (int i = 0; i < cantChapas; i++) {
            chapasEquipadas[i]->alDestapar(fichas, mult, minasCerca, false);
        }
        puntajeRonda = static_cast<int>(fichas * mult + 0.5f);
    }

    return res;
}

void Partida::marcarCelda(int fila, int col) {
    if (estaTerminada() || enTienda) return;
    tablero.alternarBandera(fila, col);
}

void Partida::cobrarRonda() {
    if (!rondaSuperada() || partidaGanada()) return;
    puntajeTotal += puntajeRonda;
    ronda++;
    if (ronda <= maxRondas) {
        iniciarSiguienteRonda();
    }
}

bool Partida::equiparChapa(Chapa* chapa) {
    if (chapa == nullptr) return false;
    if (cantChapas >= maxChapas) return false;

    for (int i = 0; i < cantChapas; i++) {
        if (*chapasEquipadas[i] == *chapa) return false;
    }

    chapasEquipadas[cantChapas] = chapa->clonar();
    cantChapas++;
    return true;
}

bool Partida::comprarChapa(int idChapa) {
    Chapa* encontrada = nullptr;
    for (int i = 0; i < cantTienda; i++) {
        if (tiendaActual[i] != nullptr && tiendaActual[i]->getId() == idChapa) {
            encontrada = tiendaActual[i];
            break;
        }
    }
    if (encontrada == nullptr) return false;
    if (dinero < encontrada->getCosto()) return false;

    if (equiparChapa(encontrada)) {
        dinero -= encontrada->getCosto();
        return true;
    }
    return false;
}

bool Partida::comprarZapato() {
    if (dinero < costoZapato) return false;
    dinero -= costoZapato;
    zapatos++;
    return true;
}

Chapa* Partida::buscarChapa(int id) const {
    for (int i = 0; i < cantCatalogo; i++) {
        if (catalogoChapas[i]->getId() == id) return catalogoChapas[i];
    }
    return nullptr;
}

Chapa* Partida::buscarChapa(const string& nombre) const {
    for (int i = 0; i < cantCatalogo; i++) {
        if (catalogoChapas[i]->getNombre() == nombre) return catalogoChapas[i];
    }
    return nullptr;
}

bool Partida::guardarPuntaje(const string& rutaArchivo, const string& nombreJugador) const {
    ofstream archivo(rutaArchivo, ios::app);
    if (!archivo.is_open()) return false;

    string limpio = nombreJugador.empty() ? "Soldado" : nombreJugador;
    archivo << limpio << " " << ronda << " " << (puntajeTotal + puntajeRonda) << "\n";
    archivo.close();
    return true;
}

int Partida::cargarPuntajes(const string& rutaArchivo, RegistroPuntaje* lista, int maxRegistros) const {
    if (lista == nullptr || maxRegistros <= 0) return 0;

    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) return 0;

    int cantidad = 0;
    string linea;
    while (cantidad < maxRegistros && getline(archivo, linea)) {
        istringstream iss(linea);
        RegistroPuntaje reg;
        if (iss >> reg.nombre >> reg.ronda >> reg.puntaje) {
            lista[cantidad] = reg;
            cantidad++;
        }
    }
    archivo.close();

    // ordena de mayor a menor
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - 1 - i; j++) {
            if (lista[j + 1] < lista[j]) {
                RegistroPuntaje aux = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = aux;
            }
        }
    }

    return cantidad;
}

bool Partida::guardarPartida(const string& rutaArchivo) const {
    ofstream archivo(rutaArchivo);
    if (!archivo.is_open()) return false;

    archivo << dificultad << "\n";
    archivo << ronda << " " << vidas << " " << dinero << " " << zapatos << " "
            << puntajeTotal << " " << contadorTienda << " " << (tiendaDisponible ? 1 : 0) << "\n";
    archivo << cantChapas;
    for (int i = 0; i < cantChapas; i++) {
        archivo << " " << chapasEquipadas[i]->getId();
    }
    archivo << "\n";
    archivo.close();
    return true;
}

bool Partida::cargarPartida(const string& rutaArchivo) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) return false;

    string dif;
    int r = 1, v = 3, din = 0, zap = 1, pt = 0, contT = 0, tiendaDisp = 0;
    if (!getline(archivo, dif)) return false;
    if (!(archivo >> r >> v >> din >> zap >> pt >> contT >> tiendaDisp)) return false;

    int cant = 0;
    if (!(archivo >> cant)) cant = 0;

    nuevaPartida(dif);
    ronda = (r < 1) ? 1 : (r > maxRondas ? maxRondas : r);
    vidas = (v < 1) ? 1 : (v > maxVidas ? maxVidas : v);
    dinero = (din < 0) ? 0 : din;
    zapatos = (zap < 0) ? 0 : zap;
    puntajeTotal = (pt < 0) ? 0 : pt;
    contadorTienda = (contT < 0) ? 0 : contT;
    tiendaDisponible = (tiendaDisp == 1);

    for (int i = 0; i < cant; i++) {
        int idChapa = 0;
        if (archivo >> idChapa) {
            Chapa* c = buscarChapa(idChapa);
            if (c != nullptr) equiparChapa(c);
        }
    }

    archivo.close();
    iniciarSiguienteRonda();
    return true;
}

bool Partida::borrarPartida(const string& rutaArchivo) const {
    return remove(rutaArchivo.c_str()) == 0;
}

bool Partida::estaTerminada() const { return vidas <= 0 || partidaGanada(); }
bool Partida::partidaGanada() const { return ronda > maxRondas || (ronda == maxRondas && rondaSuperada()); }
bool Partida::rondaSuperada() const { return tablero.estaCompletado(); }
bool Partida::puedeAbrirTienda() const { return tiendaDisponible; }
bool Partida::estaEnTienda() const { return enTienda; }

void Partida::abrirTienda() {
    if (tiendaDisponible) {
        generarTienda();
        enTienda = true;
    }
}

void Partida::cerrarTienda() {
    enTienda = false;
    tiendaDisponible = false;
    contadorTienda = 0;
}

int Partida::getVidas() const { return vidas; }
int Partida::getRonda() const { return ronda; }
int Partida::getMaxRondas() const { return maxRondas; }
int Partida::getFichas() const { return fichas; }
float Partida::getMult() const { return mult; }
int Partida::getPuntajeRonda() const { return puntajeRonda; }
int Partida::getMetaRonda() const { return metaRonda; }
int Partida::getPuntajeTotal() const { return puntajeTotal; }
int Partida::getDinero() const { return dinero; }
int Partida::getZapatos() const { return zapatos; }
int Partida::getCostoZapato() const { return costoZapato; }
int Partida::getContadorTienda() const { return contadorTienda; }
const Tablero& Partida::getTablero() const { return tablero; }

int Partida::getCantChapas() const { return cantChapas; }
Chapa* Partida::getChapaEquipada(int indice) const {
    if (indice < 0 || indice >= cantChapas) return nullptr;
    return chapasEquipadas[indice];
}

int Partida::getCantCatalogo() const { return cantCatalogo; }
Chapa* Partida::getChapaCatalogo(int indice) const {
    if (indice < 0 || indice >= cantCatalogo) return nullptr;
    return catalogoChapas[indice];
}

int Partida::getCantTienda() const { return cantTienda; }
Chapa* Partida::getChapaTienda(int indice) const {
    if (indice < 0 || indice >= cantTienda) return nullptr;
    return tiendaActual[indice];
}
