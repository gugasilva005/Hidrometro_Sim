#include "Hidrometro.h"
#include <iostream>
#include <thread>
#include <chrono>


int main(){
    // caminho do arquivo config
    std::string cfg = "config.txt";

    Hidrometro h(cfg);
    h.setReadingCallback([](double litros,double vaz,double press) {
        printf("Leitura -> Volume: %.3f L | Vazao: %.2f mm | Pressao: %.1f kPa", litros, vaz, press);
        printf("\n");
    });

    h.start();

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    h.stop();
    return 0;
}