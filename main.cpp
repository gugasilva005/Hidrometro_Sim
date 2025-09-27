#include "Hidrometro.h"
#include "HidrometroGUI.cpp"
#include <iostream>
#include <thread>
#include <chrono>


int main() {
    std::string cfg = "config.txt";
    Hidrometro h(cfg);

    // start da simulação do hidrômetro
    h.start();

    HidrometroGUI gui(h);
    gui.run();

    h.stop();
    return 0;
}