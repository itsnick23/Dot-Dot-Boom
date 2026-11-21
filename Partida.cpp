#include "Partida.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

Partida::Partida(const string& dif)
    : vidas(3), maxVidas(3), ronda(1), maxRondas(10),
      puntajeTotal(0), dinero(0), zapatos(1), costoZapato(3), zapatosRestantesTienda(5),
      contadorTienda(0), tiendaDisponible(false), enTienda(false), dificultad(dif),
      inventario(5), catalogoChapas(nullptr), cantCatalogo(9), cantTienda(3) {
    for (int i = 0; i < 3; i++) tiendaActual[i] = nullptr;

    inicializarCatalogo();
    nuevaPartida(dif);
}

Partida::~Partida() {
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
    catalogoChapas[0] = new ChapaZapador();
    catalogoChapas[1] = new ChapaSaqueador();
    catalogoChapas[2] = new ChapaEco();
    catalogoChapas[3] = new ChapaMedico();
    catalogoChapas[4] = new ChapaTactica();
    catalogoChapas[5] = new ChapaBlindaje();
    catalogoChapas[6] = new ChapaBlindajePesado();
    catalogoChapas[7] = new ChapaFenix();
    catalogoChapas[8] = new ChapaSuministros();
}

void Partida::generarTienda() {
    for (int i = 0; i < 3; i++) tiendaActual[i] = nullptr;

    int disponibles = 0;
    for (int i = 0; i < cantCatalogo; i++) {
        if (!inventario.contiene(catalogoChapas[i])) {
            disponibles++;
        }
    }

    cantTienda = (disponibles < 3) ? disponibles : 3;
    int elegidas = 0;
    while (elegidas < cantTienda) {
        int idx = rand() % cantCatalogo;
        Chapa* candidata = catalogoChapas[idx];

        if (inventario.contiene(candidata)) continue;

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
    inventario.limpiar();
    iniciarSiguienteRonda();
}

void Partida::iniciarSiguienteRonda() {
    int minas = 7 + (ronda - 1) * 2;
    int tam = 5 + (ronda - 1) * 2;
    if (dificultad == "Dificil") {
        minas += 2;
    } else if (dificultad == "Facil" && minas > 5) {
        minas -= 2;
    }

    tablero.configurar(tam, tam, minas);
    for (int i = 0; i < inventario.getCantidad(); i++) {
        inventario.getChapa(i)->reiniciarRonda();
        inventario.getChapa(i)->alIniciarRonda(vidas, maxVidas, zapatos, dinero);
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
        for (int i = 0; i < inventario.getCantidad(); i++) {
            if (inventario.getChapa(i)->alPisarMina(vidas)) {
                bloqueada = true;
                break;
            }
        }
        if (!bloqueada && vidas > 0) {
            vidas--;
        }
        return true;
    }

    int resueltas = 1 + cascada;
    dinero += resueltas;
    puntajeTotal += resueltas * 10;

    for (int i = 0; i < inventario.getCantidad(); i++) {
        inventario.getChapa(i)->alDestapar(dinero, zapatos, minasCerca, cascada);
    }

    if (!tiendaDisponible) {
        contadorTienda += resueltas;
        if (contadorTienda >= 5) {
            tiendaDisponible = true;
        }
    }

    return true;
}

int Partida::usarZapato(int fila, int col) {
    if (estaTerminada() || enTienda || zapatos <= 0) return -1;

    int minasCerca = 0;
    int seguras = 0;
    int res = tablero.tirarZapato(fila, col, minasCerca, seguras);
    if (res == -1) return -1;

    zapatos--;

    if (seguras > 0) {
        dinero += seguras;
        puntajeTotal += seguras * 10;
        if (!tiendaDisponible) {
            contadorTienda += seguras;
            if (contadorTienda >= 5) {
                tiendaDisponible = true;
            }
        }
        if (res == 0) {
            for (int i = 0; i < inventario.getCantidad(); i++) {
                inventario.getChapa(i)->alDestapar(dinero, zapatos, minasCerca, 0);
            }
        }
    }

    return res;
}

void Partida::marcarCelda(int fila, int col) {
    if (estaTerminada() || enTienda) return;
    tablero.alternarBandera(fila, col);
}

void Partida::cobrarRonda() {
    if (!rondaSuperada() || partidaGanada()) return;
    puntajeTotal += ronda * 100;
    for (int i = 0; i < inventario.getCantidad(); i++) {
        inventario.getChapa(i)->alSuperarRonda(vidas, maxVidas, dinero, zapatos);
    }
    ronda++;
    if (ronda <= maxRondas) {
        iniciarSiguienteRonda();
    }
}

bool Partida::equiparChapa(Chapa* chapa) {
    return inventario.agregar(chapa);
}

bool Partida::venderChapa(int indice) {
    Chapa* c = inventario.getChapa(indice);
    if (c == nullptr) return false;
    int reembolso = c->getCosto() / 2;
    if (inventario.quitar(indice)) {
        dinero += reembolso;
        return true;
    }
    return false;
}

bool Partida::comprarChapa(int idChapa) {
    int slot = -1;
    Chapa* encontrada = nullptr;
    for (int i = 0; i < cantTienda; i++) {
        if (tiendaActual[i] != nullptr && tiendaActual[i]->getId() == idChapa) {
            encontrada = tiendaActual[i];
            slot = i;
            break;
        }
    }
    if (encontrada == nullptr) return false;
    if (dinero < encontrada->getCosto()) return false;

    if (equiparChapa(encontrada)) {
        dinero -= encontrada->getCosto();
        tiendaActual[slot] = nullptr;
        return true;
    }
    return false;
}

bool Partida::comprarZapato() {
    if (zapatosRestantesTienda <= 0) return false;
    if (dinero < costoZapato) return false;
    dinero -= costoZapato;
    zapatos++;
    zapatosRestantesTienda--;
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
    archivo << limpio << " " << ronda << " " << puntajeTotal << "\n";
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
    archivo << inventario.getCantidad();
    for (int i = 0; i < inventario.getCantidad(); i++) {
        archivo << " " << inventario.getChapa(i)->getId();
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
        zapatosRestantesTienda = 5;
        generarTienda();
        for (int i = 0; i < inventario.getCantidad(); i++) {
            inventario.getChapa(i)->alAbrirTienda(zapatos);
        }
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
int Partida::getPuntajeTotal() const { return puntajeTotal; }
int Partida::getDinero() const { return dinero; }
int Partida::getZapatos() const { return zapatos; }
int Partida::getCostoZapato() const { return costoZapato; }
int Partida::getZapatosRestantesTienda() const { return zapatosRestantesTienda; }
int Partida::getContadorTienda() const { return contadorTienda; }
const Tablero& Partida::getTablero() const { return tablero; }
const Inventario& Partida::getInventario() const { return inventario; }

int Partida::getCantChapas() const { return inventario.getCantidad(); }
Chapa* Partida::getChapaEquipada(int indice) const {
    return inventario.getChapa(indice);
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
