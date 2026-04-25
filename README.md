
**Gabriela de Almeida da Silva** 

```markdown
# Prossel 2026.1 Oxebots

Implementação modular de inteligência para uma equipe de 3 robôs na categoria Very Small Size Soccer (VSSS).  
A solução separa **tomada de decisão** (FSM) e **planeamento de caminhos** (interface com injeção de dependência), permitindo testes com diferentes algoritmos - Campos Potenciais e D* Lite.

---

## Estrutura do Projeto

```
solucao/       
                                                                                          ├── include/                        # Interfaces e cabeçalhos público  
│   ├── Planejador.h                # Interface abstrata do planejador           
│   ├── PlanejadorPotencial.h       # Declaração do planejador por campos potenciais   
│   ├── PlanejadorDStar.h           # Declaração do planejador D* Lite  
│   └── Estrategia.h                # Classe principal (FSM + orquestração)  
├── src/                            # Implementações  
│   ├── PlanejadorPotencial.cpp  
│   ├── PlanejadorDStar.cpp   
│   └── Estrategia.cpp
└── CMakeLists.txt                  



## Arquitetura do código: Tomada de Decisão + Planejamento

### Diagrama de fluxo


GameState
   │
   ▼

┌─────────────────────────┐
│  Estrategia::decidirAlvo│   ← Máquina de Estados
│  alvoX, alvoY           │
└───────────┬─────────────┘
            │
            ▼

┌─────────────────────────┐
│  planejador->planejar() │   ← Interface Planejador (injétavel)
│  vecX, vecY             │
└───────────┬─────────────┘
            │
            ▼

┌─────────────────────────┐
│  Ação (moveDirection)   │
│  + paragem se perto     │
└─────────────────────────┘





```

O planejador é injetado no construtor da `Estrategia` - isso permite trocar o algoritmo de desvio de obstáculos sem alterar a FSM.

// Construtor com injeção de dependência
Estrategia(int id, bool isTeamA, std::unique_ptr<Planejador> planejador);

// Exemplo de uso:
if (isTeamA)
    agent = std::make_unique<Estrategia>(id, true,
                std::make_unique<PlanejadorPotencial>());
else
    agent = std::make_unique<Estrategia>(id, false,
                std::make_unique<PlanejadorDStar>());
```


## Tomador de Decisão (FSM)

Os papéis são definidos pelo `myIndex`:

| Índice | Papel     | Comportamento principal |
|--------|-----------|--------------------------|
| 0      | Goleiro   | Defender a baliza; **empurrar a bola para fora da área** se ela entrar. |
| 1      | Ala       | Coordenar com o atacante: atacar a bola ou dar suporte defensivo. |
| 2      | Atacante  | Prioridade ofensiva; em caso de empate na distância, ele avança. |

### Goleiro melhorado (feedback aplicado)
- Bola dentro da área: o alvo passa a ser **uma posição além da bola**, garantindo que o goleiro a empurre para fora da zona de perigo.
- Bola fora da área: mantém‑se sobre a linha do gol, acompanhando a coordenada Y da bola.

### Jogadores de linha
- **Quem ataca?** O robô mais próximo da bola assume o ataque; em caso de empate (diferença < 2 cm), o atacante (ID 2) ganha prioridade.
- **Atacante:** move‑se para trás da bola (ponto de preparação) até ficar alinhado; quando alinhado, dispara em "ataque total" direto à bola.
- **Suporte:** recua para uma posição mais defensiva, cobrindo o campo e acompanhando lateralmente a bola.

---

## Planeadores de Caminho (Algoritmos usados)

### 1. Campos Potenciais

Inspirado em forças da física: atração ao alvo + repulsão de obstáculos + força tangencial (contorno) + escape de paredes.

**Fórmula do vetor resultante:**

$$
\vec{v}_{res} = \vec{F}_{atracao} + \sum_{obs} \vec{F}_{rep}(obs) + \vec{F}_{tan} + \vec{F}_{escape}
$$

- **Atração:** `vec = (alvoX - eu.x, alvoY - eu.y)`
- **Repulsão:** para cada obstáculo a distância `d < R_rep`, força inversamente proporcional:  
  `F_rep = K_rep * (R_rep - d) / d`
- **Força tangencial:** perpendicular à repulsão, evita que o robô bata de frente e oscile.
- **Escape de paredes:** se o robô fica preso nos limites com adversário próximo, gera uma força para o centro do campo.

#### Ajustes pós‑feedback para evitar vibração e mínimos locais

- **Intensidade da repulsão reduzida:** de `0.6` para `0.3` - isso reduziu a vibração excessiva.
- **Perturbação contra mínimos locais:** quando o vetor resultante é muito pequeno (robô preso), adiciona‑se um pequeno vetor aleatório para desbloquear o caminho.

```cpp
// Trecho do escape de mínimo local (PlanejadorPotencial.cpp)
float mag = std::sqrt(vecX*vecX + vecY*vecY);
if (mag < 0.01f) {
    float ang = (float)rand() / RAND_MAX * 2 * M_PI;
    vecX += std::cos(ang) * 0.1f;
    vecY += std::sin(ang) * 0.1f;
}
```

### 2. D* Lite

Esse algoritmo foi utilizado por ser de replanejamento incremental para ambientes dinâmicos como o do futebol que a todo momento terão adversários e obstáculos.

O campo é discretizado numa grelha de 10 cm. Cada célula tem dois valores:
- **`g`**: custo real estimado até ao objetivo.
- **`rhs`**: custo "um‑passo‑à‑frente" baseado nos vizinhos.

Quando `g ≠ rhs`, a célula é **inconsistente** e entra numa fila de prioridade. Em cada frame, apenas as células afectadas por novos obstáculos são reprocessadas.

Após o cálculo, o robô segue o gradiente descendente de `g + custo` até ao alvo, garantindo um caminho óptimo (dentro da resolução da grelha) e livre de mínimos locais.

---

## Coordenação e Matemática

- **Unidades no SI:** todas as constantes em metros e segundos.
- **Cálculo do lado do campo:** `lado = teamA ? 1.0 : -1.0` (inverte eixos e limites conforme o time).
- **Distâncias e ângulos:** uso intensivo de `distTo()`, `atan2()`, normalização de vetores.
- **Coordenação:** o `myIndex` define papéis; a escolha de quem ataca é dinâmica e baseada em distância, com desempate determinístico.

---

## Compilação e Execução

### Dependências (Linux)
```bash
sudo apt-get install cmake g++ libx11-dev libxcursor-dev \
  libxrandr-dev libudev-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Compilar
```bash
cmake -G "Unix Makefiles" -B build
cd build && make -j$(nproc)
```







