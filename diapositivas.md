# DIAPOSITIVAS DE SUSTENTACIÓN — DOT-DOT-BOOM
**Proyecto Final de Programación Orientada a Objetos (Semestre 2026-II)**  
*Escuela Profesional de Ciencia de la Computación*

---

## DIAPOSITIVA 1: Portada
* **Título:** DOT-DOT-BOOM
* **Subtítulo:** Videojuego Rogue-Minesweeper Táctico en C++20 con SFML
* **Curso:** Programación Orientada a Objetos — Semestre 2026-II
* **Escuela:** Escuela Profesional de Ciencia de la Computación
* **Integrantes:**
  * [Nombre Integrante 1] — [Código]
  * [Nombre Integrante 2] — [Código]
  * [Nombre Integrante 3] — [Código]

---

## DIAPOSITIVA 2: Problema y Objetivos
* **El Problema del Buscaminas Tradicional:**
  * Determinismo rígido: un solo clic en una mina destruye la partida completa.
  * Adivinanzas ciegas: situaciones 50/50 donde la deducción lógica matemática es insuficiente.
  * Falta de rejugabilidad estratégica y acumulación de recursos.
* **Nuestra Propuesta:**
  * Dinámica Roguelike: Vidas progresivas, economía de monedas, tienda táctica y reliquias.
  * Garantía Matemática (BFS): Algoritmo de conectividad que garantiza tableros 100% resolubles.
  * Dominio Riguroso de POO: Desacoplar completamente la lógica del juego de la interfaz gráfica.

---

## DIAPOSITIVA 3: Funcionalidades del Videojuego
* **10 Rondas de Combate:** Tableros auto-escalables desde 5×5 (7 minas) hasta 23×23 (25 minas).
* **El Zapato Táctico:** Herramienta para inspeccionar celdas a distancia sin perder vidas (desactiva bombas en 3×3 o revela zonas limpias).
* **25 Chapas (Reliquias Polimórficas):** 4 rarezas (*Común, Rara, Épica, Legendaria*) que modifican las reglas del juego en runtime.
* **Tienda Táctica:** Permite comprar suministros y vender chapas equipadas para recuperar monedas.
* **Persistencia Doble:** Guardado completo de partidas (`partida.txt`) y Salón de la Fama (`scores.txt`).
* **Doble Interfaz:** Modo gráfico de alto rendimiento en SFML 3 y simulador en consola (`test/prueba.cpp`).

---

## DIAPOSITIVA 4: Arquitectura del Software
* **Separación de Responsabilidades:**
  * **Capa de Dominio (Motor Puro C++20):**
    * `Celda`: Representación atómica de estado.
    * `Tablero`: Matriz dinámica 2D, cálculo de adyacencias y algoritmo de grafos.
    * `Chapa`: Jerarquía abstracta de eventos de juego.
    * `Inventario`: Colección limitada a 5 ranuras.
    * `Partida`: Controlador central, economía, rondas y persistencia.
  * **Capa de Presentación:**
    * `main.cpp`: Menús, renderizado 3D de celdas, HUD interactivo, screen shake.
    * `test/prueba.cpp`: Interfaz de terminal para verificar la lógica de forma agnóstica.

---

## DIAPOSITIVA 5: Diagrama de Clases UML
* **Relaciones Estructurales:**
  * `Tablero` *-- `Celda` : Composición fuerte (1 a muchas).
  * `Partida` *-- `Tablero` : Composición (1 a 1).
  * `Partida` *-- `Inventario` : Composición (1 a 1).
  * `Partida` o-- `Chapa` : Agregación (catálogo y tienda).
  * `Inventario` o-- `Chapa` : Agregación (0 a 5 chapas equipadas).
  * `Chapa` <|-- `ChapaZapador`, `ChapaBlindaje`, etc. : Herencia pura (25 derivadas).
  * `Partida` ..> `RegistroPuntaje` : Asociación y uso para persistencia.

---

## DIAPOSITIVA 6: Composición, Agregación y Asociación
* **Composición Fuerte:**
  * En `Partida.h`: `Tablero tablero;` e `Inventario inventario;` son miembros por valor. Su ciclo de vida está ligado al de la partida.
* **Agregación:**
  * En `Inventario.h`: `Chapa* chapas[5];`. El inventario almacena punteros a objetos que existen independientemente en el catálogo.
* **Asociación:**
  * En `Partida.h`: `int cargarPuntajes(..., RegistroPuntaje* lista);`. La partida interactúa con estructuras de puntuación sin ser su dueña exclusiva.

---

## DIAPOSITIVA 7: Herencia y Polimorfismo Dinámico
* **Clase Base Abstracta Pura (`Chapa`):**
  * Define `virtual ~Chapa();` y el método virtual puro `virtual Chapa* clonar() const = 0;` (Patrón Prototype).
* **25 Clases Derivadas:**
  * Implementan comportamientos específicos sobrescribiendo métodos virtuales: `alIniciarRonda`, `alDestapar`, `alPisarMina`, `alSuperarRonda`.
* **Despacho Dinámico (Dynamic Dispatch):**
  * Invocación polimórfica en tiempo de ejecución:  
    `inventario.getChapa(i)->alDestapar(dinero, zapatos, minasCerca, cascada);`
  * El motor no conoce la clase concreta, garantizando modularidad y extensibilidad abierta/cerrada (Principio OCP).

---

## DIAPOSITIVA 8: Encapsulamiento y Gestión de Recursos
* **Encapsulamiento con Validación:**
  * Miembros estrictamente `private:` y `protected:`.
  * Setters con validación de invariantes: `Celda::setMinasCerca` asegura rangos entre 0 y 8; `Tablero::configurar` valida límites de tamaño y densidad de minas.
* **Gestión de Memoria Dinámica Justificada:**
  * Matriz 2D dinámica en `Tablero`: justificada porque el tamaño del tablero crece de ronda en ronda (de 5×5 a 23×23).
  * Liberación estricta de memoria en los destructores `Tablero::liberar()`, `Inventario::limpiar()` y `Partida::~Partida()`.
  * Cero fugas de memoria (Memory Leaks).

---

## DIAPOSITIVA 9: Sobrecarga de Métodos y Operadores
* **Sobrecarga de Métodos:**
  * `Tablero::destapar(int fila, int col)` vs `Tablero::destapar(Coord pos)`.
  * `Partida::buscarChapa(int id)` vs `Partida::buscarChapa(const string& nombre)`.
* **Sobrecarga de Operadores:**
  * `RegistroPuntaje::operator<`: Ordena registros de mayor a menor (`puntaje > otro.puntaje`).
  * `RegistroPuntaje::operator==`: Compara si dos registros son idénticos.
  * `Chapa::operator==`: Compara igualdad de reliquias por su ID único.

---

## DIAPOSITIVA 10: Algoritmos y Colecciones
* **Algoritmo de Grafos / Conectividad (BFS):**
  * `Tablero::tableroConexo`: Búsqueda en anchura con cola FIFO que garantiza que todas las celdas seguras forman un único componente conexo transitable.
* **Algoritmo de Ordenamiento:**
  * `Partida::cargarPuntajes`: Ordenamiento por intercambio para el Salón de la Fama utilizando la sobrecarga de `operator<`.
* **Algoritmo de Inundación (Flood-Fill):**
  * `Tablero::expandirCascada`: Despeje recursivo de celdas cuando se descubre una celda con 0 minas vecinas.

---

## DIAPOSITIVA 11: Persistencia de Información en Archivos
* **Guardado y Carga de Partidas (`partida.txt`):**
  * Serialización de ronda, vidas, monedas, zapatos, puntaje y IDs de chapas equipadas.
  * Permite cerrar el juego y reanudar la partida en cualquier momento.
* **Registro de Récords (`scores.txt`):**
  * Modo de adición (`ios::app`) con nombre, ronda alcanzada y puntuación total.
  * Carga y parseo seguro con `istringstream`.

---

## DIAPOSITIVA 12: Demostración en Vivo y Conclusiones
* **Puntos a Demostrar en Vivo:**
  1. Apertura de celdas y visualización de minas/números con colores.
  2. Uso del zapato táctico para neutralizar una mina.
  3. Compra y venta de reliquias en la Tienda Táctica.
  4. Guardado y carga de partida desde el Menú Principal.
* **Conclusiones:**
  * Separación exitosa entre la lógica del dominio y la interfaz gráfica.
  * Aplicación práctica de polimorfismo para construir mecánicas complejas y escalables.
  * Integración de teoría de grafos para resolver un problema clásico de diseño en el Buscaminas.
