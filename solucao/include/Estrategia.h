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


   // 1. Estados da FSM (Máquina de Estados)
   enum State { IDLE, DEFENDING, ATTACKING, REPOSITIONING };
   State currentState;


   // 2. Alvos Atuais (para evitar recalculados constantes)
   float targetX;
   float targetY;


   // 3. Parâmetros de Ajuste (Facilitam a calibração sem recompilar tudo)
   const float ROBOT_SPEED = 0.5f;    // Velocidade máxima permitida
   const float DISTANCE_THRESHOLD = 0.05f; // Margem de erro (5cm)
   const float BALL_OFFSET = 0.12f;   // Distância para o ponto de chute

   
   // Você é livre para criar variáveis, instanciar Máquinas de Estado ou Behavior Trees aqui.
};


#endif // ESTRATEGIA_H