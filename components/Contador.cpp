#include "Contador.h"

Contador::Contador() : volume_litros(0.0) { } 

void Contador::adicionarLitros(double litros) {
    if (litros < 0.0)
        litros = 0.0;
    std::lock_guard<std::mutex> lock(mtx);
    volume_litros += litros;
}

double Contador::getVolumeLitros() const {
    std::lock_guard<std::mutex> lock(mtx);
    return volume_litros;
}

void Contador::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    volume_litros = 0.0;
}