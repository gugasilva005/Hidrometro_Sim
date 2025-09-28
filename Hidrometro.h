#ifndef HIDROMETRO_H
#define HIDROMETRO_H

#include "components/Contador.h"
#include "components/VazaoEntrada.h"
#include "components/SensorPressao.h"
#include "components/ConfigWatcher.h"

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

using ReadingCallback = std::function<void(double litros, double vazao_mm, double pressao_kpa)>;

class Hidrometro {
    public:
        Hidrometro(const std::string &config_path);
        ~Hidrometro();

        void start();
        void stop();

        // API de controle
        void setVazaoEntradaMm(double mm);
        double getVazaoEntradaMm() const;

        double getVolumeLitros() const;
        double getPressaoKpa() const;

        void reset();

        void setReadingCallback(ReadingCallback cb);

    private:
        void updateLoop();
        void applyConfig(const HidroConfig &cfg);

        // componentes
        VazaoEntrada vazao;
        SensorPressao pressao;
        Contador contador;
        ConfigWatcher configWatcher;

        // threads
        std::thread update_thread;
        std::atomic<bool> running;

        // callback
        std::mutex cb_mutex;
        ReadingCallback cb;

        // parâmetros operacionais
        std::atomic<double> area_m2;    // usado pa conversão mm --> L/s
};

#endif // HIDROMETRO_H