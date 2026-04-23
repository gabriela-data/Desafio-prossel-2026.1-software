// PlanejadorDStar.h
#ifndef PLANEJADOR_DSTAR_H
#define PLANEJADOR_DSTAR_H

#include "Planejador.h"

class PlanejadorDStar : public Planejador {
public:
    void planejar(const GameState& state,
                  const EntityState& eu,
                  float alvoX,
                  float alvoY,
                  float& vecX,
                  float& vecY) override;
};

#endif