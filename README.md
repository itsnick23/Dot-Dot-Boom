# Dot-Dot-Boom

Juego hecho en C++ con SFML que mezcla buscaminas con mecanicas tipo Balatro.

## Como jugar
Ejecuta `DotDotBoom.exe`. No necesita instalar nada ni archivos extra.

### Reglas
- Empiezas con 3 vidas y 1 zapato en un tablero 5x5 con 7 minas.
- En cada ronda el tablero crece +2 en tamaño hasta llegar a la ronda 10 para ganar.
- Cada casilla con numero da 0.5 monedas (las casillas en 0 no dan monedas).
- Cada 5 turnos (clics) se habilita la tienda para comprar chapas o zapatos.
- Puedes tener hasta 5 chapas en el inventario al mismo tiempo.

### Controles
- Clic izquierdo: destapar casilla
- Clic derecho: poner o quitar bandera
- Z: activar modo zapato (si no hay mina revela esa casilla; si hay mina revela el 3x3 sin perder vida)
- T: abrir tienda (cuando este disponible)
- ESC: menu / pausa

## Archivos principales
- `main.cpp`: interfaz grafica, menu, opciones, musica y controles
- `Partida.h / .cpp`: logica de rondas, turnos, tienda y guardado en `partida.txt`
- `Tablero.h / .cpp` y `Celda.h / .cpp`: generacion del campo de minas y cascada
- `Chapa.h / .cpp`: chapas de identificacion (herencia y polimorfismo)
- `Inventario.h / .cpp`: inventario fijo de 5 chapas
