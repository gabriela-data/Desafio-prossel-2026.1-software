// PlanejadorDStar.h
#ifndef PLANEJADOR_DSTAR_H
#define PLANEJADOR_DSTAR_H

#include "Planejador.h"
#include <vector>
#include <queue>
#include <cmath>

struct CellKey
{
    double k1, k2;
    bool operator<(const CellKey &other) const
    {
        if (k1 != other.k1)
            return k1 < other.k1;
        return k2 < other.k2;
    }
};

struct CellCompare
{
    bool operator()(const std::pair<CellKey, std::pair<int, int>> &a,
                    const std::pair<CellKey, std::pair<int, int>> &b) const
    {
        return a.first < b.first;
    }
};

class PlanejadorDStar : public Planejador
{
public:
    PlanejadorDStar();
    void planejar(const GameState &state,
                  const EntityState &eu,
                  float alvoX,
                  float alvoY,
                  float &vecX,
                  float &vecY) override;

private:
    // Configuração da grade
    static constexpr double RESOLUCAO = 0.1;   // tamanho da célula em metros
    static constexpr double ROBOT_RAIO = 0.08; // raio do robô para colisão
    static constexpr double MARGEM = 0.05;     // margem extra
    static constexpr double INF = 1e9;

    int width, height;
    double minX, minY;

    // Estruturas do D* Lite
    std::vector<std::vector<double>> g;
    std::vector<std::vector<double>> rhs;
    std::vector<std::vector<bool>> obstaculo;
    double km;
    std::pair<int, int> start, goal;
    std::priority_queue<std::pair<CellKey, std::pair<int, int>>,
                        std::vector<std::pair<CellKey, std::pair<int, int>>>,
                        CellCompare>
        U;
    std::vector<std::vector<bool>> inU; // para marcação de presença na fila

    // Funções auxiliares
    void inicializarGrade();
    void atualizarObstaculos(const GameState &state, const EntityState &eu);
    void clearDStarState();
    CellKey calcularChave(int x, int y);
    void atualizarVertice(int x, int y);
    double custo(int x1, int y1, int x2, int y2);
    double heuristica(int x, int y);
    std::vector<std::pair<double, double>> computeShortestPath();
    std::pair<int, int> worldToGrid(double wx, double wy);
    std::pair<double, double> gridToWorld(int gx, int gy);
    bool valido(int x, int y) const;
};

#endif