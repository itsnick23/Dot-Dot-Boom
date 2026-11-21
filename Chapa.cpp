#include "Chapa.h"

Chapa::Chapa(int id, const string& nombre, const string& descripcion, int costo, const string& rareza)
    : id(id), nombre(nombre), descripcion(descripcion), costo(costo < 0 ? 0 : costo), rareza(rareza) {
}

Chapa::~Chapa() {}

bool Chapa::alPisarMina(int& vidas) {
    return false;
}

void Chapa::reiniciarRonda() {}

int Chapa::getId() const { return id; }
string Chapa::getNombre() const { return nombre; }
string Chapa::getDescripcion() const { return descripcion; }
int Chapa::getCosto() const { return costo; }
string Chapa::getRareza() const { return rareza; }

bool Chapa::operator==(const Chapa& otra) const {
    return id == otra.id;
}

ChapaArtilleria::ChapaArtilleria()
    : Chapa(1, "Chapa de Artilleria", "+0.5 Mult si hay 2 o mas minas cerca", 5, "Rara"), multExtra(0.5f) {
}

void ChapaArtilleria::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
    if (minasCerca >= 2) {
        mult += multExtra;
    }
}

Chapa* ChapaArtilleria::clonar() const {
    return new ChapaArtilleria(*this);
}

ChapaBlindaje::ChapaBlindaje()
    : Chapa(2, "Chapa Kevlar", "Bloquea la primera mina de cada ronda", 6, "Epica"), escudoActivo(true) {
}

void ChapaBlindaje::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
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

ChapaReconocimiento::ChapaReconocimiento()
    : Chapa(3, "Chapa Recon", "+15 Fichas por cada celda en cascada", 4, "Comun"), fichasExtra(15) {
}

void ChapaReconocimiento::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
    if (esCascada) {
        fichas += fichasExtra;
    }
}

Chapa* ChapaReconocimiento::clonar() const {
    return new ChapaReconocimiento(*this);
}

ChapaFrancotirador::ChapaFrancotirador()
    : Chapa(4, "Chapa Francotirador", "+20 Fichas si hay exactamente 1 mina cerca", 4, "Comun"), fichasExtra(20) {
}

void ChapaFrancotirador::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
    if (!esCascada && minasCerca == 1) {
        fichas += fichasExtra;
    }
}

Chapa* ChapaFrancotirador::clonar() const {
    return new ChapaFrancotirador(*this);
}

ChapaMortero::ChapaMortero()
    : Chapa(5, "Chapa Mortero", "+1.0 Mult si hay 3 o mas minas cerca", 6, "Rara"), multExtra(1.0f) {
}

void ChapaMortero::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
    if (minasCerca >= 3) {
        mult += multExtra;
    }
}

Chapa* ChapaMortero::clonar() const {
    return new ChapaMortero(*this);
}

ChapaVeterano::ChapaVeterano()
    : Chapa(6, "Chapa Veterano", "+35 Fichas base en cada celda destapada", 7, "Epica"), fichasFijas(35) {
}

void ChapaVeterano::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
    if (!esCascada) {
        fichas += fichasFijas;
    }
}

Chapa* ChapaVeterano::clonar() const {
    return new ChapaVeterano(*this);
}

ChapaComandante::ChapaComandante()
    : Chapa(7, "Chapa Comandante", "x1.5 Mult al abrir una casilla despejada (0 minas)", 9, "Legendaria"), factorMult(1.5f) {
}

void ChapaComandante::alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) {
    if (!esCascada && minasCerca == 0) {
        mult *= factorMult;
    }
}

Chapa* ChapaComandante::clonar() const {
    return new ChapaComandante(*this);
}
