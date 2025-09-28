#include "Hidrometro.h"
#include <chrono>
#include <thread>
#include <cmath>
#include <random>
#include <iostream>

Hidrometro::Hidrometro(const std::string &config_path) :
    vazao(0.0), pressao(120.0), contador(), configWatcher(config_path, 5), running(false), area_m2(0.05)
{
    // registrar callback do config watcher para aplicar configurações
    configWatcher.setCallback([this](const HidroConfig &cfg) { this->applyConfig(cfg); });
}

Hidrometro::~Hidrometro() { stop(); }

void Hidrometro::applyConfig(const HidroConfig& cfg) {
    // Atualiza os parâmetros conforme config
    vazao.setVazaoMm(cfg.vazao_mm);
    area_m2.store(cfg.area_m2);

    // Log para verificar aplicação da configuração
    // std::cout << "[Config] Vazao: " << cfg.vazao_mm 
    //           << " mm, Area: " << cfg.area_m2 << " m2" << std::endl;
}

void Hidrometro::start() {
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true)) return;

    // inicia leitura de configurações
    configWatcher.start();

    // inicia thread de simulação/atualização
    update_thread = std::thread(&Hidrometro::updateLoop, this);
}

void Hidrometro::stop() {
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false)) {
        configWatcher.stop();
        if(update_thread.joinable()) update_thread.join();
    }
}


void Hidrometro::updateLoop() {
    // Define a frequência de atualização desejada (ex: 100 Hz)
    const auto update_interval = std::chrono::milliseconds(10); // 100 atualizações por segundo
    auto last_time = std::chrono::steady_clock::now();

    std::mt19937 rng((unsigned)std::time(nullptr));
    std::uniform_real_distribution<double> uni(0.0, 1.0);

    double last_volume = -1.0;
    double last_vazao = -1.0;
    double last_pressao = -1.0;

    while(running) {
        auto current_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = current_time - last_time;
        double dt = elapsed.count(); // dt agora é o tempo real decorrido, ex: ~0.01s
        last_time = current_time;

        // --- LÓGICA DA SIMULAÇÃO EXECUTADA A CADA CICLO ---
                double vazao_mm = vazao.getVazaoMm();
        double a = area_m2.load();
        double litros_por_s = vazao_mm * a;

        // Volume é incrementado continuamente com base no dt
        if (vazao_mm > 0.0) {
            contador.adicionarLitros(litros_por_s * dt);
            
            // Pressão quando há fluxo
            double base = 120.0;
            double p = base - (vazao_mm / 100.0) * 30.0; // 120..90 kPa
            pressao.setPressaoKpa(p);
        } else {
            // Quando vazão zero, simula ar
            double prob = 0.05; // 5% de chance por segundo
            // A probabilidade é ajustada por dt para ser consistente
            if (uni(rng) < (prob * dt)) {
                pressao.setPressaoKpa(40.0 + uni(rng)*20.0);
            } else {
                // A pressão de repouso é ajustada mais suavemente
                double pressao_atual = pressao.getPressaoKpa();
                double pressao_alvo = 30.0;
                // Interpola suavemente para a pressão de repouso
                pressao.setPressaoKpa(pressao_atual + (pressao_alvo - pressao_atual) * 0.1 * dt);
            }
        }

        // --- CALLBACK ---
        double curr_volume = contador.getVolumeLitros();
        double curr_vazao = vazao.getVazaoMm();
        double curr_pressao = pressao.getPressaoKpa();

        // Otimização: callback só é chamada se houver mudança
        if (curr_volume != last_volume || curr_vazao != last_vazao || curr_pressao != last_pressao) {
            std::lock_guard<std::mutex> lk(cb_mutex);
            if (cb) cb(curr_volume, curr_vazao, curr_pressao);

            last_volume = curr_volume;
            last_vazao = curr_vazao;
            last_pressao = curr_pressao;
        }

        // Controla a frequência do loop para não usar 100% da CPU
        auto execution_duration = std::chrono::steady_clock::now() - current_time;
        auto sleep_duration = update_interval - execution_duration;
        if (sleep_duration > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleep_duration);
        }
    }
}

void Hidrometro::setVazaoEntradaMm(double mm) { vazao.setVazaoMm(mm); }

double Hidrometro::getVazaoEntradaMm() const { return vazao.getVazaoMm(); }

double Hidrometro::getVolumeLitros() const { return contador.getVolumeLitros(); }

double Hidrometro::getPressaoKpa() const { return pressao.getPressaoKpa(); }

void Hidrometro::reset() { contador.reset(); }

void Hidrometro::setReadingCallback(ReadingCallback c) { 
    std::lock_guard<std::mutex> lk(cb_mutex); 
    cb = c; 
}