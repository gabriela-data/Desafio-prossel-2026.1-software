//PlanejadorDStar.cpp
#include "PlanejadorDStar.h"
#include <algorithm>
#include <limits>

PlanejadorDStar::PlanejadorDStar() : km(0)
{
    // Campo: x de -0.8 a 0.8, y de -0.6 a 0.6 (ajuste conforme seu campo)
    minX = -0.8;
    minY = -0.6;
    double maxX = 0.8;
    double maxY = 0.6;

    width = static_cast<int>((maxX - minX) / RESOLUCAO) + 1;
    height = static_cast<int>((maxY - minY) / RESOLUCAO) + 1;

    g.resize(width, std::vector<double>(height, INF));
    rhs.resize(width, std::vector<double>(height, INF));
    obstaculo.resize(width, std::vector<bool>(height, false));
    inU.resize(width, std::vector<bool>(height, false));
}

void PlanejadorDStar::inicializarGrade()
{
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
        {
            g[x][y] = INF;
            rhs[x][y] = INF;
            obstaculo[x][y] = false;
            inU[x][y] = false;
        }
    // Limpa a fila de prioridade (não há método clear direto)
    while (!U.empty())
        U.pop();
    km = 0;
}

void PlanejadorDStar::clearDStarState()
{
    km = 0;
    while (!U.empty())
        U.pop();
    for (auto &row : inU)
        std::fill(row.begin(), row.end(), false);
}

std::pair<int, int> PlanejadorDStar::worldToGrid(double wx, double wy)
{
    int gx = static_cast<int>((wx - minX) / RESOLUCAO);
    int gy = static_cast<int>((wy - minY) / RESOLUCAO);
    gx = std::max(0, std::min(width - 1, gx));
    gy = std::max(0, std::min(height - 1, gy));
    return {gx, gy};
}

std::pair<double, double> PlanejadorDStar::gridToWorld(int gx, int gy)
{
    double wx = minX + gx * RESOLUCAO;
    double wy = minY + gy * RESOLUCAO;
    return {wx, wy};
}

bool PlanejadorDStar::valido(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

double PlanejadorDStar::custo(int x1, int y1, int x2, int y2)
{
    if (obstaculo[x1][y1] || obstaculo[x2][y2])
        return INF;
    // Movimento 8-conectado: diagonal = sqrt(2), ortogonal = 1
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);
    if (dx + dy == 1)
        return 1.0; // ortogonal
    else if (dx == 1 && dy == 1)
        return 1.41421356; // diagonal
    return INF;
}

double PlanejadorDStar::heuristica(int x, int y)
{
    // Distância Euclidiana em unidades de célula (multiplica pela resolução depois, mas aqui usamos distância em células)
    double dx = x - goal.first;
    double dy = y - goal.second;
    return std::sqrt(dx * dx + dy * dy);
}

CellKey PlanejadorDStar::calcularChave(int x, int y)
{
    double minVal = std::min(g[x][y], rhs[x][y]);
    return {minVal + heuristica(x, y) + km, minVal};
}

void PlanejadorDStar::atualizarVertice(int x, int y)
{
    if (!valido(x, y))
        return;
    if (x == goal.first && y == goal.second)
    {
        rhs[x][y] = 0;
    }
    else
    {
        rhs[x][y] = INF;
        // Vizinhos 8-conectados
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0)
                    continue;
                int nx = x + dx, ny = y + dy;
                if (!valido(nx, ny))
                    continue;
                double c = custo(x, y, nx, ny);
                if (c < INF)
                {
                    rhs[x][y] = std::min(rhs[x][y], g[nx][ny] + c);
                }
            }
        }
    }
    // Remove da fila se presente
    if (inU[x][y])
    {
        inU[x][y] = false;
    }
    if (g[x][y] != rhs[x][y])
    {
        U.push({calcularChave(x, y), {x, y}});
        inU[x][y] = true;
    }
}

void PlanejadorDStar::atualizarObstaculos(const GameState &state, const EntityState &eu)
{
    // Marca todas as células como livres inicialmente
    for (auto& row : obstaculo)
        std::fill(row.begin(), row.end(), false);

    double obstaculo_raio = ROBOT_RAIO + MARGEM;

    // Marca oponentes como obstáculos
    for (const auto& op : state.opponents)
    {
        // Para cada oponente, bloqueia as células dentro do raio
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                auto [wx, wy] = gridToWorld(x, y);
                double dx = wx - op.x;
                double dy = wy - op.y;
                if (dx * dx + dy * dy <= obstaculo_raio * obstaculo_raio)
                {
                    obstaculo[x][y] = true;
                }
            }
        }
    }

    // Marca companheiros como obstáculos (exceto o próprio robô)
    for (const auto &t : state.teammates)
    {
        // Ignora o próprio robô comparando posições (evita usar .id)
        double dx = t.x - eu.x;
        double dy = t.y - eu.y;
        if (dx * dx + dy * dy < 0.0001)
            continue; // mesma posição = eu mesmo
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                auto [wx, wy] = gridToWorld(x, y);
                double dx = wx - t.x;
                double dy = wy - t.y;
                if (dx*dx + dy*dy <= obstaculo_raio*obstaculo_raio) {
                    obstaculo[x][y] = true;
                }
            }
        }
    }
}

std::vector<std::pair<double, double>> PlanejadorDStar::computeShortestPath()
{
    // D* Lite principal
    while (!U.empty())
    {
        auto top = U.top();
        CellKey key = top.first;
        auto [x, y] = top.second;
        U.pop();

        if (!inU[x][y])
            continue; // entrada inválida
        inU[x][y] = false;

        if (key < calcularChave(x, y))
            continue; // chave desatualizada
        if (g[x][y] > rhs[x][y])
        {
            g[x][y] = rhs[x][y];
        }
        else
        {
            g[x][y] = INF;
            atualizarVertice(x, y);
        }
        // Propaga para vizinhos
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0)
                    continue;
                int nx = x + dx, ny = y + dy;
                if (valido(nx, ny))
                {
                    atualizarVertice(nx, ny);
                }
            }
        }
        if (calcularChave(start.first, start.second) < calcularChave(goal.first, goal.second))
            continue;
    }

    // Após o cálculo, extrai o caminho (seguindo gradiente)
    std::vector<std::pair<double, double>> path;
    if (g[start.first][start.second] >= INF)
    {
        // Sem caminho: retorna vetor vazio (tratado como parado)
        return path;
    }
    int cx = start.first, cy = start.second;
    while (!(cx == goal.first && cy == goal.second))
    {
        // Adiciona célula atual ao caminho (coordenadas do centro)
        auto [wx, wy] = gridToWorld(cx, cy);
        path.push_back({wx, wy});
        // Escolhe vizinho com menor g + custo
        double best = INF;
        int nx_best = cx, ny_best = cy;
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0)
                    continue;
                int nx = cx + dx, ny = cy + dy;
                if (!valido(nx, ny))
                    continue;
                double c = custo(cx, cy, nx, ny);
                if (c < INF && g[nx][ny] + c < best)
                {
                    best = g[nx][ny] + c;
                    nx_best = nx;
                    ny_best = ny;
                }
            }
        }
        if (best >= INF)
            break; // beco sem saída
        cx = nx_best;
        cy = ny_best;
    }
    // Adiciona o goal
    auto [gx, gy] = gridToWorld(goal.first, goal.second);
    path.push_back({gx, gy});
    return path;
}

void PlanejadorDStar::planejar(const GameState &state,
                               const EntityState &eu,
                               float alvoX,
                               float alvoY,
                               float &vecX,
                               float &vecY)
{
    // 1. Atualizar obstáculos
    atualizarObstaculos(state, eu);

    // 2. Definir start e goal discretos
    auto new_start = worldToGrid(eu.x, eu.y);
    auto new_goal = worldToGrid(alvoX, alvoY);

    // Se já no objetivo (célula), retorna vetor zero
    if (new_start == new_goal)
    {
        vecX = 0.0f;
        vecY = 0.0f;
        return;
    }

    // 3. Se o goal mudou, reiniciar o D* Lite
    if (new_goal != goal)
    {
        goal = new_goal;
        clearDStarState();
        // Inicializa rhs e g do goal
        for (auto &row : rhs)
            std::fill(row.begin(), row.end(), INF);
        rhs[goal.first][goal.second] = 0;
        U.push({calcularChave(goal.first, goal.second), goal});
        inU[goal.first][goal.second] = true;
        // Não reseta g, pois será sobrescrito no compute
    }

    // 4. Se o start mudou, atualiza km e heurística
    if (start != new_start)
    {
        km += heuristica(start.first, start.second); // na verdade, D* Lite tradicional faz km += h(start_antigo, start_novo)
        start = new_start;
    }

    // 5. Recalcula caminho (D* Lite)
    auto path = computeShortestPath();

    // 6. Gera vetor de movimento: do robô para a primeira célula do caminho (ou segunda, se a primeira for a atual)
    if (path.empty())
    {
        // Sem caminho, fica parado
        vecX = 0.0f;
        vecY = 0.0f;
        return;
    }

    // O primeiro ponto do caminho é a posição atual (start). Usamos o segundo como alvo local.
    double target_x, target_y;
    if (path.size() >= 2)
    {
        target_x = path[1].first;
        target_y = path[1].second;
    }
    else
    {
        // Só tem o goal (provavelmente já está lá)
        target_x = path[0].first;
        target_y = path[0].second;
    }

    vecX = target_x - eu.x;
    vecY = target_y - eu.y;

    // Normalização opcional (a Estratégia já normaliza via atan2, mas aqui garantimos direção unitária)
    double len = std::sqrt(vecX * vecX + vecY * vecY);
    if (len > 0.001)
    {
        vecX /= len;
        vecY /= len;
    }
    else
    {
        vecX = 0.0f;
        vecY = 0.0f;
    }
}