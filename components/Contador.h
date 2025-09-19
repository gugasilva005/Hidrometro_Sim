#ifndef CONTADOR_H
#define CONTADOR_H

#include <mutex>

class Contador {
    public:
        Contador();
        void adicionarLitros(double litros);
        double getVolumeLitros() const;
        void reset();
    private:
        double volume_litros;
        mutable std::mutex mtx;
};

#endif // CONTADOR_H