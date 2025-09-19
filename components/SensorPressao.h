#ifndef SENSORPRESSAO_H
#define SENSORPRESSAO_H

#include <atomic>

class SensorPressao {
    public:
        SensorPressao(double inicial_kpa = 0.0);
        void setPressaoKpa(double kpa);
        double getPressaoKpa() const;
    private:
        std::atomic<double> pressao_kpa;
};

#endif // SENSORPRESSAO_H