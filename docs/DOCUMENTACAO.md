# Documentação do Projeto: Simulador de Hidrômetro (SHA 2.0)

Este documento detalha a arquitetura e os principais componentes do Simulador de Hidrômetro, um sistema desenvolvido em C++ que utiliza a biblioteca Raylib para visualização gráfica e uma arquitetura multi-thread para a lógica da simulação.

## 1. Visão Geral da Arquitetura

O projeto é construído em torno de um padrão de design **Facade**, onde a classe `Hidrometro` atua como a interface principal, orquestrando um conjunto de componentes especializados. A simulação roda em uma thread separada, garantindo que a interface gráfica (GUI) permaneça fluida e responsiva.

---

## 2. Classes Principais

### 2.1. `Hidrometro`
Esta é a classe central e a fachada do sistema. Ela gerencia o ciclo de vida da simulação e coordena todos os componentes.

* **Responsabilidades:**
    * Orquestrar os componentes (`VazaoEntrada`, `SensorPressao`, `Contador`, `ConfigWatcher`).
    * Gerenciar a thread da simulação (`update_thread`).
    * Fornecer uma API pública e segura para a `main` (GUI) interagir com a simulação.

* **Métodos Importantes:**
    * `Hidrometro(const std::string &config_path)`: O construtor inicializa todos os componentes e configura o callback do `ConfigWatcher` para aplicar configurações dinamicamente.
    * `start()`: Inicia o `ConfigWatcher` e lança o `updateLoop()` em uma nova thread, começando a simulação.
    * `stop()`: Finaliza a simulação de forma segura, sinalizando para a thread parar e aguardando sua conclusão (`join()`).
    * `setVazaoEntradaMm(double mm)`: Permite que a interface de usuário (ou outra parte do sistema) defina a vazão desejada, que será usada pela simulação.
    * `reset()`: Reinicia o contador de volume, chamando o método `reset()` do componente `Contador`.
    * `applyConfig(const HidroConfig &cfg)`: Método privado chamado pelo `ConfigWatcher` para atualizar os parâmetros da simulação (como vazão e área) em tempo de execução.
    * `updateLoop()`: O coração da simulação. Este método roda continuamente na thread secundária. A cada ciclo, ele calcula o `deltaTime`, atualiza o estado dos componentes com base na vazão atual, calcula o novo volume e pressão, e chama o callback se houver mudanças.

---

### 2.2. `VazaoEntrada`
Componente responsável por gerenciar o valor da vazão de entrada.

* **Responsabilidades:**
    * Armazenar e fornecer o valor atual da vazão em milímetros (`mm`).

* **Métodos Importantes:**
    * `setVazaoMm(double mm)`: Atualiza o valor da vazão. O uso de `std::atomic` garante que esta operação seja segura entre threads.

---

### 2.3. `SensorPressao`
Componente que simula o sensor de pressão do sistema.

* **Responsabilidades:**
    * Armazenar e fornecer o valor da pressão em quilopascals (kPa).

* **Métodos Importantes:**
    * `setPressaoKpa(double kpa)`: Atualiza o valor da pressão. O `std::atomic` garante a segurança da operação.

---

### 2.4. `Contador`
Componente responsável por acumular e gerenciar o volume total de água consumida.

* **Responsabilidades:**
    * Manter o registro do volume total em litros.

* **Métodos Importantes:**
    * `adicionarLitros(double litros)`: Incrementa o volume total. Esta é a função central que faz o odômetro "avançar".
    * `reset()`: Zera o contador de volume.

---

### 2.5. `ConfigWatcher`
Um componente mais complexo, responsável por monitorar o arquivo `config.txt` para detectar mudanças e recarregar a configuração dinamicamente.

* **Responsabilidades:**
    * Rodar uma thread de monitoramento em segundo plano.
    * Ler o arquivo de configuração em intervalos definidos.
    * Notificar a classe `Hidrometro` sobre novas configurações através de um callback.

* **Métodos Importantes:**
    * `start()`: Inicia a thread de monitoramento (`watcher_thread`).
    * `stop()`: Para a thread de monitoramento de forma segura.
    * `watchLoop()`: Método privado que executa na thread. Ele entra em um loop que, a cada `interval_s` segundos, chama `readConfig()` e, se detectar mudanças, invoca o callback registrado.
    * `readConfig()`: Abre e lê o arquivo `config.txt`, preenchendo e retornando uma estrutura `HidroConfig`.

---

### 2.6. `HidroConfig` (struct)
Uma estrutura de dados simples (`struct`) usada para transportar os dados lidos do arquivo `config.txt` entre o `ConfigWatcher` e a classe `Hidrometro`.

---

### 3. `main.cpp` (Ponto de Entrada e GUI)

O arquivo `main.cpp` serve como a camada de apresentação e controle do usuário.

* **Responsabilidades:**
    * Inicializar a janela gráfica usando a Raylib.
    * Instanciar e gerenciar o ciclo de vida do objeto `Hidrometro` (`start()` e `stop()`).
    * Executar o loop principal da aplicação (game loop).
    * **Capturar a entrada do usuário** (teclado) para definir uma "vazão alvo".
    * **Implementar a lógica de suavização (interpolação)** para que a vazão mude gradualmente, proporcionando um controle mais preciso e linear.
    * **Ler os dados** da simulação a cada quadro (usando os métodos `get...()` do `Hidrometro`).
    * **Desenhar a interface gráfica**, incluindo o odômetro e os mostradores analógicos, com base nos dados lidos.