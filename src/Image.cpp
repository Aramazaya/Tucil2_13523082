#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_NO_EXIF
#include "stb_image.h"
#include "stb_image_write.h"
#include "Image.hpp"
//Divide blocks into 4 equal halfs. Returns array of Block
vector<Block> Block::divideBlock() {
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    int rightWidth = width - halfWidth;
    int bottomHeight = height - halfHeight;

    Block UpLeft(x, y, halfWidth, halfHeight, imgwidth);
    Block UpRight(x + halfWidth, y, rightWidth, halfHeight, imgwidth);
    Block DownLeft(x, y + halfHeight, halfWidth, bottomHeight, imgwidth);
    Block DownRight(x + halfWidth, y + halfHeight, rightWidth, bottomHeight, imgwidth);

    return { UpLeft, UpRight, DownLeft, DownRight };
}
//Get the average RGB value and the pixel count for the block
RGB Block::getRGBAvg(const unsigned char *imageData, int channels) const {
    double sumR = 0.0;
    double sumG = 0.0;
    double sumB = 0.0;
    int pixelCount = 0;
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int index = (i * imgwidth + j) * channels;
            sumR += imageData[index];
            sumG += imageData[index + 1];
            sumB += imageData[index + 2];
            pixelCount++;
        }
    }
    return RGB(sumR / pixelCount ,sumG / pixelCount, sumB / pixelCount, pixelCount);
}


