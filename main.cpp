#include <iostream>
#include <stdlib.h>
#include <thread>
#include <chrono> 
#include <string>
#include <vector>

const int width = 40;
const int height = 20;
const int fps = 60;
const int mspf = 1000 / fps;
char* frameBuffer = static_cast<char*>(malloc(width * height * sizeof(char)));

void clearScreen(){
    //printf("\x1b[H");

    printf("\x1b[%dD", width);
    printf("\x1b[%dA", height);
}

void resetCursor(){
    printf("\x1b[%dC", width);
    printf("\x1b[%dB", height);
}

void render(char val){

    char row[width + 1];
    row[width] = '\0';

    // generate frame buffer
    for (int i = 0 ; i < height; i++){
        for (int j = 0; j < width; j++){
            row[j] = val;
        }

        row[width] = '\0';
        printf("%s", row);
        printf("\n");
    }

    clearScreen();
}

int main(int argc, char *argv[])
{   
    printf("\x1b[?25l");    // turn off cursor
    std::flush(std::cout);

    for (int i = 50; i < 70; i++){
        render(static_cast<char>(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(mspf));     
    }

    resetCursor();
    printf("\x1b[?25h"); // turn it back on

    std::cout << std::endl;
    return 0;
}