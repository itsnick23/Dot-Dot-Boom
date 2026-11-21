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
    virtual bool alPisarMina(int& vidas, int& zapatos, int& dinero);
    virtual void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos);
    virtual void alAbrirTienda(int& zapatos, int& dinero);
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

class ChapaExplorador : public Chapa {
public:
    ChapaExplorador();
    void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) override;
    Chapa* clonar() const override;
};

class ChapaChatarrero : public Chapa {
public:
    ChapaChatarrero();
    void alAbrirTienda(int& zapatos, int& dinero) override;
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

class ChapaBotin : public Chapa {
public:
    ChapaBotin();
    void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) override;
    Chapa* clonar() const override;
};

class ChapaAhorrista : public Chapa {
public:
    ChapaAhorrista();
    void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) override;
    Chapa* clonar() const override;
};

class ChapaBlindaje : public Chapa {
private:
    bool escudoActivo;

public:
    ChapaBlindaje();
    bool alPisarMina(int& vidas, int& zapatos, int& dinero) override;
    void reiniciarRonda() override;
    Chapa* clonar() const override;
};

class ChapaParacaidas : public Chapa {
public:
    ChapaParacaidas();
    bool alPisarMina(int& vidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaCuartel : public Chapa {
public:
    ChapaCuartel();
    void alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaBlindajePesado : public Chapa {
private:
    int cargas;

public:
    ChapaBlindajePesado();
    bool alPisarMina(int& vidas, int& zapatos, int& dinero) override;
    void reiniciarRonda() override;
    Chapa* clonar() const override;
};

class ChapaFenix : public Chapa {
private:
    bool usada;

public:
    ChapaFenix();
    bool alPisarMina(int& vidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaSuministros : public Chapa {
public:
    ChapaSuministros();
    void alAbrirTienda(int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaVigilante : public Chapa {
public:
    ChapaVigilante();
    void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) override;
    Chapa* clonar() const override;
};

class ChapaRacion : public Chapa {
public:
    ChapaRacion();
    void alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaMochila : public Chapa {
public:
    ChapaMochila();
    void alAbrirTienda(int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaFrancotirador : public Chapa {
public:
    ChapaFrancotirador();
    void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) override;
    Chapa* clonar() const override;
};

class ChapaEnfermero : public Chapa {
public:
    ChapaEnfermero();
    void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) override;
    Chapa* clonar() const override;
};

class ChapaBrigada : public Chapa {
public:
    ChapaBrigada();
    void alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaCasco : public Chapa {
public:
    ChapaCasco();
    bool alPisarMina(int& vidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaArsenal : public Chapa {
public:
    ChapaArsenal();
    void alIniciarRonda(int& vidas, int maxVidas, int& zapatos, int& dinero) override;
    Chapa* clonar() const override;
};

class ChapaGeneral : public Chapa {
public:
    ChapaGeneral();
    void alSuperarRonda(int& vidas, int maxVidas, int& dinero, int& zapatos) override;
    Chapa* clonar() const override;
};

class ChapaMidas : public Chapa {
public:
    ChapaMidas();
    void alDestapar(int& dinero, int& zapatos, int minasCerca, int cantCascada) override;
    Chapa* clonar() const override;
};
