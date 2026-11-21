#include "Chapa.h"

Chapa::Chapa(int id, const string& nombre, const string& descripcion, int costo, const string& rareza)
    : id(id), nombre(nombre), descripcion(descripcion), costo(costo < 0 ? 0 : costo), rareza(rareza) {
}

Chapa::~Chapa() {}

void Chapa::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {}
void Chapa::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {}
bool Chapa::alPisarMina(int& vidas) { return false; }
void Chapa::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {}
void Chapa::alAbrirTienda(int& zapatos) {}
void Chapa::reiniciarRonda() {}

int Chapa::getId() const { return id; }
string Chapa::getNombre() const { return nombre; }
string Chapa::getDescripcion() const { return descripcion; }
int Chapa::getCosto() const { return costo; }
string Chapa::getRareza() const { return rareza; }

bool Chapa::operator==(const Chapa& otra) const {
    return id == otra.id;
}

ChapaZapador::ChapaZapador()
    : Chapa(1, "Chapa Zapador", "+1 Zapato gratis al inicio de cada ronda", 4, "Comun") {
}

void ChapaZapador::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {
    zapatos += 1;
}

Chapa* ChapaZapador::clonar() const {
    return new ChapaZapador(*this);
}

ChapaSaqueador::ChapaSaqueador()
    : Chapa(2, "Chapa Saqueador", "+1 Moneda extra al abrir celda con 2 o mas minas cerca", 4, "Comun") {
}

void ChapaSaqueador::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (minasCerca >= 2) {
        dinero += 1;
    }
}

Chapa* ChapaSaqueador::clonar() const {
    return new ChapaSaqueador(*this);
}

ChapaEco::ChapaEco()
    : Chapa(3, "Chapa Eco", "+1 Moneda extra cada 3 casillas abiertas en cascada", 5, "Comun") {
}

void ChapaEco::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (cantCascada >= 3) {
        dinero += cantCascada / 3;
    }
}

Chapa* ChapaEco::clonar() const {
    return new ChapaEco(*this);
}

ChapaMedico::ChapaMedico()
    : Chapa(4, "Chapa Medico", "Recupera +1 Vida al superar cada ronda", 6, "Rara") {
}

void ChapaMedico::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    if (vidas < maxVidas) {
        vidas += 1;
    }
}

Chapa* ChapaMedico::clonar() const {
    return new ChapaMedico(*this);
}

ChapaTactica::ChapaTactica()
    : Chapa(5, "Chapa Tactica", "+5 Monedas extra al superar cada ronda", 5, "Rara") {
}

void ChapaTactica::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    dinero += 5;
}

Chapa* ChapaTactica::clonar() const {
    return new ChapaTactica(*this);
}

ChapaBlindaje::ChapaBlindaje()
    : Chapa(6, "Chapa Kevlar", "Bloquea la primera mina de cada ronda", 6, "Epica"), escudoActivo(true) {
}

bool ChapaBlindaje::alPisarMina(int& vidas) {
    if (escudoActivo) {
        escudoActivo = false;
        return true;
    }
    return false;
}

void ChapaBlindaje::reiniciarRonda() {
    escudoActivo = true;
}

Chapa* ChapaBlindaje::clonar() const {
    return new ChapaBlindaje(*this);
}

ChapaBlindajePesado::ChapaBlindajePesado()
    : Chapa(7, "Chapa Blindaje Pesado", "Bloquea hasta 2 minas en cada ronda", 9, "Legendaria"), cargas(2) {
}

bool ChapaBlindajePesado::alPisarMina(int& vidas) {
    if (cargas > 0) {
        cargas--;
        return true;
    }
    return false;
}

void ChapaBlindajePesado::reiniciarRonda() {
    cargas = 2;
}

Chapa* ChapaBlindajePesado::clonar() const {
    return new ChapaBlindajePesado(*this);
}

ChapaFenix::ChapaFenix()
    : Chapa(8, "Chapa Fenix", "Evita la muerte una vez en toda la partida", 10, "Legendaria"), usada(false) {
}

bool ChapaFenix::alPisarMina(int& vidas) {
    if (!usada && vidas <= 1) {
        usada = true;
        return true;
    }
    return false;
}

Chapa* ChapaFenix::clonar() const {
    return new ChapaFenix(*this);
}

ChapaSuministros::ChapaSuministros()
    : Chapa(9, "Chapa Suministros", "+2 Zapatos gratis cada vez que abres la tienda", 9, "Legendaria") {
}

void ChapaSuministros::alAbrirTienda(int& zapatos) {
    zapatos += 2;
}

Chapa* ChapaSuministros::clonar() const {
    return new ChapaSuministros(*this);
}
