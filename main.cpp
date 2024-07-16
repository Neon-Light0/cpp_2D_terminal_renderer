#include <iostream>
#include <stdlib.h>
#include <thread>
#include <chrono> 
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

////<Settings>//////////
const int width = 100;
const int height = 24;
const int fps = 60;
const int delta = 1000 / fps;
const double gravity = 9.8;
const double objPosX = 50;
const double objPosY = 12;
const double objVelX = 100;
const double objVelY = 50;
char* frameBuffer = new char[width * height];
////////////////////////

struct Object{
    char* objectBuffer = nullptr;
    int width = 0;
    int height = 0;
    double x = 0;
    double y = 0;
    double vX = 0;
    double vY = 0;

    Object(int width, int height) {
        objectBuffer = new char[width * height];
        this->width = width;
        this->height = height;
        std::fill_n(objectBuffer, width * height, ' ');
    }

    Object(int width, int height, int x, int y, int vX, int vY) : Object(width, height){
        this->x = x;
        this->y = y;
        this->vX = vX;
        this->vY = vY;
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

void clearFrameBuffer(){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1){
                frameBuffer[i * width + j] = '#';
                continue;
            }
            int index = i * width + j;
            frameBuffer[index] = ' ';
        }
    }
}

void drawObject(Object* obj){

    for (int i = 0; i < obj->height; i++){
        for (int j = 0; j < obj->width; j++){
            char& val = obj->objectBuffer[i * obj->width + j];
            if (val == ' ') continue;
            int pixelX = static_cast<int>(obj->x);
            int pixelY = static_cast<int>(obj->y);
            frameBuffer[(pixelY + i) * width + (pixelX + j)] = val;
        }
    }
}

void updateObject(Object* obj){
    double deltaSeconds = delta / 1000.0;
    obj->x = obj->x + obj->vX * deltaSeconds;
    obj->y = obj->y + (obj->vY * deltaSeconds + 0.5 * gravity * deltaSeconds * deltaSeconds);

    if (obj->x < 1){
        obj->vX = -obj->vX;
        obj->x = 1;
    } else if (obj->x + obj->width >= width - 1){
        obj->vX = -obj->vX;
        obj->x = width - obj->width - 1;
    }

    if (obj->y < 1){
        obj->vY = -obj->vY;
        obj->y = 1;
    } else if (obj->y + obj->height >= height - 1){
        obj->vY = -obj->vY;
        obj->y = height - obj->height - 1;
    }
}

int main(int argc, char *argv[]){   
    printf("\x1b[?25l");    // turn off cursor
    std::flush(std::cout);

    Object* obj = loadObject("object.txt");
    obj->x = objPosX;
    obj->y = objPosY;
    obj->vX = objVelX;
    obj->vY = objVelY;

    if (obj == nullptr){
        std::cout << "Failed to load object" << std::endl;
        return 1;
    } 

    while (true){
        clearFrameBuffer();
        updateObject(obj);
        drawObject(obj);
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