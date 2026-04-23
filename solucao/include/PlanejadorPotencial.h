// PlanejadorPotencial.h
#ifndef PLANEJADOR_POTENCIAL_H
#define PLANEJADOR_POTENCIAL_H

#include "Planejador.h"

class PlanejadorPotencial : public Planejador {
public:
    void planejar(const GameState& state,
                  const EntityState& eu,
                  float alvoX,
                  float alvoY,
                  float& vecX,
                  float& vecY) override;

private:
    // Constantes do campo potencial
    static constexpr float REPULSION_RADIUS = 0.20f;
    static constexpr float REPULSION_STRENGTH = 0.6f;
    static constexpr float TANGENTIAL_STRENGTH = 0.5f;
    static constexpr float WALL_ESCAPE_STRENGTH = 0.8f;
    static constexpr float STUCK_DISTANCE = 0.15f;
    static constexpr float FIELD_X_LIMIT = 0.75f;
    static constexpr float FIELD_Y_LIMIT = 0.55f;

    void aplicarAtracaoAlvo(const EntityState& eu, float alvoX, float alvoY,
                            float& vecX, float& vecY);
    void aplicarRepulsaoObstaculos(const GameState& state, const EntityState& eu,
                                   float& vecX, float& vecY);
    void aplicarForcaTangencial(const EntityState& eu, const EntityState& obs,
                                float d, float& vecX, float& vecY);
    void aplicarEscapeParede(const GameState& state, const EntityState& eu,
                             float& vecX, float& vecY);
    void aplicarLogicaEscape(const GameState& state, const EntityState& eu,
                             float& vecX, float& vecY);
    void aplicarEscapeTangencial(const EntityState& eu, const EntityState& oponente,
                                 float& vecX, float& vecY);
};

#endif