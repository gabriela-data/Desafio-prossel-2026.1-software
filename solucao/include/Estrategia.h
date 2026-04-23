#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "Interfaces.h"
#include "Planejador.h"
#include <memory>
#include <string>

class Estrategia : public PlayerAgent {
public:
    // Construtor de compatibilidade (usa planejador padrão: campo potencial)
    Estrategia(int id, bool isTeamA);

    // Construtor com injeção de dependência do planejador
    Estrategia(int id, bool isTeamA, std::unique_ptr<Planejador> planejador);

    Action think(const GameState& state) override;

private:
    int id;
    bool teamA;
    std::string role;
    std::unique_ptr<Planejador> planejador;

    static constexpr float DISTANCE_THRESHOLD = 0.05f;

    void decidirAlvo(const GameState& state, float& alvoX, float& alvoY, bool& modoAtaqueTotal);
    void calcularAlvoGoleiro(const GameState& state, float& alvoX, float& alvoY);
    void calcularAlvoJogadorLinha(const GameState& state, float& alvoX, float& alvoY, bool& modoAtaqueTotal);
    float calcularLado() const;
};

#endif