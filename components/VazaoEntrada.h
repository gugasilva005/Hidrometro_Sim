#ifndef VAZAOENTRADA_H
#define VAZAOENTRADA_H

#include <atomic>

class VazaoEntrada {
    public:
        VazaoEntrada(double inicial_mm = 0.0);
        void setVazaoMm(double mm);
        double getVazaoMm() const;
    private:
        std::atomic<double> vazao_mm;
};

#endif // VAZAOENTRADA_H