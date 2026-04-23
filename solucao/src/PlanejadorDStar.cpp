// PlanejadorDStar.cpp
#include "PlanejadorDStar.h"

void PlanejadorDStar::planejar(const GameState& state,
                               const EntityState& eu,
                               float alvoX,
                               float alvoY,
                               float& vecX,
                               float& vecY) {
    // Implementação futura: D* Lite
    // Por enquanto, comportamento simples: vetor direto ao alvo
    vecX = alvoX - eu.x;
    vecY = alvoY - eu.y;
}