#ifndef CONFIGWATCHER_H
#define CONFIGWATCHER_H

#include <string>
#include <functional>
#include <thread>
#include <atomic>

struct HidroConfig {
    double vazao_mm = 0.0;  // valor padrão
    double area_m2 = 0.005; // área do cano usada na conversão de mm para litros
};

using ConfigCallback = std::function<void(const HidroConfig&)>;

class ConfigWatcher {
    public:
        ConfigWatcher(const std::string& path, int interval_seconds = 5);
        ~ConfigWatcher();
        
        void start();
        void stop();
        void setCallback(ConfigCallback cb);
    
    private:
        void loop();
        std::string filepath;
        int interval;
        std::atomic<bool> running;
        std::thread th;
        ConfigCallback cb;
};

#endif // CONFIGWATCHER_H