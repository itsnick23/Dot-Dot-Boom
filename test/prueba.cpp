#include <iostream>
#include <string>
#include "../Partida.h"
using namespace std;

void mostrarTablero(const Partida& p) {
    const Tablero& t = p.getTablero();
    cout << "\n========================================\n";
    cout << "Ronda " << p.getRonda() << "/" << p.getMaxRondas()
         << " | Vidas: " << p.getVidas()
         << " | Monedas: " << p.getDinero()
         << " | Zapatos: " << p.getZapatos()
         << " | Chapas: " << p.getCantChapas() << "/5\n";

    if (p.puedeAbrirTienda()) {
        cout << "[!] Tienda disponible (escribe 't' para abrir)\n";
    } else {
        cout << "Progreso tienda: " << p.getContadorTienda() << "/5\n";
    }

    cout << "    ";
    for (int c = 0; c < t.getColumnas(); c++) {
        if (c < 10) cout << c << "  ";
        else cout << c << " ";
    }
    cout << "\n";

    for (int f = 0; f < t.getFilas(); f++) {
        if (f < 10) cout << " " << f << "  ";
        else cout << f << "  ";

        for (int c = 0; c < t.getColumnas(); c++) {
            const Celda& cel = t.getCelda(f, c);
            if (cel.estaMarcada()) cout << "P  ";
            else if (!cel.estaRevelada()) cout << ".  ";
            else if (cel.esMina()) cout << "*  ";
            else cout << cel.getMinasCerca() << "  ";
        }
        cout << "\n";
    }
}

void menuInventario(Partida& p) {
    const Inventario& inv = p.getInventario();
    cout << "\n--- INVENTARIO (" << inv.getCantidad() << "/" << inv.getCapacidad() << ") ---\n";
    if (inv.getCantidad() == 0) {
        cout << "(Vacio)\n";
        return;
    }
    for (int i = 0; i < inv.getCantidad(); i++) {
        Chapa* ch = inv.getChapa(i);
        cout << i << ") [" << ch->getRareza() << "] " << ch->getNombre()
             << " (Vender: $" << (ch->getCosto() * 0.5f) << ") - " << ch->getDescripcion() << "\n";
    }
    cout << "Escribe el numero para vender o -1 para volver: ";
    int idx;
    if (cin >> idx && idx >= 0) {
        if (p.venderChapa(idx)) cout << "Chapa vendida!\n";
        else cout << "Indice invalido.\n";
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
        if (!(cin >> op)) {
            p.cerrarTienda();
            break;
        }
        if (op == 0) {
            p.cerrarTienda();
        } else if (op == 99) {
            if (p.comprarZapato()) cout << "Zapato comprado!\n";
            else cout << "No se pudo comprar (sin monedas o limite alcanzado).\n";
        } else {
            if (p.comprarChapa(op)) cout << "Chapa comprada!\n";
            else cout << "No se pudo comprar (sin monedas, inventario lleno o no disponible).\n";
        }
    }
}

void mostrarRanking(const Partida& p) {
    RegistroPuntaje lista[20];
    int cant = p.cargarPuntajes("scores.txt", lista, 20);
    cout << "\n--- RANKING DE PUNTAJES ---\n";
    if (cant == 0) {
        cout << "(Aun no hay puntajes guardados)\n";
        return;
    }
    for (int i = 0; i < cant; i++) {
        cout << (i + 1) << ". " << lista[i].nombre << " - Ronda " << lista[i].ronda
             << " - " << lista[i].puntaje << " pts\n";
    }
}

void jugar(Partida& p) {
    char cmd;
    int f, c;

    cout << "\nComandos:\n"
         << "  d <fila> <col> : destapar\n"
         << "  m <fila> <col> : marcar bandera\n"
         << "  z <fila> <col> : tirar zapato\n"
         << "  t : abrir tienda (cuando este 5/5)\n"
         << "  i : ver/vender inventario\n"
         << "  g : guardar partida\n"
         << "  q : salir al menu\n";

    while (!p.estaTerminada()) {
        mostrarTablero(p);

        if (p.rondaSuperada()) {
            cout << "\n[!] Ronda " << p.getRonda() << " completada!\n";
            p.cobrarRonda();
            if (p.partidaGanada()) break;
            continue;
        }

        cout << "> ";
        if (!(cin >> cmd)) break;

        if (cmd == 'q') return;
        else if (cmd == 't') {
            if (p.puedeAbrirTienda()) menuTienda(p);
            else cout << "Aun faltan casillas para abrir la tienda.\n";
        } else if (cmd == 'i') {
            menuInventario(p);
        } else if (cmd == 'g') {
            if (p.guardarPartida("partida.txt")) cout << "Partida guardada en partida.txt\n";
            else cout << "No se pudo guardar.\n";
        } else if (cmd == 'd') {
            cin >> f >> c;
            int vidasAntes = p.getVidas();
            if (!p.jugarCelda(f, c)) {
                cout << "Movimiento invalido.\n";
            } else if (p.getVidas() < vidasAntes) {
                cout << "BOOM! Pisaste una mina. Vidas restantes: " << p.getVidas() << "\n";
            }
        } else if (cmd == 'm') {
            cin >> f >> c;
            p.marcarCelda(f, c);
        } else if (cmd == 'z') {
            cin >> f >> c;
            int r = p.usarZapato(f, c);
            if (r == -1) cout << "No se pudo tirar el zapato.\n";
            else if (r == 0) cout << "Zona limpia! Se revelo solo esa casilla.\n";
            else cout << "Habia mina! Se revelo el 3x3 alrededor sin perder vida.\n";
        }
    }

    if (p.partidaGanada()) {
        cout << "\n=== VICTORIA! Sobreviviste las 10 rondas ===\n";
    } else {
        cout << "\n=== FIN DEL JUEGO! Ronda alcanzada: " << p.getRonda() << " ===\n";
    }

    cout << "Puntaje final: " << p.getPuntajeTotal() << "\n";
    cout << "Ingresa tu nombre para guardar puntaje (sin espacios): ";
    string nombre;
    if (cin >> nombre) {
        p.guardarPuntaje("scores.txt", nombre);
        p.borrarPartida("partida.txt");
    }
    mostrarRanking(p);
}

int main() {
    Partida p("Normal");
    int opcion = 0;

    while (true) {
        cout << "\n=== DOT DOT BOOM (Consola) ===\n";
        cout << "1) Nueva Partida\n";
        cout << "2) Cargar Partida (partida.txt)\n";
        cout << "3) Ver Ranking\n";
        cout << "4) Salir\n";
        cout << "Opcion: ";

        if (!(cin >> opcion)) break;

        if (opcion == 1) {
            cout << "Dificultad (1: Facil, 2: Normal, 3: Dificil): ";
            int d = 2;
            cin >> d;
            string dif = "Normal";
            if (d == 1) dif = "Facil";
            else if (d == 3) dif = "Dificil";
            p.nuevaPartida(dif);
            jugar(p);
        } else if (opcion == 2) {
            if (p.cargarPartida("partida.txt")) {
                cout << "Partida cargada!\n";
                jugar(p);
            } else {
                cout << "No hay partida guardada.\n";
            }
        } else if (opcion == 3) {
            mostrarRanking(p);
        } else if (opcion == 4) {
            break;
        }
    }

    return 0;
}
