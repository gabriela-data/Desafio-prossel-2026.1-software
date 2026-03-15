# Desafio de Prossel: Simulador de Futebol 2D (Padrão VSSS)

Bem-vindo ao desafio de programação de robôs! Seu objetivo é implementar a lógica de comportamento de uma equipe de 3 robôs em um simulador que segue as proporções reais da categoria VSSS (Very Small Size Soccer).

---

## 🚀 Como Iniciar

Toda a sua lógica deve ser implementada dentro da pasta `solucao/`. 
O ponto de entrada principal é a classe `Estrategia` nos arquivos:
- `solucao/include/Estrategia.h`
- `solucao/src/Estrategia.cpp`
### 📦 Dependências

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install cmake g++ libx11-dev libxcursor-dev libxrandr-dev libudev-dev libgl1-mesa-dev libglu1-mesa-dev
```

#### Windows
1. Instale o **[CMake](https://cmake.org/download/)**.
2. Instale um compilador C++:
   - **Opção A (Recomendada):** **[Visual Studio 2022](https://visualstudio.microsoft.com/pt-br/vs/community/)** (Marque a carga de trabalho "Desenvolvimento para desktop com C++").
   - **Opção B:** **[MinGW-w64](https://www.mingw-w64.org/)** (via MSYS2 ou instalador direto).
3. O CMake cuidará do download da SFML automaticamente durante a configuração.

### 🛠️ Compilação e Execução

#### Linux
1. Compile o projeto: `mkdir build && cd build && cmake .. && make -j$(nproc)`
2. Execute o binário: `./SimuladorFutebol`

#### Windows (Via Terminal/PowerShell)
1. Crie a pasta de build: `mkdir build`
2. Entre na pasta: `cd build`
3. Configure o projeto: `cmake ..`
4. Compile: `cmake --build . --config Release`
5. Execute o binário: `.\Release\SimuladorFutebol.exe`

#### Windows (Via Visual Studio)
1. Abra o Visual Studio 2022.
2. Selecione **"Abrir uma Pasta Local"** e escolha a raiz deste projeto.
3. O Visual Studio detectará o `CMakeLists.txt` automaticamente.
4. Clique em **"Compilar" > "Compilar Tudo"**.
5. Selecione `SimuladorFutebol.exe` no menu de inicialização e aperte **F5**.

3. **Automático:** Assim que a janela abrir, todos os robôs já estarão executando o seu código do método `think()`.

> **Nota:** Por padrão, tanto o **Time Azul** quanto o **Time Vermelho** utilizam a sua classe `Estrategia`.
> 
> **Dica:** Você pode usar a variável **`this->teamA`** (booleana) para diferenciar o comportamento entre os times.

## 🎯 Requisitos da Solução

Para uma solução profissional e robusta, espera-se que você implemente duas camadas lógicas principais:

1.  **Tomador de Decisão:** Uma estrutura que determine o que cada robô deve fazer em cada momento (ex: "Ir para a bola", "Ficar na defesa", "Bloquear oponente"). Você pode usar **Máquinas de Estados (FSM)**, **Behavior Trees (BT)** ou lógicas similares.
2.  **Planejador de Caminho (Planner):** Uma lógica que calcule o movimento necessário para atingir o objetivo definido pelo Tomador de Decisão. 
    *   *Dica:* Um bom planejador não apenas vai em linha reta, mas considera o desvio de obstáculos e o posicionamento correto para empurrar a bola.

---

## 📐 Unidades e Medidas (Sistema Internacional)

O simulador trabalha exclusivamente com **METROS** e **SEGUNDOS**.
- **Sistema de Coordenadas:** O centro do campo é o ponto `(0, 0)`.
- **Limites de X:** de `-0.85` até `+0.85`.
- **Limites de Y:** de `-0.65` até `+0.65`.
- **Gols:** Verticalmente entre `y = -0.20` e `y = +0.20`.

---

## 🧠 Como Programar

O simulador chama `think(const GameState& state)` para cada robô. Você tem acesso aos seguintes dados:
- **Seu Robô:** `state.getMe()` (Posição `x, y` e Velocidade `vx, vy`).
- **A Bola:** `state.ball` (Posição `x, y` e Velocidade `vx, vy`).
- **Aliados e Inimigos:** Listas `state.teammates` e `state.opponents`.

```cpp
Action Estrategia::think(const GameState& state) {
    Action a;
    const EntityState& eu = state.getMe();
    const EntityState& bola = state.ball;

    // Exemplo: Como mover para um ponto (Ex: x=0.4, y=0.2)
    float alvoX = 0.4f;
    float alvoY = 0.2f;

    float angulo = eu.angleTo(alvoX, alvoY);
    a.moveDirectionX = std::cos(angulo);
    a.moveDirectionY = std::sin(angulo);

    // Como saber se chegou no ponto? (Threshold de 5cm)
    if (eu.distTo(alvoX, alvoY) < 0.05f) {
        a.moveDirectionX = 0; a.moveDirectionY = 0;
    }

    return a;
}
```

---

## 🤝 Coordenando Robôs e Times

Use o `state.myIndex` (0, 1 ou 2) para definir papéis e `this->teamA` para táticas diferentes por time:

```cpp
if (this->teamA) {
    // Sou do Time Azul. Meu alvo é o X positivo (+0.85)
} else {
    // Sou do Time Vermelho. Meu alvo é o X negativo (-0.85)
}
```

---

## 🖥️ Monitoramento via Terminal

O terminal exibe o estado do jogo a 10Hz no formato:
`[METROS] Bola:(X, Y) | Azul: (P0_X, P0_Y) (P1_X, P1_Y) (P2_X, P2_Y)`

---

## 🛠️ Regras Físicas
1. **Duração:** As partidas duram 5 minutos (300 segundos).
2. **Velocidade:** Robôs atingem até `0.5 m/s`.
3. **Colisões:** Robôs colidem entre si, com a bola e com as paredes. 
4. **Gols:** Devem ser feitos empurrando a bola fisicamente. Não há comando de chute.

---

## 📤 Entrega e Avaliação

Ao finalizar o desafio, você deve entregar um **repositório (GitHub/GitLab)** contendo:
1.  O código fonte da sua solução (pasta `solucao/`).
2.  Um documento explicativo (pode ser um `README_SOLUCAO.md` ou um `PDF`).

### 📝 O que deve conter no documento:
- **Explicação da Lógica:** Detalhamento do seu **Tomador de Decisão** e Planejador.
- **Justificativa das Escolhas:** Por que você escolheu essa arquitetura? (Ex: Por que usar uma Behavior Tree em vez de uma Máquina de Estados? Ou, caso tenha feito via lógica linear no código, por que essa escolha foi suficiente?).
- **Coordenação:** Como você lidou com a matemática e a cooperação entre os 3 robôs.

### 📊 Critérios de Avaliação:
- **Robustez:** Como os robôs reagem a diferentes situações.
- **Organização:** Código limpo e modularizado.
- **Matemática:** Uso de vetores, trigonometria e física.
- **Planejamento de Rota:** Eficiência na movimentação.
- **Apresentação:** Você deverá explicar sua solução para a equipe.

> **⚠️ Regra sobre IA:** O uso de IA não é proibido para auxílio, mas a solução **não deve ser feita inteiramente por IA**. Você deve ser capaz de explicar cada parte da lógica. O uso excessivo sem compreensão resultará em eliminação.

Boa sorte! ⚽🤖🚀
