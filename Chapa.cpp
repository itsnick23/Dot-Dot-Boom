#include "Chapa.h"
#include <cstdlib>

Chapa::Chapa(int id, const string& nombre, const string& descripcion, int costo, const string& rareza)
    : id(id), nombre(nombre), descripcion(descripcion), costo(costo < 0 ? 0 : costo), rareza(rareza) {
}

Chapa::~Chapa() {}

void Chapa::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {}
void Chapa::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {}
bool Chapa::alPisarMina(int& vidas, int& zapatos, int& dinero) { return false; }
void Chapa::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {}
void Chapa::alAbrirTienda(int& zapatos, int& dinero) {}
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

Chapa* ChapaZapador::clonar() const { return new ChapaZapador(*this); }

ChapaSaqueador::ChapaSaqueador()
    : Chapa(2, "Chapa Saqueador", "+1 Moneda extra al abrir celda con 2 o mas minas cerca", 4, "Comun") {
}

void ChapaSaqueador::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (minasCerca >= 2) {
        dinero += 1;
    }
}

Chapa* ChapaSaqueador::clonar() const { return new ChapaSaqueador(*this); }

ChapaEco::ChapaEco()
    : Chapa(3, "Chapa Eco", "+1 Moneda extra cada 3 casillas abiertas en cascada", 5, "Comun") {
}

void ChapaEco::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (cantCascada >= 3) {
        dinero += cantCascada / 3;
    }
}

Chapa* ChapaEco::clonar() const { return new ChapaEco(*this); }

ChapaExplorador::ChapaExplorador()
    : Chapa(4, "Chapa Explorador", "+1 Moneda extra al abrir una celda despejada (0 minas)", 4, "Comun") {
}

void ChapaExplorador::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (minasCerca == 0) {
        dinero += 1;
    }
}

Chapa* ChapaExplorador::clonar() const { return new ChapaExplorador(*this); }

ChapaChatarrero::ChapaChatarrero()
    : Chapa(5, "Chapa Chatarrero", "+2 Monedas gratis cada vez que abres la tienda", 4, "Comun") {
}

void ChapaChatarrero::alAbrirTienda(int& zapatos, int& dinero) {
    dinero += 2;
}

Chapa* ChapaChatarrero::clonar() const { return new ChapaChatarrero(*this); }

ChapaMedico::ChapaMedico()
    : Chapa(6, "Chapa Medico", "Recupera +1 Vida al superar cada ronda", 6, "Rara") {
}

void ChapaMedico::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    if (vidas < maxVidas) {
        vidas += 1;
    }
}

Chapa* ChapaMedico::clonar() const { return new ChapaMedico(*this); }

ChapaTactica::ChapaTactica()
    : Chapa(7, "Chapa Tactica", "+5 Monedas extra al superar cada ronda", 5, "Rara") {
}

void ChapaTactica::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    dinero += 5;
}

Chapa* ChapaTactica::clonar() const { return new ChapaTactica(*this); }

ChapaBotin::ChapaBotin()
    : Chapa(8, "Chapa Botin", "+1 Zapato gratis al superar cada ronda", 6, "Rara") {
}

void ChapaBotin::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    zapatos += 1;
}

Chapa* ChapaBotin::clonar() const { return new ChapaBotin(*this); }

ChapaAhorrista::ChapaAhorrista()
    : Chapa(9, "Chapa Ahorrista", "+1 Moneda por cada 5 que tengas al superar ronda (max +5)", 6, "Rara") {
}

void ChapaAhorrista::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    int bono = dinero / 5;
    if (bono > 5) bono = 5;
    dinero += bono;
}

Chapa* ChapaAhorrista::clonar() const { return new ChapaAhorrista(*this); }

ChapaBlindaje::ChapaBlindaje()
    : Chapa(10, "Chapa Kevlar", "Bloquea la primera mina de cada ronda", 6, "Epica"), escudoActivo(true) {
}

bool ChapaBlindaje::alPisarMina(int& vidas, int& zapatos, int& dinero) {
    if (escudoActivo) {
        escudoActivo = false;
        return true;
    }
    return false;
}

void ChapaBlindaje::reiniciarRonda() {
    escudoActivo = true;
}

Chapa* ChapaBlindaje::clonar() const { return new ChapaBlindaje(*this); }

ChapaParacaidas::ChapaParacaidas()
    : Chapa(11, "Chapa Paracaidas", "Si pisas mina y pierdes vida, ganas +1 Zapato y +2 Monedas", 7, "Epica") {
}

bool ChapaParacaidas::alPisarMina(int& vidas, int& zapatos, int& dinero) {
    zapatos += 1;
    dinero += 2;
    return false;
}

Chapa* ChapaParacaidas::clonar() const { return new ChapaParacaidas(*this); }

ChapaCuartel::ChapaCuartel()
    : Chapa(12, "Chapa Cuartel", "+1 Zapato y +3 Monedas al inicio de cada ronda", 7, "Epica") {
}

void ChapaCuartel::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {
    zapatos += 1;
    dinero += 3;
}

Chapa* ChapaCuartel::clonar() const { return new ChapaCuartel(*this); }

ChapaBlindajePesado::ChapaBlindajePesado()
    : Chapa(13, "Chapa Blindaje Pesado", "Bloquea hasta 2 minas en cada ronda", 9, "Legendaria"), cargas(2) {
}

bool ChapaBlindajePesado::alPisarMina(int& vidas, int& zapatos, int& dinero) {
    if (cargas > 0) {
        cargas--;
        return true;
    }
    return false;
}

void ChapaBlindajePesado::reiniciarRonda() {
    cargas = 2;
}

Chapa* ChapaBlindajePesado::clonar() const { return new ChapaBlindajePesado(*this); }

ChapaFenix::ChapaFenix()
    : Chapa(14, "Chapa Fenix", "Evita la muerte una vez en toda la partida", 10, "Legendaria"), usada(false) {
}

bool ChapaFenix::alPisarMina(int& vidas, int& zapatos, int& dinero) {
    if (!usada && vidas <= 1) {
        usada = true;
        return true;
    }
    return false;
}

Chapa* ChapaFenix::clonar() const { return new ChapaFenix(*this); }

ChapaSuministros::ChapaSuministros()
    : Chapa(15, "Chapa Suministros", "+2 Zapatos gratis cada vez que abres la tienda", 9, "Legendaria") {
}

void ChapaSuministros::alAbrirTienda(int& zapatos, int& dinero) {
    zapatos += 2;
}

Chapa* ChapaSuministros::clonar() const { return new ChapaSuministros(*this); }

ChapaVigilante::ChapaVigilante()
    : Chapa(16, "Chapa Vigilante", "+1 Moneda extra al abrir celda con 1 mina cerca", 4, "Comun") {
}

void ChapaVigilante::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (minasCerca == 1) {
        dinero += 1;
    }
}

Chapa* ChapaVigilante::clonar() const { return new ChapaVigilante(*this); }

ChapaRacion::ChapaRacion()
    : Chapa(17, "Chapa Racion", "+3 Monedas al inicio de cada ronda", 4, "Comun") {
}

void ChapaRacion::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {
    dinero += 3;
}

Chapa* ChapaRacion::clonar() const { return new ChapaRacion(*this); }

ChapaMochila::ChapaMochila()
    : Chapa(18, "Chapa Mochila", "+1 Zapato gratis cada vez que abres la tienda", 5, "Comun") {
}

void ChapaMochila::alAbrirTienda(int& zapatos, int& dinero) {
    zapatos += 1;
}

Chapa* ChapaMochila::clonar() const { return new ChapaMochila(*this); }

ChapaFrancotirador::ChapaFrancotirador()
    : Chapa(19, "Chapa Francotirador", "+2 Monedas extra al abrir celda con 3 o mas minas cerca", 5, "Rara") {
}

void ChapaFrancotirador::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    if (minasCerca >= 3) {
        dinero += 2;
    }
}

Chapa* ChapaFrancotirador::clonar() const { return new ChapaFrancotirador(*this); }

ChapaEnfermero::ChapaEnfermero()
    : Chapa(20, "Chapa Enfermero", "Si terminas la ronda con 1 vida, te cura a 2 vidas", 6, "Rara") {
}

void ChapaEnfermero::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    if (vidas == 1 && vidas < maxVidas) {
        vidas += 1;
    }
}

Chapa* ChapaEnfermero::clonar() const { return new ChapaEnfermero(*this); }

ChapaBrigada::ChapaBrigada()
    : Chapa(21, "Chapa Brigada", "Si empiezas la ronda sin zapatos, te da +1 Zapato", 5, "Rara") {
}

void ChapaBrigada::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {
    if (zapatos == 0) {
        zapatos += 1;
    }
}

Chapa* ChapaBrigada::clonar() const { return new ChapaBrigada(*this); }

ChapaCasco::ChapaCasco()
    : Chapa(22, "Chapa Casco", "50% de probabilidad de bloquear cualquier mina", 7, "Epica") {
}

bool ChapaCasco::alPisarMina(int& vidas, int& zapatos, int& dinero) {
    return (rand() % 2 == 0);
}

Chapa* ChapaCasco::clonar() const { return new ChapaCasco(*this); }

ChapaArsenal::ChapaArsenal()
    : Chapa(23, "Chapa Arsenal", "+2 Zapatos al inicio de cada ronda", 7, "Epica") {
}

void ChapaArsenal::alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) {
    zapatos += 2;
}

Chapa* ChapaArsenal::clonar() const { return new ChapaArsenal(*this); }

ChapaGeneral::ChapaGeneral()
    : Chapa(24, "Chapa General", "+1 Vida y +1 Zapato al superar cada ronda", 10, "Legendaria") {
}

void ChapaGeneral::alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) {
    if (vidas < maxVidas) {
        vidas += 1;
    }
    zapatos += 1;
}

Chapa* ChapaGeneral::clonar() const { return new ChapaGeneral(*this); }

ChapaMidas::ChapaMidas()
    : Chapa(25, "Chapa Midas", "+1 Moneda extra por cada casilla destapada", 9, "Legendaria") {
}

void ChapaMidas::alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) {
    dinero += 1 + cantCascada;
}

Chapa* ChapaMidas::clonar() const { return new ChapaMidas(*this); }
