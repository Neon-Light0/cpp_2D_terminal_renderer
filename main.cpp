#include <iostream>
#include <stdlib.h>
#include <thread>
#include <chrono> 
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

const int width = 100;
const int height = 24;
const int fps = 60;
const int delta = 1000 / fps;
char* frameBuffer = new char[width * height];

struct Object{
    char* objectBuffer = nullptr;
    int width = 0;
    int height = 0;

    Object(int width, int height) {
        objectBuffer = new char[width * height];
        this->width = width;
        this->height = height;
        std::fill_n(objectBuffer, width * height, ' ');
    }

    ~Object(){
        if (objectBuffer != nullptr){
            delete[] objectBuffer;
        }
    }

    void printObject(){
        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                std::cout << objectBuffer[i * width + j];
            }
            
            std::cout << "|" << std::endl;
        }
    }
};

void clearScreen(){
    printf("\x1b[%dD", width);
    printf("\x1b[%dA", height);
}

void resetCursor(){
    printf("\x1b[%dC", width);
    printf("\x1b[%dB", height);
}

void render(){
    char row[width];

    // generate frame buffer
    for (int i = 0 ; i < height; i++){
        for (int j = 0; j < width; j++){
            row[j] = frameBuffer[i * width + j];
        }

        fwrite(row, 1, width, stdout);
        fputc('\n', stdout);
    }

    clearScreen();
}

Object* loadObject(std::string path){
    std::ifstream file(path);
    
    if (!file.is_open()){
        return nullptr;
    }

    int minC = INT_MAX;
    int minR = INT_MAX;
    int maxC = INT_MIN;
    int maxR = INT_MIN;
    int r = 0;
    std::string line;

    while (std::getline(file, line)){
        for (int c = 0; c < line.size(); c++){
            char& val = line[c];

            if (!std::isspace(val)){
                minC = std::min(minC, c);
                minR = std::min(minR, r);
                maxC = std::max(maxC, c);
                maxR = std::max(maxR, r);
            }
        }

        r++;
    }

    file.clear();
    file.seekg(0, std::ios::beg);
    r = 0;
    line.clear();

    Object* obj = new Object(maxC - minC + 1, maxR - minR + 1);

    while (std::getline(file, line)){
        for (int c = 0; c < line.size(); c++){
            if (c >= obj->width || r >= obj->height){
                break;
            }

            if (c >= minC && c <= maxC && r >= minR && r <= maxR){
                obj->objectBuffer[(r - minR) * obj->width + (c - minC)] = line[c];
            } 
        }

        r++;
    }

    return obj;
}

void drawObject(Object* obj, int x, int y){
    for (int i = 0; i < width * height; i++){
        frameBuffer[i] = ' ';
    }

    for (int i = 0; i < obj->height; i++){
        for (int j = 0; j < obj->width; j++){
            frameBuffer[(y + i) * width + (x + j)] = obj->objectBuffer[i * obj->width + j];
        }
    }
}

int main(int argc, char *argv[]){   
    printf("\x1b[?25l");    // turn off cursor
    std::flush(std::cout);

    Object* obj = loadObject("object.txt");

    if (obj == nullptr){
        std::cout << "Failed to load object" << std::endl;
        return 1;
    } 

    for (int i = 0; i < 20; i++){
        drawObject(obj, i, 5);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(delta));     
    }

    resetCursor();
    printf("\x1b[?25h"); // turn it back on
    std::cout << std::endl;

    delete obj;
    delete[] frameBuffer;
    return 0;
}