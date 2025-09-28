#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "HidrometroImage.h"

#include <cmath>

HidrometroImage::HidrometroImage(Hidrometro& h, int w, int hgt) :
    hidrometro_ref(h), width(w), height(hgt) {

        // Inicializa o buffer de imagem da Raylib
        bufferImage = GenImageColor(width, height, BLACK);
        // Cria uma textura a partir da imagem, que será atualizada a cada quadro
        screenTexture = LoadTextureFromImage(bufferImage);
    }

HidrometroImage::~HidrometroImage() {
    UnloadImage(bufferImage);
    UnloadTexture(screenTexture);
}

void HidrometroImage::updateAndDraw() {
    // 1. Gera a noav imagem no buffer da CPU
    generateImageBuffer();

    // 2. Envia os novos dados de pixel para atextura na GPU
    UpdateTexture(screenTexture, bufferImage.data);

    // 3. Desenha na tela
    BeginDrawing();
    ClearBackground(RAYWHITE);  // Limpa a janela
    DrawTexture(screenTexture, 0, 0, WHITE);

    const char* volumeText = TextFormat("Volume: %.2f L", hidrometro_ref.getVolumeLitros());
    DrawText(volumeText, 20, 20, 24, BLACK);

    EndDrawing();
}

void HidrometroImage::generateImageBuffer() {
    Color background = {220, 220, 220, 255};
    Color pointerVol = {0, 200, 0, 255};
    Color pointerPrs = {0, 0, 200, 255};
    Color pointerVaz = {200, 0, 0, 255};

    int cx = width / 2;
    int cy = height / 2;
    int radius = 150;

    // Limpa a imagem (fundo)
    ImageClearBackground(&bufferImage, BLACK);

    // Círculo Principal
    ImageDrawCircle(&bufferImage, cx, cy, radius, background);

    float volumeLitros = hidrometro_ref.getVolumeLitros();
    float pressaoKpa = hidrometro_ref.getPressaoKpa();
    float vazaoMm = hidrometro_ref.getVazaoEntradaMm();

    // Calculo dos ângulos
    float angleVol = fmod(volumeLitros, 10.0) * 36.0f;
    float anglePres = (pressaoKpa / 500.0f) * 270.0f - 135.0f;  // Arco de 270 graus
    float angleVaz = (vazaoMm / 100.0f) * 360.0f;

    // Desenha as linhas dos ponteiros
    Vector2 endVol = { cx + sinf(DEG2RAD * angleVol) * (radius - 20), cy - cosf(DEG2RAD * angleVol) * (radius - 20) };
    ImageDrawLineV(&bufferImage, {(float)cx, (float)cy}, endVol, pointerVol);

    Vector2 endPres = { cx + sinf(DEG2RAD * anglePres) * (radius - 50), cy - cosf(DEG2RAD * anglePres) * (radius - 50) };
    ImageDrawLineV(&bufferImage, {(float)cx, (float)cy}, endPres, pointerPrs);

    Vector2 endVaz = { cx + sinf(DEG2RAD * angleVaz) * (radius - 80), cy - cosf(DEG2RAD * angleVaz) * (radius - 80) };
    ImageDrawLineV(&bufferImage, {(float)cx, (float)cy}, endVaz, pointerVaz);
}