#include "main.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

unsigned char *init(int &width, int &height, int &channels, string &savePath, int &maxDepth, double &minBlockSize, int &method, string &imagePath) {
    cout << "Please enter the absolute path to the image file: ";
    getline(cin, imagePath);
    unsigned char *imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
    //cout << "Load failed reason: " << stbi_failure_reason() << endl;
    if (imageData == nullptr) {
        cout << "Error loading image" << endl;
        return nullptr;
    }
    
    cout << "Image data pointer: " << (void*)imageData << endl;
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

void processLeafNodes(QuadTree* root, unsigned char *outputData, unsigned char *imageData, int channels, int width) {
    RGB avg = root->getInfo().getRGBAvg(imageData, channels);
    for (int i = root->getInfo().getY(); i < root->getInfo().getY() + root->getInfo().getHeight(); ++i) {
        for (int j = root->getInfo().getX(); j < root->getInfo().getX() + root->getInfo().getWidth(); ++j) {
            int index = (i * root->getInfo().getImgWidth() + j) * channels;
            outputData[index] = avg.r;
            outputData[index + 1] = avg.g;
            outputData[index + 2] = avg.b;
            if (channels == 4){
                outputData[index + 3] = 255;
            }
        }
    }
}

void callVar(int &n_node, int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double variance = root->getRGBVariance(imageData, channels);
    if (variance < threshold){processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, imageData, channels, width);}
    else {
        root->makeChild();
        callVar(n_node, width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callVar(n_node, width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callVar(n_node, width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callVar(n_node, width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
}

void callMAD(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double MAD = root->getMAD(imageData, channels);
    if (MAD < threshold){processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, imageData, channels, width);}
    else {
        root->makeChild();
        callMAD(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callMAD(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callMAD(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callMAD(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    
}

void callMPD(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double MPD = root->getMPD(imageData, channels);
    if (MPD < threshold){processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getInfo().getWidth()*root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, imageData, channels, width);}
    else {
        root->makeChild();
        callMPD(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callMPD(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callMPD(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callMPD(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    
}

void callEntrophy(int width, unsigned char *outputData, unsigned char *imageData, int channels, double threshold, QuadTree* root, int maxDepth, int minBlockSize) {
    double MRD = root->getEntrophy(imageData, channels);
    if (MRD < threshold){processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getDepth() >= maxDepth) {processLeafNodes(root, outputData, imageData, channels, width);}
    else if (root->getInfo().getWidth() < minBlockSize || root->getInfo().getHeight() < minBlockSize) {processLeafNodes(root, outputData, imageData, channels, width);}
    else {
        root->makeChild();
        callEntrophy(width, outputData, imageData, channels, threshold, root->getUpLeft(), maxDepth, minBlockSize);
        callEntrophy(width, outputData, imageData, channels, threshold, root->getUpRight(), maxDepth, minBlockSize);
        callEntrophy(width, outputData, imageData, channels, threshold, root->getDownLeft(), maxDepth, minBlockSize);
        callEntrophy(width, outputData, imageData, channels, threshold, root->getDownRight(), maxDepth, minBlockSize);
    }
    
}

void saveImage(unsigned char *outputData, int width, int height, int channels, const string& savePath) {
    string ext = savePath.substr(savePath.find_last_of(".") + 1);
    bool success = false;
    
    if (ext == "png" || ext == "PNG") {
        success = stbi_write_png(savePath.c_str(), width, height, channels, outputData, width * channels);
    } 
    else if (ext == "jpg" || ext == "jpeg" || ext == "JPG" || ext == "JPEG") {
        int quality = 50;
        success = stbi_write_jpg(savePath.c_str(), width, height, channels, outputData, quality);
    }
    else if (ext == "bmp" || ext == "BMP") {
        success = stbi_write_bmp(savePath.c_str(), width, height, channels, outputData);
    }
    else if (ext == "tga" || ext == "TGA") {
        success = stbi_write_tga(savePath.c_str(), width, height, channels, outputData);
    }
    if (success) {
        cout << "Image saved successfully to " << savePath << endl;
    } else {
        cout << "Error saving image to " << savePath << endl;
    }
}

chrono::_V2::system_clock::time_point run(int &n_node, unsigned char *outputData, QuadTree* root, int method, int width, int height, int channels, unsigned char *imageData, int maxDepth, double minBlockSize){
    double threshold;
    chrono::_V2::system_clock::time_point startTime;
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
            startTime = chrono::high_resolution_clock::now();
            callVar(n_node, width, outputData, imageData, channels, threshold, root, maxDepth, minBlockSize);
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
            startTime = chrono::high_resolution_clock::now();
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
            startTime = chrono::high_resolution_clock::now();
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
            startTime = chrono::high_resolution_clock::now();
            callEntrophy(width, outputData, imageData, channels, threshold, root, maxDepth, minBlockSize);
            break;
        default:
            startTime = chrono::high_resolution_clock::now();
            cout << "Invalid method selected" << endl;
            break;
    }
    stbi_image_free(imageData);
    return startTime;
}

int main(){
    cout << "WELCOME TO THE QUADTREE IMAGE COMPRESSION PROGRAM" << endl;
    int width, height, channels, maxDepth,  method;
    double minBlockSize;
    string savePath, imagePath;
    unsigned char *imageData = init(width, height, channels, savePath, maxDepth, minBlockSize, method, imagePath);
    if (imageData == nullptr) {
        cout << "Closing.." << endl;
        return 0;
    }
    unsigned char *outputData = new unsigned char[width * height * channels];
    QuadTree* root = new QuadTree(Block(0, 0, width, height, width), 0);
    while (true) {
        int n_node = 0;
        auto startTime = run(n_node, outputData, root, method, width, height, channels, imageData, maxDepth, minBlockSize);
        auto endTimeNoSave = chrono::high_resolution_clock::now();
        saveImage(outputData, width, height, channels, savePath);
        auto endTimeWSave = chrono::high_resolution_clock::now();
        auto durationNoSave = chrono::duration_cast<chrono::milliseconds>(endTimeNoSave - startTime);
        auto durationWSave = chrono::duration_cast<chrono::milliseconds>(endTimeWSave - startTime);
        cout << "Number of nodes: " << root->n_node << endl;
        cout << "Max depth: " << root->max_depth << endl;
        cout << "Execution time without saving: " << durationNoSave.count() << " ms" << endl;
        cout << "Execution time with saving: " << durationWSave.count() << " ms" << endl;
        uintmax_t fileSizeInit;
        uintmax_t fileSizeEnd;
        ifstream file(imagePath, ios::binary | ios::ate);
        if (!file.is_open()) fileSizeInit = 0;
        else fileSizeInit = file.tellg()/1000;
        file.close();
        ifstream file2(savePath, ios::binary | ios::ate);
        if (!file2.is_open()) fileSizeEnd = 0;
        else fileSizeEnd = file2.tellg()/1000;
        file2.close();
        double compressionRatio = 100.0 * (1.0 - static_cast<double>(fileSizeEnd) / fileSizeInit);
        cout << "Original file size: " << fileSizeInit << " KB" << endl;
        cout << "Compressed file size: " << fileSizeEnd << " KB" << endl;
        cout << "Compression ratio: " << compressionRatio << "%" << endl;
        cout << "Do you want to run the program again? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            delete[] outputData;
            imageData = init(width, height, channels, savePath, maxDepth, minBlockSize, method, imagePath);
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