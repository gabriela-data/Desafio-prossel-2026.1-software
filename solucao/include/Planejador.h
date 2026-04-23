// Planejador.h
#ifndef PLANEJADOR_H
#define PLANEJADOR_H

#include "Interfaces.h"   // Contém GameState, EntityState, etc.

class Planejador {
public:
    virtual ~Planejador() = default;

    // Calcula um vetor de movimento (vecX, vecY) a partir da posição atual (eu)
    // e do alvo desejado (alvoX, alvoY). O vetor deve apontar na direção desejada
    // e pode incluir forças de repulsão, escape, etc.
    virtual void planejar(const GameState& state,
                          const EntityState& eu,
                          float alvoX,
                          float alvoY,
                          float& vecX,
                          float& vecY) = 0;
};

#endif