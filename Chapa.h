#pragma once
#include <string>
using namespace std;

class Chapa {
protected:
    int id;
    string nombre;
    string descripcion;
    int costo;
    string rareza;

public:
    Chapa(int id, const string& nombre, const string& descripcion, int costo, const string& rareza);
    virtual ~Chapa();

    virtual void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) = 0;
    virtual bool alPisarMina(int& vidas);
    virtual void reiniciarRonda();
    virtual Chapa* clonar() const = 0;

    int getId() const;
    string getNombre() const;
    string getDescripcion() const;
    int getCosto() const;
    string getRareza() const;

    bool operator==(const Chapa& otra) const;
};

class ChapaArtilleria : public Chapa {
private:
    float multExtra;

public:
    ChapaArtilleria();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    Chapa* clonar() const override;
};

class ChapaBlindaje : public Chapa {
private:
    bool escudoActivo;

public:
    ChapaBlindaje();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    bool alPisarMina(int& vidas) override;
    void reiniciarRonda() override;
    Chapa* clonar() const override;
};

class ChapaReconocimiento : public Chapa {
private:
    int fichasExtra;

public:
    ChapaReconocimiento();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    Chapa* clonar() const override;
};

class ChapaFrancotirador : public Chapa {
private:
    int fichasExtra;

public:
    ChapaFrancotirador();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    Chapa* clonar() const override;
};

class ChapaMortero : public Chapa {
private:
    float multExtra;

public:
    ChapaMortero();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    Chapa* clonar() const override;
};

class ChapaVeterano : public Chapa {
private:
    int fichasFijas;

public:
    ChapaVeterano();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    Chapa* clonar() const override;
};

class ChapaComandante : public Chapa {
private:
    float factorMult;

public:
    ChapaComandante();
    void alDestapar(int& fichas, float& mult, int minasCerca, bool esCascada) override;
    Chapa* clonar() const override;
};
