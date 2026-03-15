#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "Interfaces.h"
#include <string>

class Estrategia : public PlayerAgent {
public:
    Estrategia(int id, bool isTeamA);
    
    // Ponto de entrada da lógica do candidato
    Action think(const GameState& state) override;

private:
    int id;
    bool teamA;
    std::string role; // "Goleiro", "Ala", "Atacante"
    
    // Você é livre para criar variáveis, instanciar Máquinas de Estado ou Behavior Trees aqui.
};

#endif // ESTRATEGIA_H
