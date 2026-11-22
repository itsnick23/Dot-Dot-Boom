#include <iostream>
#include "../Partida.h"
using namespace std;

void mostrarTablero(const Partida& p) {
    const Tablero& t = p.getTablero();
    cout << "\nRonda " << p.getRonda() << "/" << p.getMaxRondas()
         << " | Vidas: " << p.getVidas()
         << " | Monedas: " << p.getDinero()
         << " | Zapatos: " << p.getZapatos()
         << " | Chapas: " << p.getCantChapas() << "/5\n";

    if (p.puedeAbrirTienda()) {
        cout << "[!] Tienda disponible (escribe 't' para abrir)\n";
    } else {
        cout << "Progreso tienda: " << p.getContadorTienda() << "/5\n";
    }

    cout << "   ";
    for (int c = 0; c < t.getColumnas(); c++) cout << c << " ";
    cout << "\n";

    for (int f = 0; f < t.getFilas(); f++) {
        cout << f << "  ";
        for (int c = 0; c < t.getColumnas(); c++) {
            const Celda& cel = t.getCelda(f, c);
            if (cel.estaMarcada()) cout << "P ";
            else if (!cel.estaRevelada()) cout << ". ";
            else if (cel.esMina()) cout << "* ";
            else cout << cel.getMinasCerca() << " ";
        }
        cout << "\n";
    }
}

void menuTienda(Partida& p) {
    p.abrirTienda();
    while (p.estaEnTienda()) {
        cout << "\n--- TIENDA (Monedas: " << p.getDinero() << ") ---\n";
        for (int i = 0; i < p.getCantTienda(); i++) {
            Chapa* c = p.getChapaTienda(i);
            if (c == nullptr) continue;
            cout << c->getId() << ") [" << c->getRareza() << "] " << c->getNombre()
                 << " ($" << c->getCosto() << ") - " << c->getDescripcion() << "\n";
        }
        cout << "99) Comprar Zapato ($" << p.getCostoZapato() << ") [Quedan: " << p.getZapatosRestantesTienda() << "/5]\n";
        cout << "0) Salir de la tienda\nOpcion: ";
        int op;
        cin >> op;
        if (op == 0) {
            p.cerrarTienda();
        } else if (op == 99) {
            if (p.comprarZapato()) cout << "Zapato comprado!\n";
            else cout << "No se pudo comprar (sin monedas o limite alcanzado).\n";
        } else {
            if (p.comprarChapa(op)) cout << "Chapa comprada!\n";
            else cout << "No se pudo comprar.\n";
        }
    }
}

int main() {
    Partida p("Normal");
    char cmd;
    int f, c;

    cout << "Comandos: d <f> <c> (destapar) | m <f> <c> (marcar) | z <f> <c> (zapato) | t (tienda) | i (inventario) | g (guardar) | c (cargar) | q (salir)\n";

    while (!p.estaTerminada()) {
        mostrarTablero(p);

        if (p.rondaSuperada()) {
            cout << "Ronda superada! Pasando a la siguiente...\n";
            p.cobrarRonda();
            if (p.partidaGanada()) break;
            continue;
        }

        cout << "> ";
        if (!(cin >> cmd)) break;

        if (cmd == 'q') break;
        else if (cmd == 't') {
            if (p.puedeAbrirTienda()) menuTienda(p);
            else cout << "Aun faltan casillas para abrir la tienda.\n";
        } else if (cmd == 'd') {
            cin >> f >> c;
            if (!p.jugarCelda(f, c)) cout << "Movimiento invalido.\n";
        } else if (cmd == 'm') {
            cin >> f >> c;
            p.marcarCelda(f, c);
        } else if (cmd == 'z') {
            cin >> f >> c;
            int r = p.usarZapato(f, c);
            if (r == -1) cout << "No se pudo tirar el zapato.\n";
            else if (r == 0) cout << "Zona limpia! Se revelo solo esa casilla.\n";
            else cout << "Habia mina! Se revelo el 3x3 alrededor.\n";
        } else if (cmd == 'i') {
            const Inventario& inv = p.getInventario();
            cout << "\n--- INVENTARIO (" << inv.getCantidad() << "/" << inv.getCapacidad() << ") ---\n";
            for (int i = 0; i < inv.getCantidad(); i++) {
                Chapa* ch = inv.getChapa(i);
                cout << i << ") [" << ch->getRareza() << "] " << ch->getNombre() << " - " << ch->getDescripcion() << "\n";
            }
        } else if (cmd == 'g') {
            if (p.guardarPartida("partida.txt")) cout << "Partida guardada en partida.txt\n";
            else cout << "No se pudo guardar.\n";
        } else if (cmd == 'c') {
            if (p.cargarPartida("partida.txt")) cout << "Partida cargada desde partida.txt\n";
            else cout << "No hay partida guardada.\n";
        }
    }

    if (p.partidaGanada()) cout << "\nGANASTE LA PARTIDA!\n";
    else cout << "\nFIN DEL JUEGO. Ronda alcanzada: " << p.getRonda() << "\n";

    return 0;
}
