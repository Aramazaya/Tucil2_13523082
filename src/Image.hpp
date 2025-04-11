#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
using namespace std;

struct RGB
{
    double r;
    double g;
    double b;
    int pixelCount;
    RGB(double r, double g, double b, int pxc) : r(r), g(g), b(b), pixelCount(pxc) {}
    RGB() : r(0), g(0), b(0), pixelCount(0) {}
};

class Block
{
private:
    int x;
    int y;
    int width;
    int height;
    int imgwidth;

public:
    // Constructor and Destructor
    Block(int x, int y, int width, int height, int imwidth) : x(x), y(y), width(width), height(height), imgwidth(imwidth) {}
    Block() : x(0), y(0), width(0), height(0), imgwidth() {}
    ~Block() {}
    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getImgWidth() const { return imgwidth; }
    // Setters
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setWidth(int width) { this->width = width; }
    void setHeight(int height) { this->height = height; }
    // Methods
    vector<Block> divideBlock();
    RGB getRGBAvg(const unsigned char *imageData, int channels) const;
};

#endif
