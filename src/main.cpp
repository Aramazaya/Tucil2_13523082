#include "main.hpp"

unsigned char *init(int &width, int &height, int &channels, string &savePath, int &maxDepth, double &minBlockSize, int &method) {
    cout << "Please enter the absolute path to the image file: ";
    string imagePath;
    cin >> imagePath;
    const char* charPtr = imagePath.c_str();
    unsigned char *imageData = stbi_load(charPtr, &width, &height, &channels, 0);
    //cout << "Load failed reason: " << stbi_failure_reason() << endl;
    if (imageData = nullptr) {
        cout << "Error loading image" << endl;
        return nullptr;
    }
    
    std::cout << "Image data pointer: " << (void*)imageData << std::endl;
    cout << "Image loaded successfully" << endl;
    cout << "Image width: " << width << endl;
    cout << "Image height: " << height << endl;
    cout << "Please enter the absolute path to save the compressed image: ";
    cin >> savePath;
    cout << "Please enter the maximum depth of the quadtree: ";
    cin >> maxDepth;
    cout << "Please enter the minimum block size: ";
    cin >> minBlockSize;
    cout << "Please enter the compression method (1: RGB Variance, 2: MAD, 3: MPD, 4: Entropy): ";
    cin >> method;
    return imageData;
}

void processLeafNodes(QuadTree* root, unsigned char *outputData, int channels, int width) {
    RGB avg = root->getInfo().getRGBAvg(outputData, channels);
    for (int i = root->getInfo().getY(); i < root->getInfo().getY() + root->getInfo().getHeight(); ++i) {
        for (int j = root->getInfo().getX(); j < root->getInfo().getX() + root->getInfo().getWidth(); ++j) {
            int index = (i * root->getInfo().getWidth() + j) * channels;
            outputData[index] = avg.r;
            outputData[index + 1] = avg.g;
            outputData[index + 2] = avg.b;
            if (channels == 4){
                outputData[index + 3] = 255;
            }
        }
    }
}

void callVar(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double variance = root->getRGBVariance(imageData, channels);
    if (variance < threshold){processLeafNodes(root, outputData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, channels, width);}
    else {
        root->makeChild();
        callVar(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callVar(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callVar(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callVar(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    delete root;
}

void callMAD(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double MAD = root->getMAD(imageData, channels);
    if (MAD < threshold){processLeafNodes(root, outputData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, channels, width);}
    else {
        root->makeChild();
        callMAD(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callMAD(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callMAD(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callMAD(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    delete root;
}

void callMPD(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double MPD = root->getMPD(imageData, channels);
    if (MPD < threshold){processLeafNodes(root, outputData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, channels, width);}
    else {
        root->makeChild();
        callMPD(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callMPD(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callMPD(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callMPD(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    delete root;
}

void callEntrophy(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double MRD = root->getEntrophy(imageData, channels);
    if (MRD < threshold){processLeafNodes(root, outputData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, channels, width);}
    else {
        root->makeChild();
        callEntrophy(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callEntrophy(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callEntrophy(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callEntrophy(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    delete root;
}

void saveImage(unsigned char *outputData, int width, int height, int channels, const string& savePath) {
    if (stbi_write_png(savePath.c_str(), width, height, channels, outputData, width * channels)) {
        cout << "Image saved successfully" << endl;
    } else {
        cout << "Error saving image" << endl;
    }
}

void run(unsigned char *outputData, QuadTree* root, int method, int width, int height, int channels, unsigned char *imageData, int maxDepth, double minBlockSize){
    double threshold;
    switch (method) {
        case 1:
            while (true) {
                cout << "Please enter the threshold for RGB variance: ";
                cin >> threshold;
                if (cin.fail()) {
                    cout << "Error: Please enter a numeric value.\n";
                    
                    cin.clear();
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                if (threshold > 0) {
                    break;
                } else {
                    cout << "Invalid threshold. Please enter a value above 0." << endl;
                }
            }
            cout << "Threshold: " << threshold << endl;
            cout << "Width: " << width << endl;
            cout << "Height: " << height << endl;
            cout << "Channels: " << channels << endl;
            cout << "Max Depth: " << maxDepth << endl;
            cout << "Min Block Size: " << minBlockSize << endl;
            cout << "Image Data: " << (void*)imageData << endl;
            callVar(width, outputData, imageData, channels, threshold, root, maxDepth, minBlockSize);
            break;
        case 2:
            while (true) {
                cout << "Please enter the threshold for Mean Absolute Deviation: ";
                cin >> threshold;
                if (cin.fail()) {
                    cout << "Error: Please enter a numeric value.\n";
                    
                    cin.clear();
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                if (threshold > 0 && threshold <= 127.5) {
                    break;
                } else {
                    cout << "Invalid threshold. Please enter a value between 0 and 127.5." << endl;
                }
            }
            callMAD(width, outputData, imageData, channels, threshold, root, maxDepth, minBlockSize);
            break;
        case 3:
            while (true) {
                cout << "Please enter the threshold for Maximum Pixel Difference: ";
                cin >> threshold;
                if (cin.fail()) {
                    cout << "Error: Please enter a numeric value.\n";
                    
                    cin.clear();
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                if (threshold > 0 && threshold <= 255) {
                    break;
                } else {
                    cout << "Invalid threshold. Please enter a value between 0 and 255." << endl;
                }
            }
            callMPD(width, outputData, imageData, channels, threshold, root, maxDepth, minBlockSize);
            break;
        case 4:
            while (true) {
                cout << "Please enter the threshold for Entrophy: ";
                cin >> threshold;
                if (cin.fail()) {
                    cout << "Error: Please enter a numeric value." << endl;
                    
                    cin.clear();
                    
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                if (threshold > 0 && threshold <= 8) {
                    break;
                } else {
                    cout << "Invalid threshold. Please enter a value between 0 and 8." << endl;
                }
            }
            callEntrophy(width, outputData, imageData, channels, threshold, root, maxDepth, minBlockSize);
            break;
        default:
            cout << "Invalid method selected" << endl;
            break;
    }
    delete root;
    stbi_image_free(imageData);
}

int main(){
    cout << "WELCOME TO THE QUADTREE IMAGE COMPRESSION PROGRAM" << endl;
    int width, height, channels, maxDepth,  method;
    double minBlockSize;
    string savePath;
    cout << "Please enter the absolute path to the image file: ";
    string imagePath;
    cin >> imagePath;
    const char* charPtr = imagePath.c_str();
    unsigned char *imageData = stbi_load(charPtr, &width, &height, &channels, 0);
    //cout << "Load failed reason: " << stbi_failure_reason() << endl;
    if (imageData = nullptr) {
        cout << "Error loading image" << endl;
    }
    std::cout << "Image data pointer: " << (void*)imageData << std::endl;
    cout << "Image loaded successfully" << endl;
    cout << "Image width: " << width << endl;
    cout << "Image height: " << height << endl;
    cout << "Please enter the absolute path to save the compressed image: ";
    cin >> savePath;
    cout << "Please enter the maximum depth of the quadtree: ";
    cin >> maxDepth;
    cout << "Please enter the minimum block size: ";
    cin >> minBlockSize;
    cout << "Please enter the compression method (1: RGB Variance, 2: MAD, 3: MPD, 4: Entropy): ";
    cin >> method;
    //unsigned char *imageData = init(width, height, channels, savePath, maxDepth, minBlockSize, method);
    if (imageData = nullptr) {
        cout << "Closing.." << endl;
        return 0;
    }
    unsigned char *outputData = new unsigned char[width * height * channels];
    QuadTree* root = new QuadTree(Block(0, 0, width, height, width), 0);
    while (true) {
        run(outputData, root, method, width, height, channels, imageData, maxDepth, minBlockSize);
        saveImage(outputData, width, height, channels, savePath);
        cout << "Do you want to run the program again? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            delete[] outputData;
            delete root;
            imageData = init(width, height, channels, savePath, maxDepth, minBlockSize, method);
            if (imageData = nullptr) {
                cout << "Closing.." << endl;
                return 0;
            }
            outputData = new unsigned char[width * height * channels];
            root = new QuadTree(Block(0, 0, width, height, width), 0);
        } else {
            break;
        }
    }
}