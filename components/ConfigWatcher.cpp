#include "ConfigWatcher.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>

ConfigWatcher::ConfigWatcher(const std::string& path, int interval_seconds) :
    filepath(path), interval(interval_seconds), running(false) {}

ConfigWatcher::~ConfigWatcher() { stop(); }

void ConfigWatcher::start() {
    bool expected = false;
    if (running.compare_exchange_strong(expected, true)) th = std::thread(&ConfigWatcher::loop, this);
}

void ConfigWatcher::stop() {
    bool expected = true;
    if (running.compare_exchange_strong(expected, false)) {
        if (th.joinable()) th.join();
    }
}

void ConfigWatcher::setCallback(ConfigCallback c) { cb = c; }

void ConfigWatcher::loop() {
    while(running) {
        HidroConfig cfg;
        std::ifstream ifs(filepath);
        if(ifs) {
            std::string line;
            while(std::getline(ifs, line)) {
                if(line.empty()) continue;
                if(line[0] == '#') continue;

                auto eq = line.find('=');
                if(eq == std::string::npos) continue;

                std::string key = line.substr(0, eq);
                std::string val = line.substr(eq + 1);

                auto trim = [](std::string &t){
                    size_t a = t.find_first_not_of(" \t\r\n");
                    if (a == std::string::npos) { t.clear(); return; }
                    size_t b = t.find_last_not_of(" \t\r\n");
                    t = t.substr(a, b - a + 1);
                };

                trim(key);
                trim(val);

                try {
                    if (key == "vazao_mm") cfg.vazao_mm = std::stod(val);
                    else if (key == "area_m2") cfg.area_m2 = std::stod(val);
                } catch(...) { /* ignora parse errors */ }
            }
        }
        if (cb) cb(cfg);
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}