classDiagram
    direction LR

    class Hidrometro {
        -VazaoEntrada vazao
        -SensorPressao pressao
        -Contador contador
        -ConfigWatcher configWatcher
        -thread update_thread
        -atomic bool running
        -mutex cb_mutex
        -ReadingCallback cb
        -atomic double area_m2
        +Hidrometro(const string& config_path)
        +~Hidrometro()
        +start() void
        +stop() void
        +setVazaoEntradaMm(double mm) void
        +getVazaoEntradaMm() double
        +getVolumeLitros() double
        +getPressaoKpa() double
        +reset() void
        +setReadingCallback(ReadingCallback cb) void
        -updateLoop() void
        -applyConfig(const HidroConfig& cfg) void
    }

    class VazaoEntrada {
        -atomic double vazao_mm
        +VazaoEntrada(double vazao_inicial)
        +setVazaoMm(double mm) void
        +getVazaoMm() double
        +getLitrosPorSegundo(double area_m2) double
    }

    class SensorPressao {
        -atomic double pressao_kpa
        +SensorPressao(double pressao_inicial)
        +setPressaoKpa(double kpa) void
        +getPressaoKpa() double
    }

    class Contador {
        -atomic double volume_litros
        +Contador()
        +adicionarLitros(double litros) void
        +getVolumeLitros() double
        +reset() void
    }

    class ConfigWatcher {
        -string config_path
        -int interval_s
        -thread watcher_thread
        -atomic bool watching
        -ConfigCallback callback
        +ConfigWatcher(const string& path, int interval)
        +~ConfigWatcher()
        +start() void
        +stop() void
        +setCallback(ConfigCallback cb) void
        -watchLoop() void
        -readConfig() HidroConfig
    }

    class HidroConfig {
        <<struct>>
        +double vazao_mm
        +double area_m2
    }

    Hidrometro *-- "1" VazaoEntrada : contém
    Hidrometro *-- "1" SensorPressao : contém
    Hidrometro *-- "1" Contador : contém
    Hidrometro *-- "1" ConfigWatcher : contém

    ConfigWatcher ..> HidroConfig : utiliza
    Hidrometro ..> HidroConfig : utiliza