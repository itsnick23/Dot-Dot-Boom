#pragma once

class Celda {
private:
    bool tieneMina;
    bool revelada;
    bool marcada;
    int minasCerca;

public:
    Celda();

    bool esMina() const;
    bool estaRevelada() const;
    bool estaMarcada() const;
    int getMinasCerca() const;

    void colocarMina();
    void setMinasCerca(int cantidad);
    bool revelar();
    void marcar();
    void alternarMarca();
    void reiniciar();
};
