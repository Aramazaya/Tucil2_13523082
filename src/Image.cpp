#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.hpp"

vector<Block> Block::divideBlock() {
    // Divide the block into four smaller blocks
    int halfWidth = width / 2;
    int halfHeight = height / 2;

    Block UpLeft(x, y, halfWidth, halfHeight, imgwidth);
    Block UpRight(x + halfWidth, y, halfWidth, halfHeight, imgwidth);
    Block DownLeft(x, y + halfHeight, halfWidth, halfHeight, imgwidth);
    Block DownRight(x + halfWidth, y + halfHeight, halfWidth, halfHeight, imgwidth);

    return { UpLeft, UpRight, DownLeft, DownRight };
}

RGB Block::getRGBAvg(const unsigned char *imageData, int channels) const {
    long long sumR = 0;
    long long sumG = 0;
    long long sumB = 0;
    int pixelCount = 0;
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            cout<<"i: " << i << " j: " << j << endl;
            int index = (i * imgwidth + j) * channels;
            sumR += imageData[index];
            sumG += imageData[index + 1];
            sumB += imageData[index + 2];
            pixelCount++;
        }
    }
    return RGB(static_cast<unsigned char>(sumR / pixelCount),static_cast<unsigned char>(sumG / pixelCount),static_cast<unsigned char>(sumB / pixelCount), pixelCount);
}


