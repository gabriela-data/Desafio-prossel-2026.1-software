//PlanejadorPotencial.cpp
#include "PlanejadorPotencial.h"
#include <cmath>

void PlanejadorPotencial::planejar(const GameState& state,
                                   const EntityState& eu,
                                   float alvoX,
                                   float alvoY,
                                   float& vecX,
                                   float& vecY) {
    aplicarAtracaoAlvo(eu, alvoX, alvoY, vecX, vecY);
    aplicarRepulsaoObstaculos(state, eu, vecX, vecY);
    aplicarEscapeParede(state, eu, vecX, vecY);
    aplicarLogicaEscape(state, eu, vecX, vecY);
}

void PlanejadorPotencial::aplicarAtracaoAlvo(const EntityState& eu, float alvoX, float alvoY,
                                             float& vecX, float& vecY) {
    vecX = alvoX - eu.x;
    vecY = alvoY - eu.y;
}

void PlanejadorPotencial::aplicarRepulsaoObstaculos(const GameState& state, const EntityState& eu,
                                                    float& vecX, float& vecY) {
    // Coleta obstáculos: oponentes + companheiros
    std::vector<EntityState> obstaculos = state.opponents;
    for (const auto& t : state.teammates) {
        obstaculos.push_back(t);
    }

    for (const auto& obs : obstaculos) {
        float d = eu.distTo(obs.x, obs.y);
        if (d < REPULSION_RADIUS && d > 0.01f) {
            float forca = REPULSION_STRENGTH * (REPULSION_RADIUS - d) / d;
            float repX = (eu.x - obs.x) * forca;
            float repY = (eu.y - obs.y) * forca;

            aplicarForcaTangencial(eu, obs, d, vecX, vecY);

            vecX += repX;
            vecY += repY;
        }
    }
}

void PlanejadorPotencial::aplicarForcaTangencial(const EntityState& eu, const EntityState& obs,
                                                 float d, float& vecX, float& vecY) {
    float forca = REPULSION_STRENGTH * (REPULSION_RADIUS - d) / d;
    float repX = (eu.x - obs.x) * forca;
    float repY = (eu.y - obs.y) * forca;
    float tanX = -repY;
    float tanY = repX;
    vecX += tanX * TANGENTIAL_STRENGTH;
    vecY += tanY * TANGENTIAL_STRENGTH;
}

void PlanejadorPotencial::aplicarEscapeParede(const GameState& state, const EntityState& eu,
                                              float& vecX, float& vecY) {
    if (std::abs(eu.x) > FIELD_X_LIMIT || std::abs(eu.y) > FIELD_Y_LIMIT) {
        for (const auto& op : state.opponents) {
            if (eu.distTo(op.x, op.y) < STUCK_DISTANCE) {
                vecX += (eu.x - op.x) * WALL_ESCAPE_STRENGTH;
                vecY += (eu.y > 0 ? -0.5f : 0.5f);
            }
        }
    }
}

void PlanejadorPotencial::aplicarLogicaEscape(const GameState& state, const EntityState& eu,
                                              float& vecX, float& vecY) {
    for (const auto& op : state.opponents) {
        if (eu.distTo(op.x, op.y) < STUCK_DISTANCE) {
            if (std::abs(eu.x) > FIELD_X_LIMIT || std::abs(eu.y) > FIELD_Y_LIMIT) {
                float tanX = -(op.y - eu.y);
                float tanY = (op.x - eu.x);
                vecX += tanX * WALL_ESCAPE_STRENGTH;
                vecY += tanY * WALL_ESCAPE_STRENGTH;
            }
        }
    }
}