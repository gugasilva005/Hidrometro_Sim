#include "SensorPressao.h"

SensorPressao::SensorPressao(double inicial_kpa) : pressao_kpa(inicial_kpa) { }

void SensorPressao::setPressaoKpa(double kpa) { pressao_kpa = kpa; }

double SensorPressao::getPressaoKpa() const { return pressao_kpa.load(); }