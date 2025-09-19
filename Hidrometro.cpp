#include "Hidrometro.h"
#include <chrono>
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
    std::cout << "[Config] Vazao: " << cfg.vazao_mm 
              << " mm, Area: " << cfg.area_m2 << " m2" << std::endl;
}

void Hidrometro::start() {
    bool expected = false;
    if (!running.compare_exchange_strong(expected, true)) return;

    // inicia leitura de configurações
    configWatcher.start();

    // inicia thread de simulação
    sim_thread = std::thread(&Hidrometro::simLoop, this);
}

void Hidrometro::stop() {
    bool expected = true;
    if (!running.compare_exchange_strong(expected, false)) {
        configWatcher.stop();
        if(sim_thread.joinable()) sim_thread.join();
    }
}

void Hidrometro::simLoop() {
    std::mt19937 rng((unsigned)std::time(nullptr));
    std::uniform_real_distribution<double> uni(0.0, 1.0);
    auto last = std::chrono::steady_clock::now();

    double last_volume = -1.0;
    double last_vazao = -1.0;
    double last_pressao = -1.0;

    while(running) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - last;
        double dt = elapsed.count();
        if (dt >= 1.0) {
            last = now;

            double vazao_mm = vazao.getVazaoMm();
            double a = area_m2.load();
            double litros_por_s = vazao_mm * a;
            double litros = litros_por_s * dt;

            // Volume
            if (vazao_mm > 0.0) {
                contador.adicionarLitros(litros);
                // Pressão quando há fluxo
                double base = 120.0;
                double p = base - (vazao_mm / 100.0) * 30.0; // 120..90 kPa
                pressao.setPressaoKpa(p);
            } else {
                // Quando vazão zero, simula ar
                double prob = 0.05; // 5% de pulso de ar
                if (uni(rng) < prob) {
                    pressao.setPressaoKpa(40.0 + uni(rng)*20.0);
                } else {
                    pressao.setPressaoKpa(30.0 + uni(rng)*15.0);
                }
            }

            // Callback somente se algum valor mudou
            double curr_volume = contador.getVolumeLitros();
            double curr_vazao = vazao.getVazaoMm();
            double curr_pressao = pressao.getPressaoKpa();

            if (curr_volume != last_volume || curr_vazao != last_vazao || curr_pressao != last_pressao) {
                std::lock_guard<std::mutex> lk(cb_mutex);
                if (cb) cb(curr_volume, curr_vazao, curr_pressao);

                last_volume = curr_volume;
                last_vazao = curr_vazao;
                last_pressao = curr_pressao;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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