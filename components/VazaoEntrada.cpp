#include "VazaoEntrada.h"

VazaoEntrada::VazaoEntrada(double inicial_mm) : vazao_mm(inicial_mm) {}

void VazaoEntrada::setVazaoMm(double mm) {
    // if (mm < 0.0)
    //     mm = 0.0;
    // if (mm > 100.0)
    //     mm = 100.0;
    vazao_mm = mm;
}

double VazaoEntrada::getVazaoMm() const {
    return vazao_mm.load();
}