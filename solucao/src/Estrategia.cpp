#include "Estrategia.h"
#include <cmath>

Estrategia::Estrategia(int id, bool isTeamA) : id(id), teamA(isTeamA) {
    if (id == 0) role = "Goleiro";
    else if (id == 1) role = "Ala";
    else role = "Atacante";
}

Action Estrategia::think(const GameState& state) {
    Action a;
    a.moveDirectionX = 0;
    a.moveDirectionY = 0;

    // TODO: Implementar lógica de tomada de decisão e planejamento aqui!
    // Você tem acesso a:
    // - state.ball (bola)
    // - state.getMe() (seu robô)
    // - state.teammates (aliados)
    // - state.opponents (inimigos)
    // - state.myIndex (seu ID: 0, 1 ou 2)
    
    return a;
}
