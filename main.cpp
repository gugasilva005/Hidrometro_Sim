#include "raylib.h"
#include "Hidrometro.h" // Sua classe de simulação multi-thread
#include <string>
#include <cmath>     // Para as funções matemáticas como fmod() e pow()

//================================================================================
// FUNÇÕES AUXILIARES DE DESENHO (sem alterações)
//================================================================================

void DrawOdometer(Vector2 position, double value, int numDigits, Vector2 digitSize) {
    int fontSize = digitSize.y * 0.8f;

    for (int i = 0; i < numDigits; i++) {
        float digitX = position.x + (numDigits - 1 - i) * digitSize.x;
        Rectangle scissorArea = { digitX, position.y, digitSize.x, digitSize.y };

        DrawRectangleRec(scissorArea, WHITE);
        DrawRectangleLinesEx(scissorArea, 1, BLACK);

        BeginScissorMode(scissorArea.x, scissorArea.y, scissorArea.width, scissorArea.height);
        
        double placeValue = pow(10, i);
        double digitValue = fmod(value / placeValue, 10.0);
        
        int currentDigit = (int)digitValue;
        float fraction = digitValue - currentDigit;

        float yOffset = fraction * digitSize.y;

        int prevDigit = (currentDigit - 1 + 10) % 10;
        int nextDigit = (currentDigit + 1) % 10;

        const char* prevText = TextFormat("%d", prevDigit);
        const char* currText = TextFormat("%d", currentDigit);
        const char* nextText = TextFormat("%d", nextDigit);
        
        int prevWidth = MeasureText(prevText, fontSize);
        int currWidth = MeasureText(currText, fontSize);
        int nextWidth = MeasureText(nextText, fontSize);

        DrawText(prevText, digitX + (digitSize.x - prevWidth)/2, position.y - yOffset + (digitSize.y - fontSize)/2 - digitSize.y, fontSize, BLACK);
        DrawText(currText, digitX + (digitSize.x - currWidth)/2, position.y - yOffset + (digitSize.y - fontSize)/2, fontSize, BLACK);
        DrawText(nextText, digitX + (digitSize.x - nextWidth)/2, position.y - yOffset + (digitSize.y - fontSize)/2 + digitSize.y, fontSize, BLACK);

        EndScissorMode();
    }
}

void DrawAnalogDial(Vector2 center, float radius, float value, const char* title, Color pointerColor) {
    DrawCircleLines(center.x, center.y, radius, LIGHTGRAY);

    DrawText("0", center.x - 5, center.y - radius - 20, 10, BLACK);

    float angle = value * 360.0f;
    Vector2 endPoint = {
        center.x + sinf(DEG2RAD * angle) * (radius * 0.9f),
        center.y - cosf(DEG2RAD * angle) * (radius * 0.9f)
    };
    DrawLineEx(center, endPoint, 3.0f, pointerColor);
    DrawCircle(center.x, center.y, 4, pointerColor);
    
    int titleWidth = MeasureText(title, 20);
    DrawText(title, center.x - titleWidth / 2, center.y + radius + 15, 20, RAYWHITE);
}


//================================================================================
// FUNÇÃO PRINCIPAL
//================================================================================

int main(void) {
    // --- 1. INICIALIZAÇÃO ---
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Simulador de Hidrômetro");
    SetTargetFPS(60);

    std::string configFile = "config.txt"; 
    Hidrometro hidrometro(configFile);
    hidrometro.start();

    // Variáveis para o controle suave da vazão
    float targetVazao = 0.0f;
    float currentVazao = 0.0f;
    const float VAZAO_CHANGE_SPEED = 10.0f;
    const float VAZAO_SMOOTHING_FACTOR = 5.0f;

    // --- 2. LOOP PRINCIPAL DA APLICAÇÃO ---
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- 2.1. ENTRADA DO USUÁRIO ---
        if (IsKeyDown(KEY_UP)) {
            targetVazao += VAZAO_CHANGE_SPEED * dt;
        }
        if (IsKeyDown(KEY_DOWN)) {
            targetVazao -= VAZAO_CHANGE_SPEED * dt;
            if (targetVazao < 0) targetVazao = 0;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            targetVazao = 0.0f;
        }
        if (IsKeyPressed(KEY_R)) {
            hidrometro.reset();
        }

        currentVazao += (targetVazao - currentVazao) * VAZAO_SMOOTHING_FACTOR * dt;
        if (fabs(targetVazao - currentVazao) < 0.01f) {
            currentVazao = targetVazao;
        }
        hidrometro.setVazaoEntradaMm(currentVazao);

        // --- 2.2. LER DADOS DA SIMULAÇÃO ---
        double volume = hidrometro.getVolumeLitros();
        double pressao = hidrometro.getPressaoKpa();
        double vazao_real = hidrometro.getVazaoEntradaMm();

        // --- 2.3. DESENHO (RENDERIZAÇÃO) ---
        BeginDrawing();
        ClearBackground((Color){ 10, 20, 30, 255 });

        Vector2 center = { (float)screenWidth / 2, (float)screenHeight / 2 };

        // --- MOSTRADOR PRINCIPAL DE VOLUME (ODÔMETRO) ---
        int numInteiros = 6;
        Vector2 odoSize = { 50, 70 };
        Vector2 odoPosition = { center.x - (numInteiros * odoSize.x) / 2, 100 };
        DrawText("Volume Consumido", odoPosition.x, odoPosition.y - 30, 20, RAYWHITE);
        DrawOdometer(odoPosition, volume, numInteiros, odoSize);
        DrawText("m³", odoPosition.x + (numInteiros * odoSize.x) + 10, odoPosition.y + odoSize.y - 25, 30, RAYWHITE);


        // --- NOVOS MOSTRADORES PARA VAZÃO E PRESSÃO ---
        Vector2 pressaoDialPos = { (float)screenWidth / 4, 350 };
        Vector2 vazaoDialPos = { (float)screenWidth * 3 / 4, 350 };
        float dialRadius = 100.0f;

        // Normaliza os valores para o ponteiro (0.0 a 1.0)
        float pressaoNormalizada = pressao / 150.0f; // Supõe que a pressão máxima do mostrador é 150 kPa
        float vazaoNormalizada = vazao_real / 100.0f; // Supõe que a vazão máxima do mostrador é 100 mm

        if (pressaoNormalizada < 0) pressaoNormalizada = 0;
        if (pressaoNormalizada > 1) pressaoNormalizada = 1;
        if (vazaoNormalizada < 0) vazaoNormalizada = 0;
        if (vazaoNormalizada > 1) vazaoNormalizada = 1;

        // Desenha os dois novos relógios
        DrawAnalogDial(pressaoDialPos, dialRadius, pressaoNormalizada, "Pressão (kPa)", BLUE);
        DrawAnalogDial(vazaoDialPos, dialRadius, vazaoNormalizada, "Vazão (mm)", GREEN);


        // --- REMOVIDO: Código dos 4 mostradores fracionários ---


        // Exibe os valores exatos para referência
        DrawText(TextFormat("Volume: %.3f m³", volume), 10, 10, 20, LIGHTGRAY);
        DrawText(TextFormat("Vazão: %.2f mm", vazao_real), 10, 35, 20, LIGHTGRAY);
        DrawText(TextFormat("Pressão: %.1f kPa", pressao), 10, 60, 20, LIGHTGRAY);

        // Instruções de uso
        DrawText("Use SETAS para controlar | ESPAÇO para zerar | R para resetar", 10, screenHeight - 30, 20, LIGHTGRAY);

        EndDrawing();
    }

    // --- 3. FINALIZAÇÃO ---
    hidrometro.stop();
    CloseWindow();

    return 0;
}