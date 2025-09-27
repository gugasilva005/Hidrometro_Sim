#include <SFML/Graphics.hpp>
#include "Hidrometro.h"
#include <cmath>

class HidrometroGUI {
private:
    sf::RenderWindow window;
    sf::CircleShape dial;
    sf::RectangleShape needleVolume;
    sf::RectangleShape needlePressao;

    Hidrometro& hidr;
    float angleVolume = 0.f;
    float anglePressao = 0.f;

public:
    HidrometroGUI(Hidrometro& h)
        : window(sf::VideoMode({400u, 400u}), "Simulador Hidrometro"),
          hidr(h)
    {
        window.setFramerateLimit(60);

        // mostrador circular
        dial.setRadius(150.f);
        dial.setFillColor(sf::Color::White);
        dial.setOutlineThickness(5.f);
        dial.setOutlineColor(sf::Color::Black);
        dial.setPosition({50.f, 50.f});

        // ponteiro de volume (maior)
        needleVolume.setSize({100.f, 4.f});
        needleVolume.setFillColor(sf::Color::Red);
        needleVolume.setOrigin({0.f, 2.f});
        needleVolume.setPosition({200.f, 200.f});

        // ponteiro de pressão (menor)
        needlePressao.setSize({70.f, 3.f});
        needlePressao.setFillColor(sf::Color::Blue);
        needlePressao.setOrigin({0.f, 1.5f});
        needlePressao.setPosition({200.f, 200.f});
    }

    void run() {
        while (window.isOpen()) {
            // Loop de eventos
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
            }

            // Atualização do ponteiro
            double volume = hidr.getVolumeLitros();
            double pressao = hidr.getPressaoKpa();

            angleVolume = fmod(volume * 10.f, 360.f);
            anglePressao = fmod((pressao - 30.0) * 3.f, 360.f);

            needleVolume.setRotation(sf::degrees(angleVolume));
            needlePressao.setRotation(sf::degrees(anglePressao));

            // Renderização
            window.clear(sf::Color::Cyan);
            window.draw(dial);
            window.draw(needleVolume);
            window.draw(needlePressao);
            window.display();
    }
}

};
