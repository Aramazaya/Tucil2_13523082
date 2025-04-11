#include "quadtree.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

void QuadTree::makeChild(){
    vector<Block> blocks = info.divideBlock();
    UpLeft = new QuadTree(blocks[0], depth + 1);
    UpRight = new QuadTree(blocks[1], depth + 1);
    DownLeft = new QuadTree(blocks[2], depth + 1);
    DownRight = new QuadTree(blocks[3], depth + 1);
}
double QuadTree::getRGBVariance(const unsigned char *imageData, int channels) const {
    auto avg = info.getRGBAvg(imageData, channels);
    double redSum = 0.0, greenSum = 0.0, blueSum = 0.0;
    for (int i = info.getY(); i < info.getY() + info.getHeight(); ++i) {
        for (int j = info.getX(); j < info.getX() + info.getWidth(); ++j) {
            int index = (i * info.getImgWidth() + j) * channels;
            redSum += imageData[index];
            greenSum += imageData[index + 1];
            blueSum += imageData[index + 2];
        }
    }
    redSum /= avg.pixelCount;
    greenSum /= avg.pixelCount;
    blueSum /= avg.pixelCount;
    for (int i = info.getY(); i < info.getY() + info.getHeight(); ++i) {
        for (int j = info.getX(); j < info.getX() + info.getWidth(); ++j) {
            int index = (i * info.getImgWidth() + j) * channels;
            double rDiff = imageData[index] - avg.r;
            double gDiff = imageData[index + 1] - avg.g;
            double bDiff = imageData[index + 2] - avg.b;
            
            redSum += rDiff * rDiff;
            greenSum += gDiff * gDiff;
            blueSum += bDiff * bDiff;
        }
    }
    
    double varianceR = redSum / avg.pixelCount;
    double varianceG = greenSum / avg.pixelCount;
    double varianceB = blueSum / avg.pixelCount;
    
    return (varianceR + varianceG + varianceB) / 3.0;
}
double QuadTree::getMAD(const unsigned char *imageData, int channels) const {
    RGB avg = info.getRGBAvg(imageData, channels);
    double redSum = 0.0;
    double greenSum = 0.0;
    double blueSum = 0.0;
    double mad = 0.0;
    for (int i = info.getY(); i < info.getY() + info.getHeight(); ++i) {
        for (int j = info.getX(); j < info.getX() + info.getWidth(); ++j) {
            int index = (i * info.getImgWidth() + j) * channels;
            redSum += abs(imageData[index] - avg.r);
            greenSum += abs(imageData[index + 1] - avg.g);
            blueSum += abs(imageData[index + 2] - avg.b);
        }
    }
    mad = ((redSum/avg.pixelCount) + (greenSum/avg.pixelCount) + (blueSum/avg.pixelCount))/3;
    return mad;
}
double QuadTree::getMPD(const unsigned char *imageData, int channels) const{
    double redMax = 0.0;
    double redMin = 255.0;
    double greenMax = 0.0;
    double greenMin = 255.0;
    double blueMax = 0.0;
    double blueMin = 255.0;
    double mpd = 0.0;
    for (int i = info.getY(); i < info.getY() + info.getHeight(); ++i) {
        for (int j = info.getX(); j < info.getX() + info.getWidth(); ++j) {
            int index = (i * info.getImgWidth() + j) * channels;
            if (imageData[index] > redMax) {
                redMax = imageData[index];
            }
            if (imageData[index] < redMin) {
                redMin = imageData[index];
            }
            if (imageData[index + 1] < greenMin) {
                greenMin = imageData[index + 1];
            }
            if (imageData[index + 1] > greenMax) {
                greenMax = imageData[index + 1];
            }
            if (imageData[index + 2] < blueMin) {
                blueMin = imageData[index + 2];
            }
            if (imageData[index + 2] > blueMax) {
                blueMax = imageData[index + 2];
            }
        }
    }
    mpd = ((redMax - redMin) + (greenMax - greenMin) + (blueMax - blueMin)) / 3;
    return mpd;
}
double QuadTree::getEntrophy(const unsigned char *imageData, int channels) const {
    map<int, int> RMap, GMap, BMap;
    int pixelCount = info.getWidth() * info.getHeight();
    double entrophyR = 0.0;
    double entrophyG = 0.0;
    double entrophyB = 0.0;
    double entrophy = 0.0;
    for (int i = 0; i < 256; ++i) {
        RMap[i] = 0;
        GMap[i] = 0;
        BMap[i] = 0;
    }
    for (int i = info.getY(); i < info.getY() + info.getHeight(); ++i) {
        for (int j = info.getX(); j < info.getX() + info.getWidth(); ++j) {
            int index = (i * info.getImgWidth() + j) * channels;
            RMap[imageData[index]]++;
            GMap[imageData[index + 1]]++;
            BMap[imageData[index + 2]]++;
        }
    }
    for (int i = 0; i < 256; ++i) {
        if (RMap[i] > 0) {
            double prob = static_cast<double>(RMap[i]) / pixelCount;
            entrophyR -= prob * log2(prob);
        }
        if (GMap[i] > 0) {
            double prob = static_cast<double>(GMap[i]) / pixelCount;
            entrophyG -= prob * log2(prob);
        }
        if (BMap[i] > 0) {
            double prob = static_cast<double>(BMap[i]) / pixelCount;
            entrophyB -= prob * log2(prob);
        }
    }
    entrophy = (entrophyR + entrophyG + entrophyB) / 3;
    return entrophy;
}

int QuadTree::getMaxDepth(QuadTree* node) const {
    if (!node) return 0;

    int maxChildDepth = 0;
    maxChildDepth = max(maxChildDepth, getMaxDepth(node->getUpLeft()));
    maxChildDepth = max(maxChildDepth, getMaxDepth(node->getUpRight()));
    maxChildDepth = max(maxChildDepth, getMaxDepth(node->getDownLeft()));
    maxChildDepth = max(maxChildDepth, getMaxDepth(node->getDownRight()));
    return maxChildDepth + 1;
}
