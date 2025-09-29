# Simulador de Hidrômetro em C++ com Raylib

![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
![Library](https://img.shields.io/badge/library-Raylib-orange.svg)

Um simulador hidrômetro interativo e em tempo real, desenvolvido em C++ com a biblioteca Raylib. O projeto utilizza uma arquitetura multi-thread para separar a lógica da simulação da interface gráfica, garantindo uma visualização fluida e responsiva.

### 📜 Sobre o Projeto

Este projeto foi criado apra simular o funcionamento de um hidrômetro residencial de forma visual. A lógica da simulação é executada em uma thread separada, enquanto a thread principal cuida renderização gráfica com a biblioteca Raylib. Isso permite que a simulação seja complexa sem impactar a performance da interface do usuário.

A simulação pode ser controlada em tempo real pelo teclado e reage a mudanças em um araquivo de configuração externo (`config.txt`), que é monitorado durante a execução.

### ✨ Funcionalidades

* **Visualização Realista:** Interface gráfica que simula um hidrômetro real, com:
    * Odômetro digital com efeito de "tambor giratório" para o volume total.
    * Mostradores analógicos para Vazão e Pressão.
* **Controle Interativo:** A vazão do sistema pode ser aumentada, diminuída ou zerada em tempo real através do teclado.
* **Sistema Multi-thread:** A lógica do hidrômetro roda de forma independente da GUI.
* **Configuração Dinâmica:** O simulador monitora um arquivo `config.txt`, permitindo alterar parâmetros como a vazão base e a área do cano durante a execução.

### 🛠️ Tecnologias Utilizadas

* **Linguagem:** C++17
* **Biblioteca Gráfica:** [Raylib](https://www.raylib.com/)
* **Compilador:** MinGW (GCC) no Windows

### ⚙️ Instalação e Compilação

Para compilar e executar este projeto, você precisará do compilador MinGW (g++) e da biblioteca Raylib.

**1. Pré-requisitos**
* Instale o [MinGW-w64](https://www.mingw-w64.org/).
* Baixe a versão correta da [Raylib para MinGW](https://github.com/raysan5/raylib/releases) (ex: `raylib-X.X_win64_mingw.zip`).

**2. Passos**

1.  **Clone o repositório:**
    ```bash
    git clone [https://github.com/gugasilva005/Hidrometro_Sim.git](https://github.com/gugasilva005/Hidrometro_Sim.git)
    cd Hidrometro_Sim
    ```

2.  **Configure a Raylib:**
    * Crie uma pasta chamada `libs` ao lado da pasta do projeto.
    * Descompacte a Raylib dentro de `libs`. A estrutura final deve ser:
        ```
        Seus_Projetos/
        ├── libs/
        │   └── raylib-5.0_win64_mingw/  <-- Pasta da Raylib
        └── Hidrometro_Sim/              <-- Pasta do seu projeto
        ```

3.  **Compile o projeto:**
    * Abra um terminal na pasta `Hidrometro_Sim` e execute o seguinte comando:
    ```bash
    g++ main.cpp Hidrometro.cpp components/*.cpp -o simulador.exe -I"../libs/raylib-5.0_win64_mingw/include" -L"../libs/raylib-5.0_win64_mingw/lib" -lraylib -lopengl32 -lgdi32 -lwinmm -static -std=c++17
    ```
    *(**Nota:** O `components/*.cpp` compila todos os arquivos da pasta de componentes. Se estiver usando PowerShell, talvez precise listar os arquivos um por um se o `*` não funcionar).*

4.  **Execute:**
    ```bash
    ./simulador.exe
    ```

### 🕹️ Como Usar

* **Setas CIMA/BAIXO:** Aumenta ou diminui a vazão de água no sistema.
* **Barra de Espaço:** Zera a vazão instataneamente.
* **Tecla R:** Reseta o contador de volume do odômetro.