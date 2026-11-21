#pragma once
#include <iostream>
#include <string>
#include "Tablero.h"
#include "Chapa.h"
using namespace std;

struct RegistroPuntaje {
    string nombre;
    int ronda;
    int puntaje;

    bool operator<(const RegistroPuntaje& otro) const {
        return puntaje > otro.puntaje;
    }

    bool operator==(const RegistroPuntaje& otro) const {
        return nombre == otro.nombre && ronda == otro.ronda && puntaje == otro.puntaje;
    }
};

class Partida {
private:
    Tablero tablero;
    int vidas;
    int maxVidas;
    int ronda;
    int maxRondas;
    int fichas;
    float mult;
    int puntajeRonda;
    int metaRonda;
    int puntajeTotal;
    int dinero;
    int zapatos;
    int costoZapato;
    int contadorTienda;
    bool tiendaDisponible;
    bool enTienda;
    string dificultad;

    Chapa** chapasEquipadas;
    int cantChapas;
    int maxChapas;

    Chapa** catalogoChapas;
    int cantCatalogo;

    Chapa* tiendaActual[3];
    int cantTienda;

    void inicializarCatalogo();
    void generarTienda();
    void limpiarChapas();

public:
    Partida(const string& dificultad = "Normal");
    ~Partida();

    void nuevaPartida(const string& dificultad);
    void iniciarSiguienteRonda();

    bool jugarCelda(int fila, int col);
    // -1 si no pudo tirar, 0 si estaba segura, 1 si habia bomba
    int usarZapato(int fila, int col);
    void marcarCelda(int fila, int col);
    void cobrarRonda();

    bool equiparChapa(Chapa* chapa);
    bool comprarChapa(int idChapa);
    bool comprarZapato();
    Chapa* buscarChapa(int id) const;
    Chapa* buscarChapa(const string& nombre) const;

    bool guardarPuntaje(const string& rutaArchivo, const string& nombreJugador) const;
    int cargarPuntajes(const string& rutaArchivo, RegistroPuntaje* lista, int maxRegistros) const;
    bool guardarPartida(const string& rutaArchivo) const;
    bool cargarPartida(const string& rutaArchivo);
    bool borrarPartida(const string& rutaArchivo) const;

    bool estaTerminada() const;
    bool partidaGanada() const;
    bool rondaSuperada() const;
    bool puedeAbrirTienda() const;
    bool estaEnTienda() const;
    void abrirTienda();
    void cerrarTienda();

    int getVidas() const;
    int getRonda() const;
    int getMaxRondas() const;
    int getFichas() const;
    float getMult() const;
    int getPuntajeRonda() const;
    int getMetaRonda() const;
    int getPuntajeTotal() const;
    int getDinero() const;
    int getZapatos() const;
    int getCostoZapato() const;
    int getContadorTienda() const;
    const Tablero& getTablero() const;

    int getCantChapas() const;
    Chapa* getChapaEquipada(int indice) const;
    int getCantCatalogo() const;
    Chapa* getChapaCatalogo(int indice) const;
    int getCantTienda() const;
    Chapa* getChapaTienda(int indice) const;
};
