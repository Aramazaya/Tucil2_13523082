#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "Image.hpp"
#include <map>

class QuadTree {
private:
    Block info;
    int depth;
    QuadTree* UpLeft;
    QuadTree* UpRight;
    QuadTree* DownLeft;
    QuadTree* DownRight;
public:
    //Constructor and Destructor
    QuadTree(Block info, int depth) : info(info), depth(depth), UpLeft(nullptr), UpRight(nullptr), DownLeft(nullptr), DownRight(nullptr) {}
    QuadTree() : info(Block()), depth(0), UpLeft(nullptr), UpRight(nullptr), DownLeft(nullptr), DownRight(nullptr) {}
    ~QuadTree() {
        delete UpLeft;
        delete UpRight;
        delete DownLeft;
        delete DownRight;
    }
    //Getters
    Block getInfo() const { return info; }
    int getDepth() const { return depth; }
    QuadTree* getUpLeft() const { return UpLeft; }
    QuadTree* getUpRight() const { return UpRight; }
    QuadTree* getDownLeft() const { return DownLeft; }
    QuadTree* getDownRight() const { return DownRight; }
    //Setters
    void setInfo(Block info) { this->info = info; }
    //Methods
    void makeChild();
    double getRGBVariance(const unsigned char *imageData, int channels) const;
    double getMAD(const unsigned char *imageData, int channels) const;
    double getMPD(const unsigned char *imageData, int channels) const;
    double getEntrophy(const unsigned char *imageData, int channels) const;
};
#endif