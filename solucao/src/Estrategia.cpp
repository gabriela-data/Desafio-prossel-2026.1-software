#include "Estrategia.h"
#include <cmath>


Estrategia::Estrategia(int id, bool isTeamA) : id(id), teamA(isTeamA)
{
   if (id == 0)
       role = "Goleiro";
   else if (id == 1)
       role = "Ala";
   else
       role = "Atacante";
}


Action Estrategia::think(const GameState &state)
{
   Action a;
   a.moveDirectionX = 0;
   a.moveDirectionY = 0;


   const EntityState &eu = state.getMe();
   const EntityState &bola = state.ball;
   float lado = (this->teamA) ? 1.0f : -1.0f;


   float alvoX, alvoY;
   bool modoAtaqueTotal = false;


   // --- CAMADA 1: TOMADOR DE DECISÃO (FSM) ---
   if (state.myIndex == 0) // Goleiro
   {
       // Posição defensiva padrão: 20 cm à frente da linha do gol
       float defX = -0.80f * lado;
       float defY = std::max(-0.20f, std::min(0.20f, bola.y));


       // Definição da área penal (ajuste os valores conforme o campo simulado)
       // lado > 0: time A ataca para a direita (x positivo); goleiro A defende x negativo
       // lado < 0: time B ataca para a esquerda (x negativo); goleiro B defende x positivo
       bool bolaNaArea = (bola.x * lado < -0.60f) && (std::abs(bola.y) < 0.30f);


       if (bolaNaArea)
       {
           // Bola dentro da área: posicionar-se atrás dela para afastá-la
           // Offset de 12 cm na direção do próprio fundo de campo (longe do gol adversário)
           float offsetX = -0.12f * lado;
           alvoX = bola.x + offsetX;
           alvoY = bola.y;


           // O campo potencial (atração + repulsão) guiará o goleiro até o alvo.
           // Ao chegar lá, o contato com a bola naturalmente a empurrará para frente.
       }
       else
       {
           // Bola fora da área: mantém a marcação normal
           alvoX = defX;
           alvoY = defY;
       }
   }
   else
   {
       // COORDENAÇÃO: Identificar o parceiro de linha
       int idxAmigo = (state.myIndex == 1) ? 1 : 0;
       const EntityState &amigo = state.teammates[idxAmigo];


       float minhaDist = eu.distTo(bola.x, bola.y);
       float amigoDist = amigo.distTo(bola.x, bola.y);


       // Decisão de quem ataca (com critério de desempate pelo ID)
       bool euAtaco = (std::abs(minhaDist - amigoDist) < 0.02f) ? (state.myIndex == 2) : (minhaDist < amigoDist);


       if (euAtaco)
       {
           // Se estou atrás da bola (alinhado para o gol), entro em modo de ataque
           bool alinhado = (lado > 0) ? (eu.x < bola.x - 0.05f) : (eu.x > bola.x + 0.05f);


           if (alinhado)
           {
               alvoX = bola.x;
               alvoY = bola.y;
               modoAtaqueTotal = true;
           }
           else
           {
               // Ponto de preparação: 15cm atrás da bola
               alvoX = bola.x - (0.15f * lado);
               alvoY = bola.y;
           }
       }
       else
       {
           // SUPORTE: Posiciona-se no meio de campo
           alvoX = -0.30f * lado;
           alvoY = bola.y * 0.5f;
       }
   }


   // --- CAMADA 2: PLANEJADOR (Espaço de Configuração & Repulsão) ---


   // Vetor de Atração (para o alvo)
   float vecAtrairX = alvoX - eu.x;
   float vecAtrairY = alvoY - eu.y;


   // Vetor de Repulsão (Obstáculos)
   // float vecRepelirX = 0;
   // float vecRepelirY = 0;


   // Consideramos todos os outros robôs como obstáculos (Aliados e Oponentes)
   std::vector<EntityState> obstaculos = state.opponents;
   for (const auto &t : state.teammates)
       obstaculos.push_back(t);


   for (const auto &obs : obstaculos)
   {
       float d = eu.distTo(obs.x, obs.y);
       // "Raio de Influência": Se estiver a menos de 0.18m (metade da largura + margem)
       // Se estiver muito perto (C-Space crítico)
       if (d < 0.20f && d > 0.01f)
       {
           // 1. Força de Repulsão Pura
           float forcaRepelir = 0.6f * (0.20f - d) / d;
           float repX = (eu.x - obs.x) * forcaRepelir;
           float repY = (eu.y - obs.y) * forcaRepelir;


           // 2. FORÇA TANGENCIAL (O segredo para os 90°)
           // Isso cria um desvio lateral que "quebra" o ângulo reto
           float tanX = -repY;
           float tanY = repX;


           // Somamos a repulsão + um pouco de força tangencial para "escorregar"
           vecAtrairX += repX + (tanX * 0.5f);
           vecAtrairY += repY + (tanY * 0.5f);
       }


       // 3. Verificação de "Stuck" (Preso na Parede)
       // Se estiver nas quinas e a direção resultante for quase nula, força um movimento
       if (std::abs(eu.x) > 0.75f || std::abs(eu.y) > 0.55f)
       {
           for (const auto &op : state.opponents)
           {
               if (eu.distTo(op.x, op.y) < 0.15f)
               {
                   vecAtrairX += (eu.x - op.x) * 0.8f;
                   vecAtrairY += (eu.y > 0 ? -0.5f : 0.5f); // Força saída da parede
               }
           }
       }
   }


   // Soma dos Vetores: Direção Final = Atração + Repulsão
   // Se estiver em modoAtaqueTotal, ignoramos a repulsão da bola para conseguir tocá-la
   // float dirFinalX = vecAtrairX + vecRepelirX;
   // float dirFinalY = vecAtrairY + vecRepelirY;


   float anguloFinal = std::atan2(vecAtrairY, vecAtrairX);
   a.moveDirectionX = std::cos(anguloFinal);
   a.moveDirectionY = std::sin(anguloFinal);


   // Evitar trepidação no alvo
   if (!modoAtaqueTotal && eu.distTo(alvoX, alvoY) < 0.05f)
   {
       a.moveDirectionX = 0;
       a.moveDirectionY = 0;
   }


   // --- LÓGICA DE ESCAPE (Anti-Travamento) ---


   // Verifica se estamos perto de algum oponente (menos de 15cm)
   for (const auto &oponente : state.opponents)
   {
       if (eu.distTo(oponente.x, oponente.y) < 0.15f)
       {
           // Se estivermos travados perto da parede (escanteio)
           if (std::abs(eu.x) > 0.75f || std::abs(eu.y) > 0.55f)
           {
               // Adiciona uma força lateral "tangencial" para girar ao redor do oponente
               // Isso faz o robô "escorregar" pelo lado em vez de bater de frente
               float tangentialX = -(oponente.y - eu.y);
               float tangentialY = (oponente.x - eu.x);


               vecAtrairX += tangentialX * 0.8f;
               vecAtrairY += tangentialY * 0.8f;
           }
       }
   }


   return a;
}



