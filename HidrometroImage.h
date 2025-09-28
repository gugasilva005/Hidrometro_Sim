#ifndef HIDROMETROIMAGE_H
#define HIDROMETROIMAGE_H

#include "Hidrometro.h"
#include "raylib.h"
#include <vector>
#include <string>


class HidrometroImage {
    public:
        HidrometroImage(Hidrometro& h, int width, int height);
        ~HidrometroImage();

        void updateAndDraw();
    
    private:
        void generateImageBuffer();

        Hidrometro& hidrometro_ref;
        int width, height;

        Image bufferImage;          // Imagem da Raylib (buffer de pixels na CPU)
        Texture2D screenTexture;    // Textura da Raylib 
};

#endif // HIDROMETROIMAGE_H