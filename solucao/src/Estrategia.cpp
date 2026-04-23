//Estrategia.cpp
#include "Estrategia.h"
#include "PlanejadorPotencial.h"   // para o planejador padrão
#include <cmath>
#include <algorithm>

// Construtor padrão: usa campo potencial
Estrategia::Estrategia(int id, bool isTeamA)
    : Estrategia(id, isTeamA, std::make_unique<PlanejadorPotencial>()) {
}

// Construtor com injeção de planejador
Estrategia::Estrategia(int id, bool isTeamA, std::unique_ptr<Planejador> planejador)
    : id(id), teamA(isTeamA), planejador(std::move(planejador)) {
    if (id == 0)
        role = "Goleiro";
    else if (id == 1)
        role = "Ala";
    else
        role = "Atacante";
}

Action Estrategia::think(const GameState& state) {
    Action a;
    a.moveDirectionX = 0;
    a.moveDirectionY = 0;

    const EntityState& eu = state.getMe();
    float alvoX, alvoY;
    bool modoAtaqueTotal = false;

    decidirAlvo(state, alvoX, alvoY, modoAtaqueTotal);

    float vecX = 0.0f, vecY = 0.0f;
    planejador->planejar(state, eu, alvoX, alvoY, vecX, vecY);

    float angulo = std::atan2(vecY, vecX);
    a.moveDirectionX = std::cos(angulo);
    a.moveDirectionY = std::sin(angulo);

    if (!modoAtaqueTotal && eu.distTo(alvoX, alvoY) < DISTANCE_THRESHOLD) {
        a.moveDirectionX = 0;
        a.moveDirectionY = 0;
    }

    return a;
}

void Estrategia::decidirAlvo(const GameState& state, float& alvoX, float& alvoY, bool& modoAtaqueTotal) {
    if (id == 0) {
        calcularAlvoGoleiro(state, alvoX, alvoY);
    } else {
        calcularAlvoJogadorLinha(state, alvoX, alvoY, modoAtaqueTotal);
    }
}

// ---------- Goleiro ----------
void Estrategia::calcularAlvoGoleiro(const GameState& state, float& alvoX, float& alvoY) {
    const EntityState& bola = state.ball;
    float lado = calcularLado();

    const float PENALTY_AREA_X = 0.60f;
    const float PENALTY_AREA_Y = 0.30f;
    const float GOALKEEPER_DEF_X_OFFSET = 0.80f;
    const float GOALKEEPER_DEF_Y_LIMIT = 0.20f;
    const float GOALKEEPER_BALL_OFFSET = 0.12f;

    bool bolaNaArea = (bola.x * lado < -PENALTY_AREA_X) && (std::abs(bola.y) < PENALTY_AREA_Y);

    if (bolaNaArea) {
        float offsetX = -GOALKEEPER_BALL_OFFSET * lado;
        alvoX = bola.x + offsetX;
        alvoY = bola.y;
    } else {
        alvoX = -GOALKEEPER_DEF_X_OFFSET * lado;
        alvoY = std::max(-GOALKEEPER_DEF_Y_LIMIT, std::min(GOALKEEPER_DEF_Y_LIMIT, bola.y));
    }
}

// ---------- Jogadores de linha ----------
void Estrategia::calcularAlvoJogadorLinha(const GameState& state, float& alvoX, float& alvoY, bool& modoAtaqueTotal) {
    const EntityState& eu = state.getMe();
    const EntityState& bola = state.ball;
    float lado = calcularLado();

    int idxAmigo = (id == 1) ? 1 : 0;
    const EntityState& amigo = state.teammates[idxAmigo];

    const float DIST_LIMIT = 0.02f;
    const float ALIGN_OFF = 0.05f;
    const float PREP_OFF = 0.15f;
    const float SUP_X_OFF = 0.30f;
    const float SUP_Y_FACT = 0.5f;

    float minhaDist = eu.distTo(bola.x, bola.y);
    float amigoDist = amigo.distTo(bola.x, bola.y);

    bool euAtaco;
    if (std::abs(minhaDist - amigoDist) < DIST_LIMIT) {
        euAtaco = (id == 2);
    } else {
        euAtaco = (minhaDist < amigoDist);
    }

    if (euAtaco) {
        bool alinhado = (lado > 0) ? (eu.x < bola.x - ALIGN_OFF) : (eu.x > bola.x + ALIGN_OFF);
        if (alinhado) {
            alvoX = bola.x;
            alvoY = bola.y;
            modoAtaqueTotal = true;
        } else {
            alvoX = bola.x - (PREP_OFF * lado);
            alvoY = bola.y;
        }
    } else {
        alvoX = -SUP_X_OFF * lado;
        alvoY = bola.y * SUP_Y_FACT;
    }
}

float Estrategia::calcularLado() const {
    return teamA ? 1.0f : -1.0f;
}