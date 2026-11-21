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

    virtual void alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero);
    virtual void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada);
    virtual bool alPisarMina(int& vidas);
    virtual void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos);
    virtual void alAbrirTienda(int& zapatos);
    virtual void reiniciarRonda();
    virtual Chapa* clonar() const = 0;

    int getId() const;
    string getNombre() const;
    string getDescripcion() const;
    int getCosto() const;
    string getRareza() const;

    bool operator==(const Chapa& otra) const;
};

class ChapaZapador : public Chapa {
public:
    ChapaZapador();
    void alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaSaqueador : public Chapa {
public:
    ChapaSaqueador();
    void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) override;
    Chapa* clonar() const override;
};

class ChapaEco : public Chapa {
public:
    ChapaEco();
    void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) override;
    Chapa* clonar() const override;
};

class ChapaMedico : public Chapa {
public:
    ChapaMedico();
    void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) override;
    Chapa* clonar() const override;
};

class ChapaTactica : public Chapa {
public:
    ChapaTactica();
    void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) override;
    Chapa* clonar() const override;
};

class ChapaBlindaje : public Chapa {
private:
    bool escudoActivo;

public:
    ChapaBlindaje();
    bool alPisarMina(int& vidas) override;
    void reiniciarRonda() override;
    Chapa* clonar() const override;
};

class ChapaBlindajePesado : public Chapa {
private:
    int cargas;

public:
    ChapaBlindajePesado();
    bool alPisarMina(int& vidas) override;
    void reiniciarRonda() override;
    Chapa* clonar() const override;
};

class ChapaFenix : public Chapa {
private:
    bool usada;

public:
    ChapaFenix();
    bool alPisarMina(int& vidas) override;
    Chapa* clonar() const override;
};

class ChapaSuministros : public Chapa {
public:
    ChapaSuministros();
    void alAbrirTienda(int& zapatos) override;
    Chapa* clonar() const override;
};
