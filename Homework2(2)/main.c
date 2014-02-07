//
//  main.c
//  Homework2(2)
//
//  Created by Ralf Cheung on 9/27/13.
//  Copyright (c) 2013 Ralf Cheung. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char byte;

#define starsHeight 768
#define starsWidth 1024
#define digitsHeight 959
#define digitsWidth 1231
#define dinosaurHeight 444
#define dinosaurWidth 960
#define pacmanHeight 441
#define grayScaleChannel 1
#define channel 3

byte *copyImage(byte *image, int imgWidth, int imgHeight, int channels);
void printMatrix(int matrix[3][3]);
byte *readFile(char *fileName, int width, int height);
byte* convertToGrayScale(byte *image, int width, int height);
void exportFile(char *fileName, byte *image, int offset, int width, int height);
byte *getPixel(byte *image, int width, int i, int j, int channels);
byte *expandImage(byte *image, int height, int width);
void countStars(byte *image, int width, int height);
void histogram(char *filename, byte *image, int channels, int w, int h);
int unconditionalFilter(int a[3][3]);
int shrinkFilter(int a);
void countNumbers(byte *image);
int thinningConditional(int matrix);
int unconditionalSkeletonize(int matrix[3][3]);
void dinosaur(byte *image);
int conditionalSkeletonize(int a);

byte* displayWall(byte *image, int width, int height);
void pacmanCount(byte *image, int height, int width);
void doWall(byte *image, int height, int width);
byte* displayPoints(byte *image, int width, int height);
void doPoints(byte *image, int height, int width);




int main(int argc, const char * argv[])
{
    
    byte *star = readFile("stars.raw", starsWidth, starsHeight);
    countStars(star, starsWidth, starsHeight);

    byte *numbers = readFile("digits.raw", digitsWidth, digitsHeight);
    countNumbers(numbers);
//
    byte *dino = readFile("dinosaur.raw", dinosaurWidth, dinosaurHeight);
    dinosaur(dino);
    
    byte *pacman = readFile("pacman.raw", pacmanHeight, pacmanHeight);
    pacmanCount(pacman, pacmanHeight, pacmanHeight);
//
//    
//    byte *T = readFile("T.raw", dinosaurWidth, dinosaurHeight);
//    dinosaur(T);
    
    return 0;
}


void pacmanCount(byte *image, int height, int width){
    
//    
    byte *wall = copyImage(image, pacmanHeight, pacmanHeight, grayScaleChannel);
    wall = displayWall(image, pacmanHeight, pacmanHeight);
    doWall(wall, pacmanHeight, pacmanHeight);

    byte *points = copyImage(image, pacmanHeight, pacmanHeight, grayScaleChannel);
    points = displayPoints(image, pacmanHeight, pacmanHeight);
//    exportFile("points.raw", points, 0, pacmanHeight, pacmanHeight);
    doPoints(points, pacmanHeight, pacmanHeight);
    
    
}


void doPoints(byte *image, int height, int width){
    image = expandImage(image, height, width);
    
    for (int i = 0; i < width + 1; i++) {
        for (int j = 0; j < height + 1; j++) {
            byte *pixel = getPixel(image, width + 1, i, j, grayScaleChannel);
            if (pixel[0] > 50) {
                
                pixel[0] = 1;
                
            }
            else pixel[0] = 0;
        }
    }
    
    byte *copyStar = (byte *)malloc((height + 1) * (width + 1) * grayScaleChannel * sizeof(byte));
    byte *output = (byte *)malloc((height + 1) * (width + 1) * grayScaleChannel * sizeof(byte));
    
    
    int match = 0;
    int iteration = 0;
    do{
        
        for (int i = 1; i < width ; i++) {
            for (int j = 1; j < height ; j++) {
                int matrix[3][3] = {0};
                
                
                matrix[0][0] = (getPixel(image, width + 1, i-1, j-1, grayScaleChannel))[0];
                matrix[0][1] = (getPixel(image, width + 1, i-1, j, grayScaleChannel))[0];
                matrix[0][2] = (getPixel(image, width + 1, i-1, j+1, grayScaleChannel))[0];
                matrix[1][0] = (getPixel(image, width + 1, i, j-1, grayScaleChannel))[0];
                matrix[1][1] = (getPixel(image, width + 1, i, j, grayScaleChannel))[0];
                matrix[1][2] = (getPixel(image, width + 1, i, j+1, grayScaleChannel))[0];
                matrix[2][0] = (getPixel(image, width + 1, i+1, j-1, grayScaleChannel))[0];
                matrix[2][1] = (getPixel(image, width + 1, i+1, j, grayScaleChannel))[0];
                matrix[2][2] = (getPixel(image, width + 1, i+1, j+1, grayScaleChannel))[0];
                

                byte *pix = getPixel(image, width + 1, i, j, grayScaleChannel);
                byte *copyPix = getPixel(copyStar, width + 1, i, j, grayScaleChannel);
                byte *outputPix = getPixel(output, width + 1, i, j, grayScaleChannel);
                
                int bound = 2 * (matrix[0][1] + matrix[1][2] + matrix[2][1] + matrix[1][0]) + (matrix[0][0] + matrix[0][2] + matrix[2][2] + matrix[2][0]);
                int a = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];
                
                if (!bound || bound == 12) {
                    copyPix[0] = 0;
                }
                else{
                    if (thinningConditional(a)) {
                        copyPix[0] = 1;
                    }else{
                        copyPix[0] = 0;
                    }
                    
                    if (copyPix[0] == 0) {
                        outputPix[0] = pix[0];
                    }
                }
                
                if (copyPix[0] == 1) {
                    if (unconditionalFilter(matrix)) {
                        outputPix[0] = pix[0];
                    }else{
                        copyPix[0] = 0;
                        outputPix[0] = 0;
                        
                    }
                }
                
                
                
            }
        }
        
        match = 0;
        for (int x = 0; x < width + 1; x++) {
            for (int y = 0; y < height + 1; y++) {
                byte *pix = getPixel(image, width + 1, x, y, grayScaleChannel);
                byte *pixCopy = getPixel(output, width + 1, x, y, grayScaleChannel);
                if (pix[0] != pixCopy[0]) {
                    match++;
                }
                pix[0] = pixCopy[0];
            }
        }
        iteration++;
    }while (match != 0);
    
    
    
    int count = 0;
    for (int x = 1; x < width -3; x++) {
        for (int y = 1; y < height - 3; y++) {
            int add = 0;
            
            byte *pix = getPixel(image, width + 1, x, y, grayScaleChannel);
            if (pix[0] == 1) {
                int match = 0;
                
                int matrix[3][3] = {{0,0,1}, {1,1,0},{0,0,0}};
                int compare[3][3] = {0};
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        byte *pixel = getPixel(image, width + 1, i + x, j + y, grayScaleChannel);
                        compare[i+1][j+1] = pixel[0];
                        if (pixel[0] == matrix[i+1][j+1]) {
                            match++;
                        }
                    }
                }
//                printf("%d %d\n", x, y);

                
                if(match == 9){
                    count++;

                }
            }
        }
    }
    
    for (int x = 1; x < width -3; x++) {
        for (int y = 1; y < height - 3; y++) {
            int add = 0;
            
            byte *pix = getPixel(image, width + 1, x, y, grayScaleChannel);
            if (pix[0] == 1) {
                pix[0] = 255;

            }
        }
    }

    
    printf("%d %d\n", count, iteration);
    
    
    exportFile("points1.raw", image, 0, width + 1, 1 + height);

    
}


void doWall(byte *image, int height, int width){
    
    for (int i = 0; i < pacmanHeight; i++) {
        for (int j = 0; j < pacmanHeight; j++) {
            byte *pixel = getPixel(image, pacmanHeight , i, j, grayScaleChannel);
            if (pixel[0] > 130) {
                
                pixel[0] = 1;
                
            }
            else pixel[0] = 0;
        }
    }
    
    byte *copyDigits = (byte *)malloc((pacmanHeight) * (pacmanHeight) * grayScaleChannel * sizeof(byte));
    byte *output = (byte *)malloc((pacmanHeight) * (pacmanHeight) * grayScaleChannel * sizeof(byte));
    
    
    int match = 0;
    
//    do {
//        
//        for (int i = 1; i < pacmanHeight + 1; i++) {
//            for (int j = 1; j < pacmanHeight + 1; j++) {
//                int matrix[3][3] = {0};
//                
//                
//                matrix[0][0] = (getPixel(image, pacmanHeight + 1, i-1, j-1, grayScaleChannel))[0];
//                matrix[0][1] = (getPixel(image, pacmanHeight + 1, i-1, j, grayScaleChannel))[0];
//                matrix[0][2] = (getPixel(image, pacmanHeight + 1, i-1, j+1, grayScaleChannel))[0];
//                matrix[1][0] = (getPixel(image, pacmanHeight + 1, i, j-1, grayScaleChannel))[0];
//                matrix[1][1] = (getPixel(image, pacmanHeight + 1, i, j, grayScaleChannel))[0];
//                matrix[1][2] = (getPixel(image, pacmanHeight + 1, i, j+1, grayScaleChannel))[0];
//                matrix[2][0] = (getPixel(image, pacmanHeight + 1, i+1, j-1, grayScaleChannel))[0];
//                matrix[2][1] = (getPixel(image, pacmanHeight + 1, i+1, j, grayScaleChannel))[0];
//                matrix[2][2] = (getPixel(image, pacmanHeight + 1, i+1, j+1, grayScaleChannel))[0];
//                
//                byte *pix = getPixel(image, pacmanHeight + 1, i, j, grayScaleChannel);
//                byte *copyPix = getPixel(copyDigits, pacmanHeight + 1, i, j, grayScaleChannel);
//                byte *outputPix = getPixel(output, pacmanHeight + 1, i, j, grayScaleChannel);
//                
//                int bound = 2 * (matrix[0][1] + matrix[1][2] + matrix[2][1] + matrix[1][0]) + (matrix[0][0] + matrix[0][2] + matrix[2][2] + matrix[2][0]);
//                int a = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];
//                
//                if (!bound || bound == 12) {
//                    copyPix[0] = 0;
//                }
//                else{
//                    if (thinningConditional(a)) {
//                        copyPix[0] = 1;
//                    }else{
//                        copyPix[0] = 0;
//                    }
//                    
//                    
//                    if (copyPix[0] == 0) {
//                        outputPix[0] = pix[0];
//                    }
//                    
//                }
//                
//                if (copyPix[0] == 1) {
//                    if (unconditionalFilter(matrix)) {
//                        outputPix[0] = pix[0];
//                    }else{
//                        copyPix[0] = 0;
//                        outputPix[0] = 0;
//                        
//                    }
//                }
//                
//                
//                
//            }
//        }
//        
//        match = 0;
//        for (int x = 0; x < pacmanHeight + 1; x++) {
//            for (int y = 0; y < pacmanHeight + 1; y++) {
//                byte *pix = getPixel(image, pacmanHeight + 1, x, y, grayScaleChannel);
//                byte *pixCopy = getPixel(output, pacmanHeight + 1, x, y, grayScaleChannel);
//                if (pix[0] != pixCopy[0]) {
//                    match++;
//                }
//                pix[0] = pixCopy[0];
//            }
//        }
//        
//    } while (match != 0);
//    
    
    int count = 0;
    
    match = 0;
    int iteration = 0;
    do{
        
        for (int i = 1; i < width ; i++) {
            for (int j = 1; j < height ; j++) {
                int matrix[3][3] = {0};
                
                
                matrix[0][0] = (getPixel(image, width + 1, i-1, j-1, grayScaleChannel))[0];
                matrix[0][1] = (getPixel(image, width + 1, i-1, j, grayScaleChannel))[0];
                matrix[0][2] = (getPixel(image, width + 1, i-1, j+1, grayScaleChannel))[0];
                matrix[1][0] = (getPixel(image, width + 1, i, j-1, grayScaleChannel))[0];
                matrix[1][1] = (getPixel(image, width + 1, i, j, grayScaleChannel))[0];
                matrix[1][2] = (getPixel(image, width + 1, i, j+1, grayScaleChannel))[0];
                matrix[2][0] = (getPixel(image, width + 1, i+1, j-1, grayScaleChannel))[0];
                matrix[2][1] = (getPixel(image, width + 1, i+1, j, grayScaleChannel))[0];
                matrix[2][2] = (getPixel(image, width + 1, i+1, j+1, grayScaleChannel))[0];
                
                byte *pix = getPixel(image, width + 1, i, j, grayScaleChannel);
                byte *copyPix = getPixel(copyDigits, width + 1, i, j, grayScaleChannel);
                byte *outputPix = getPixel(output, width + 1, i, j, grayScaleChannel);
                
                int bound = 2 * (matrix[0][1] + matrix[1][2] + matrix[2][1] + matrix[1][0]) + (matrix[0][0] + matrix[0][2] + matrix[2][2] + matrix[2][0]);
                int a = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];
                
                if (!bound || bound == 12) {
                    copyPix[0] = 0;
                }
                else{
                    if (conditionalSkeletonize(a)) {
                        copyPix[0] = 1;
                    }else{
                        copyPix[0] = 0;
                    }
                    
                    if (copyPix[0] == 0) {
                        outputPix[0] = pix[0];
                    }
                }
                
                if (copyPix[0] == 1) {
                    if (unconditionalSkeletonize(matrix)) {
                        outputPix[0] = pix[0];
                    }else{
                        copyPix[0] = 0;
                        outputPix[0] = 0;
                        
                    }
                }
                
                
                
            }
        }
        
        match = 0;
        for (int x = 0; x < width + 1; x++) {
            for (int y = 0; y < height + 1; y++) {
                byte *pix = getPixel(image, width + 1, x, y, grayScaleChannel);
                byte *pixCopy = getPixel(output, width + 1, x, y, grayScaleChannel);
                if (pix[0] != pixCopy[0]) {
                    match++;
                }
                pix[0] = pixCopy[0];
            }
        }
        iteration++;
    }while (match != 0);
    
    
    
    for (int i = 0; i < pacmanHeight; i++) {
        for (int j = 0; j < pacmanHeight; j++) {
            byte *pix = getPixel(image, pacmanHeight, i, j, grayScaleChannel);
            
            if (pix[0] == 1) {
                pix[0] = 255;
            }
        }
    }
    
    exportFile("wall.raw", image, 0, pacmanHeight, pacmanHeight);

    
    
}



void dinosaur(byte *image){
    image = convertToGrayScale(image, dinosaurWidth, dinosaurHeight);
    
    
    
    for (int i = 0; i < dinosaurWidth; i++) {
        for (int j = 0; j < dinosaurHeight; j++) {
            byte *pixel = getPixel(image, dinosaurWidth , i, j, grayScaleChannel);
            if (pixel[0] > 130) {
            
                
                pixel[0] = 1;
                
            }
            else pixel[0] = 0;
        }
    }
    
    byte *copyDigits = (byte *)malloc((dinosaurHeight) * (dinosaurWidth) * grayScaleChannel * sizeof(byte));
    byte *output = (byte *)malloc((dinosaurHeight) * (dinosaurWidth) * grayScaleChannel * sizeof(byte));
    
    
    int match = 0;
    int iteration = 0;
    do {
        
        for (int i = 1; i < dinosaurWidth + 1; i++) {
            for (int j = 1; j < dinosaurHeight + 1; j++) {
                int matrix[3][3] = {0};
                
                
                matrix[0][0] = (getPixel(image, dinosaurWidth + 1, i-1, j-1, grayScaleChannel))[0];
                matrix[0][1] = (getPixel(image, dinosaurWidth + 1, i-1, j, grayScaleChannel))[0];
                matrix[0][2] = (getPixel(image, dinosaurWidth + 1, i-1, j+1, grayScaleChannel))[0];
                matrix[1][0] = (getPixel(image, dinosaurWidth + 1, i, j-1, grayScaleChannel))[0];
                matrix[1][1] = (getPixel(image, dinosaurWidth + 1, i, j, grayScaleChannel))[0];
                matrix[1][2] = (getPixel(image, dinosaurWidth + 1, i, j+1, grayScaleChannel))[0];
                matrix[2][0] = (getPixel(image, dinosaurWidth + 1, i+1, j-1, grayScaleChannel))[0];
                matrix[2][1] = (getPixel(image, dinosaurWidth + 1, i+1, j, grayScaleChannel))[0];
                matrix[2][2] = (getPixel(image, dinosaurWidth + 1, i+1, j+1, grayScaleChannel))[0];
                
                byte *pix = getPixel(image, dinosaurWidth + 1, i, j, grayScaleChannel);
                byte *copyPix = getPixel(copyDigits, dinosaurWidth + 1, i, j, grayScaleChannel);
                byte *outputPix = getPixel(output, dinosaurWidth + 1, i, j, grayScaleChannel);
                
                int bound = 2 * (matrix[0][1] + matrix[1][2] + matrix[2][1] + matrix[1][0]) + (matrix[0][0] + matrix[0][2] + matrix[2][2] + matrix[2][0]);
                int a = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];
                
                if (!bound || bound == 12) {
                    copyPix[0] = 0;
                }
                else{
//                    printMatrix(matrix);
                    if (conditionalSkeletonize(a)) {
                        copyPix[0] = 1;
                    }else{
                        copyPix[0] = 0;
                    }
                    

                }
                
                if (copyPix[0] == 1) {
                    if (unconditionalSkeletonize(matrix)) {
                        outputPix[0] = pix[0];
                    }else{
                        copyPix[0] = 0;
                        outputPix[0] = 0;
                        
                    }
                }
                
                
                
            }
        }
        
        
        
        
        
        
        match = 0;
        for (int x = 0; x < dinosaurWidth + 1; x++) {
            for (int y = 0; y < dinosaurHeight + 1; y++) {
                byte *pix = getPixel(image, dinosaurWidth + 1, x, y, grayScaleChannel);
                byte *pixCopy = getPixel(output, dinosaurWidth + 1, x, y, grayScaleChannel);
                if (pix[0] != pixCopy[0]) {
                    match++;
                }
                pix[0] = pixCopy[0];
            }
        }
        iteration++;
    } while (match != 0);
    
    
    int count = 0;
    for (int x = 0; x < dinosaurWidth; x++) {
        for (int y = 0; y < dinosaurHeight; y++) {
            byte *pix = getPixel(image, dinosaurWidth, x, y, grayScaleChannel);
            if (pix[0] == 1) {
                pix[0] = 255;
            }
        }
    }
    
    
    printf("%d\n", iteration);
    
    exportFile("dinoExport.raw", image, 0, dinosaurWidth, dinosaurHeight);
    

}

int conditionalSkeletonize(int total){
    
    int tk4[4] = {
        0b010011000,
        0b010110000,
        0b000110010,
        0b000011010
        
    };
    
    int stk4[4] = {
        0b001011001,
        0b111010000,
        0b100110100,
        0b000010111
    };
    int stk6[8] = {
        0b111011000,
        0b011011001,
        0b111110000,
        0b110110100,
        0b100110110,
        0b000110111,
        0b000011111,
        0b001011011
    };
    
    int stk7[4] = {
        0b111011001,
        0b111110100,
        0b100110111,
        0b001011111
    };
    
    int stk8[4] = {
        0b011011011,
        0b111110100,
        0b110110110,
        0b000111111
    };
    
    int stk9[8] = {
        0b111011011,
        0b011011111,
        0b111111100,
        0b111111001,
        0b111110110,
        0b110110111,
        0b100111111,
        0b001111111
    };
    
    int stk10[4] = {
        0b111011111,
        0b111111101,
        0b111110111,
        0b101111111
    };

    int k10[4] = {
        0b111111011,
        0b111111110,
        0b110111111,
        0b011111111
    };
    
    
    for (int i = 0; i < 4; i++) {
        int tk4int = tk4[i];
        int stk4int = stk4[i];
        int stk8int = stk8[i];
        int stk7int = stk7[i];
        int stk10int = stk10[i];
        int k10int = k10[i];
        
        if (total == tk4int || total == stk4int || total == stk8int || total == stk7int || total == stk10int || total == k10int) {
//            printf("%d %d %d %d %d %d: %d\n", tk4int, stk4int, stk8int, stk7int, stk10int, k10int, total);

            return 1;
        }
    }
    
    for (int i = 0 ; i < 8; i++) {
        if (total == stk6[i] || total == stk9[i]) {
            return 1;
        }
    }
    
    return 0;
    
}


int unconditionalSkeletonize(int matrix[3][3]){
    
    
    int spur[4] = {
        0b000010001,
        0b000010100,
        0b001010000,
        0b100010000
    };
    
    int singleConnection[4] = {
        0b000010010,
        0b000011000,
        0b000110000,
        0b010010000
    };
    
    int LCorner[4] = {
        0b010011000,
        0b010110000,
        0b000011010,
        0b000110010
    };
    
    int connectedOffset[4] = {
        0b011110000,
        0b110011000,
        0b010011001,
        0b001011010
    };
    

    
    char cornerCluster [2][3][3] = {
        {
            {'1', '1', 'D'},
            {'1', '1', 'D'},
            {'D', 'D', 'D'}
        },
        {
            {'D', 'D', 'D'},
            {'D', '1', '1'},
            {'D', '1', '1'}
            
        }
    };
    
    char teeBranch [4][3][3] = {
        {
            {'D', '1', 'D'},
            {'1', '1', '1'},
            {'D', 'D', 'D'}
        },
        {
            {'D', '1', 'D'},
            {'1', '1', 'D'},
            {'D', '1', 'D'}
        },
        {
            {'D', 'D', 'D'},
            {'1', '1', '1'},
            {'D', '1', 'D'}
        },
        {
            {'D', '1', 'D'},
            {'D', '1', '1'},
            {'D', '1', 'D'}
        }
        
    };
    
    char veeBranch [4][3][3] = {
        {
            {'1', 'D', '1'},
            {'D', '1', 'D'},
            {'A', 'B', 'C'}
        },
        {
            {'1', 'D', 'C'},
            {'D', '1', 'B'},
            {'1', 'D', 'A'}
        },
        {
            {'C', 'B', 'A'},
            {'D', '1', 'D'},
            {'1', 'D', '1'}
        },
        {
            {'A', 'D', '1'},
            {'B', '1', 'D'},
            {'C', 'D', '1'}
        }
    };
    
    char diagonalBranch[4][3][3] = {
        {
            {'D', '1', '0'},
            {'0', '1', '1'},
            {'1', '0', 'D'}
        },
        {
            {'0', '1', 'D'},
            {'1', '1', '0'},
            {'D', '0', '1'}
        },
        {
            {'D', '0', '1'},
            {'1', '1', '0'},
            {'0', '1', 'D'}
        },
        {
            {'1', '0', 'D'},
            {'0', '1', '1'},
            {'D', '1', '0'}
        }
    };

    
    
    int total = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];

    
    for (int i = 0; i < 4; i++) {
        if (total == spur[i] || total == singleConnection[i] || total == LCorner[i] || total == connectedOffset[i]) {
            return 1;
        }
    }
    
    //----------------Corner Cluster
    for (int i = 0; i < 2; i++) {
        int semiMatrix[3][3] = {0};
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                semiMatrix[j][k] = cornerCluster[i][j][k] - '0';
            }
        }
        
        int match = 0;
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                
                if (semiMatrix[j][k] != 20) {
                    if (semiMatrix[j][k] == matrix[j][k]) {
                        match++;
                    }
                }
            }
        }
        
        if (match == 4) {
            return 1;
        }
        
    }
    
    
    //--------------diagonalBranch
    for (int i = 0; i < 4; i++) {
        int semiMatrix[3][3] = {0};
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                semiMatrix[j][k] = diagonalBranch[i][j][k] - '0';
            }
        }
        
        int match = 0;
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                
                if (semiMatrix[j][k] != 20) {
//                    printf("%d %d\n", semiMatrix[j][k], matrix[j][k]);
                    if (semiMatrix[j][k] == matrix[j][k]) {
                        match++;
                    }
                }
            }
        }
        
        if (match == 7) {
            return 1;
        }
        
    }
    //-----------------Tee Branch
    
    for (int i = 0; i < 4; i++) {
        int semiMatrix[3][3] = {0};
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                semiMatrix[j][k] = teeBranch[i][j][k] - '0';
            }
        }
        
        int match = 0;
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                
                if (semiMatrix[j][k] != 20) {
                    if (semiMatrix[j][k] == matrix[j][k]) {
                        match++;
                    }
                }
            }
        }
        
        if (match == 7) {
            return 1;
        }
        
    }
    //------------------Vee Branch
    for (int i = 0; i < 4; i++) {
        
        int semiMatrix[3][3] = {0};
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                semiMatrix[j][k] = veeBranch[i][j][k] - '0';
            }
        }
        
        
        int match = 0;
        int Aj, Ak = 0;
        int Bj, Bk = 0;
        int Cj, Ck = 0;
        
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                if (semiMatrix[j][k] == 1 && (semiMatrix[j][k] == matrix[j][k])){
                    match++;
                }
                if (semiMatrix[j][k] == 17) {
                    Aj = j;
                    Ak = k;
                }
                if (semiMatrix[j][k] == 18) {
                    Bj = j;
                    Bk = k;
                }
                if (semiMatrix[j][k] == 19) {
                    Cj = j;
                    Ck = k;
                }
                
            }
        }
        if (match == 3 && (matrix[Aj][Ak] || matrix[Bj][Bk] || matrix[Cj][Ck])) {
            return 1;
        }

    }
//    printMatrix(matrix);

    return 0;
    
}

void countNumbers(byte *image){
    image = convertToGrayScale(image, digitsWidth, digitsHeight);
    
    for (int i = 0; i < digitsWidth; i++) {
        for (int j = 0; j < digitsHeight; j++) {
            byte *pixel = getPixel(image, digitsWidth , i, j, grayScaleChannel);
            if (pixel[0] > 130) {
                
                pixel[0] = 1;
                
            }
            else pixel[0] = 0;
        }
    }
    
    byte *copyDigits = (byte *)malloc((digitsHeight) * (digitsWidth) * grayScaleChannel * sizeof(byte));
    byte *output = (byte *)malloc((digitsHeight) * (digitsWidth) * grayScaleChannel * sizeof(byte));


    int match = 0;

    do {

        for (int i = 1; i < digitsWidth ; i++) {
            for (int j = 1; j < digitsHeight ; j++) {
                int matrix[3][3] = {0};
                
                
                matrix[0][0] = (getPixel(image, digitsWidth + 1, i-1, j-1, grayScaleChannel))[0];
                matrix[0][1] = (getPixel(image, digitsWidth + 1, i-1, j, grayScaleChannel))[0];
                matrix[0][2] = (getPixel(image, digitsWidth + 1, i-1, j+1, grayScaleChannel))[0];
                matrix[1][0] = (getPixel(image, digitsWidth + 1, i, j-1, grayScaleChannel))[0];
                matrix[1][1] = (getPixel(image, digitsWidth + 1, i, j, grayScaleChannel))[0];
                matrix[1][2] = (getPixel(image, digitsWidth + 1, i, j+1, grayScaleChannel))[0];
                matrix[2][0] = (getPixel(image, digitsWidth + 1, i+1, j-1, grayScaleChannel))[0];
                matrix[2][1] = (getPixel(image, digitsWidth + 1, i+1, j, grayScaleChannel))[0];
                matrix[2][2] = (getPixel(image, digitsWidth + 1, i+1, j+1, grayScaleChannel))[0];
                
                byte *pix = getPixel(image, digitsWidth + 1, i, j, grayScaleChannel);
                byte *copyPix = getPixel(copyDigits, digitsWidth + 1, i, j, grayScaleChannel);
                byte *outputPix = getPixel(output, digitsWidth + 1, i, j, grayScaleChannel);
                
                int bound = 2 * (matrix[0][1] + matrix[1][2] + matrix[2][1] + matrix[1][0]) + (matrix[0][0] + matrix[0][2] + matrix[2][2] + matrix[2][0]);
                int a = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];
                
                if (!bound || bound == 12) {
                    copyPix[0] = 0;
                }
                else{
                    if (thinningConditional(a)) {
                        copyPix[0] = 1;
                    }else{
                        copyPix[0] = 0;
                    }
                                    

                    if (copyPix[0] == 0) {
                        outputPix[0] = pix[0];
                    }
                
                }
                
                if (copyPix[0] == 1) {
                    if (unconditionalFilter(matrix)) {
                        outputPix[0] = pix[0];
                    }else{
                        copyPix[0] = 0;
                        outputPix[0] = 0;
                        
                    }
                }
                
                
                
            }
        }
        
        match = 0;
        for (int x = 0; x < digitsWidth + 1; x++) {
            for (int y = 0; y < digitsHeight + 1; y++) {
                byte *pix = getPixel(image, digitsWidth + 1, x, y, grayScaleChannel);
                byte *pixCopy = getPixel(output, digitsWidth + 1, x, y, grayScaleChannel);
                if (pix[0] != pixCopy[0]) {
                    match++;
                }
                pix[0] = pixCopy[0];
            }
        }
        
    } while (match != 0);


    int count = 0;
    
    
    
    int mean = 16;
    int sigma = 13;
    
    int iGaus = -5;
    for (int j = 17; j < digitsHeight; j += 24, iGaus++) {
        int y = exp(-(pow(iGaus - mean, 2)/(2 * pow(sigma, 2)))) / (sigma * sqrt(2 * M_PI)) * 1000;
        
        for (int i = 0; i < digitsWidth; i++) {
            byte *pixel = getPixel(image, digitsWidth, i, j, grayScaleChannel);
            if (pixel[0] > 0) {
                i += y;
                count++;
                pixel[0] = 255;
            }
        }
        
    }
    exportFile("digitsExport.raw", image, 0, digitsWidth, digitsHeight);

    printf("%d\n", count);
}


int thinningConditional(int matrix){
    
    
    int tk4[4] = {
        0b010011000,
        0b010110000,
        0b000110010,
        0b000011010
    
    };
    
    int stk4[4] = {
        0b001011001,
        0b111010000,
        0b100110100,
        0b000010111
    };
    
    int st5[4] = {
        0b110011000,
        0b010011001,
        0b011110000,
        0b001011010
    };
    
    int st51[4] = {
        0b011011000,
        0b110110000,
        0b000110110,
        0b000011011
    };
    
    int st6[2] = {
        0b110011001,
        0b011110100
    };
    
    int stk6[8] = {
        0b111011000,
        0b011011001,
        0b111110000,
        0b110110100,
        0b100110110,
        0b000110111,
        0b000011111,
        0b001011011
    };
    
    int stk7[4] = {
        0b111011001,
        0b111110100,
        0b100110111,
        0b001011111
    };
    
    int stk8[4] = {
        0b011011011,
        0b111110100,
        0b110110110,
        0b000111111
    };
    
    int stk9[8] = {
        0b111011011,
        0b011011111,
        0b111111100,
        0b111111001,
        0b111110110,
        0b110110111,
        0b100111111,
        0b001111111
    };
    
    int stk10[4] = {
        0b111011111,
        0b111111101,
        0b111110111,
        0b101111111
    };
    
    for (int i = 0; i < 4; i++) {
        
        if (matrix == tk4[i] || matrix == stk4[i] || matrix == st5[i] || matrix == stk7[i] || matrix == stk8[i] || matrix == stk10[i] || matrix == st51[i]) {
            return 1;
        }
    }
    for (int i = 0; i < 8; i++) {
        if (matrix == stk6[i] || matrix == stk9[i]) {
            return 1;
        }
    }
    for (int i = 0 ; i < 2; i++) {
        if (matrix == st6[i]) {
            return 1;
        }
    }

    return 0;

}


void countStars(byte *image, int width, int height){
    
    image = convertToGrayScale(image, width, height);
    image = expandImage(image, height, width);
    
    for (int i = 0; i < width + 1; i++) {
        for (int j = 0; j < height + 1; j++) {
            byte *pixel = getPixel(image, width + 1, i, j, grayScaleChannel);
            if (pixel[0] > 50) {
                
                pixel[0] = 1;
                
            }
            else pixel[0] = 0;
        }
    }
    
    byte *copyStar = (byte *)malloc((height + 1) * (width + 1) * grayScaleChannel * sizeof(byte));
    byte *output = (byte *)malloc((height + 1) * (width + 1) * grayScaleChannel * sizeof(byte));

    
    
    FILE* fpIn = fopen("starsHistogram.csv", "w");
    
    int match = 0;
    int iteration = 0;
    do{

        for (int i = 1; i < width ; i++) {
            for (int j = 1; j < height ; j++) {
                int matrix[3][3] = {0};
                
                
                matrix[0][0] = (getPixel(image, width + 1, i-1, j-1, grayScaleChannel))[0];
                matrix[0][1] = (getPixel(image, width + 1, i-1, j, grayScaleChannel))[0];
                matrix[0][2] = (getPixel(image, width + 1, i-1, j+1, grayScaleChannel))[0];
                matrix[1][0] = (getPixel(image, width + 1, i, j-1, grayScaleChannel))[0];
                matrix[1][1] = (getPixel(image, width + 1, i, j, grayScaleChannel))[0];
                matrix[1][2] = (getPixel(image, width + 1, i, j+1, grayScaleChannel))[0];
                matrix[2][0] = (getPixel(image, width + 1, i+1, j-1, grayScaleChannel))[0];
                matrix[2][1] = (getPixel(image, width + 1, i+1, j, grayScaleChannel))[0];
                matrix[2][2] = (getPixel(image, width + 1, i+1, j+1, grayScaleChannel))[0];
                
                byte *pix = getPixel(image, width + 1, i, j, grayScaleChannel);
                byte *copyPix = getPixel(copyStar, width + 1, i, j, grayScaleChannel);
                byte *outputPix = getPixel(output, width + 1, i, j, grayScaleChannel);

                int bound = 2 * (matrix[0][1] + matrix[1][2] + matrix[2][1] + matrix[1][0]) + (matrix[0][0] + matrix[0][2] + matrix[2][2] + matrix[2][0]);
                int a = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];

                if (!bound || bound == 12) {
                    copyPix[0] = 0;
                }
                else{
                    if (shrinkFilter(a)) {
                        copyPix[0] = 1;
                    }else{
                        copyPix[0] = 0;
                    }
                    
                    if (copyPix[0] == 0) {
                        outputPix[0] = pix[0];
                    }
                }
                
                if (copyPix[0] == 1) {
                    if (unconditionalFilter(matrix)) {
                        outputPix[0] = pix[0];
                    }else{
                        copyPix[0] = 0;
                        outputPix[0] = 0;
                    
                    }
                }
                

                
            }
        }
        
        match = 0;
        int count = 0;
        for (int x = 1; x < width + 1; x++) {
            for (int y = 1; y < height + 1; y++) {
                byte *pix = getPixel(image, width + 1, x, y, grayScaleChannel);
                byte *pixCopy = getPixel(output, width + 1, x, y, grayScaleChannel);
                if (pix[0] != pixCopy[0]) {
                    match++;
                }

                int star = 0;
                for (int a = x-1; a< x+2; a++) {
                    for (int b = y-1; b < y+2; b++) {
                        byte *p = getPixel(image, width + 1, a+x, b+y, grayScaleChannel);
                        star += p[0];
                        
                    }
                }
                
                if (pix[0] == 1) {
                    count++;
                }
                pix[0] = pixCopy[0];
            }
        }
        
        fprintf(fpIn, "%d\n", count);
        iteration++;
    }while (match != 0);
    
    
    fclose(fpIn);
    
    int count = 0;
    for (int x = 0; x < starsWidth + 1; x++) {
        for (int y = 0; y < height + 1; y++) {
            int add = 0;
            byte *pix = getPixel(image, width + 1, x, y, grayScaleChannel);
            if (pix[0] == 1) {
                count++;
                while (pix[0] == 1) {
                    pix = getPixel(image, width + 1, x + add, y, grayScaleChannel);
                    add++;
                }
                y += add - 1;
                pix[0] = 255;
            }
        }
    }
    
    printf("%d %d\n", count, iteration);

    
    exportFile("export.raw", image, 0, width + 1, 1 + height);
    
    
}

int unconditionalFilter(int matrix[3][3]){
    
    int found = 0;
    
    int spur[2]={
        0b001010000,
        0b100010000
    };
    
    int single4Connection[2] = {
        0b000010010,0b000011000
    };
    
    int LCluster[8] = {
        0b001011000,
        0b011010000,
        0b110010000,
        0b100110000,
        0b000110100,
        0b000010110,
        0b000010011,
        0b000011001
    };
    
    int connectedOffset[4] = {
        0b011110000,
        0b110011000,
        0b010011001,
        0b001011010
    };
    
    char teeBranch[8][3][3] = {     //done
        {
            {'D', '1', '0'},
            {'1', '1', '1'},
            {'D', '0', '0'}
        },
        {
            {'0', '1', 'D'},
            {'1', '1', '1'},
            {'0', '0', 'D'}
        },
        {
            {'0', '0', 'D'},
            {'1', '1', '1'},
            {'0', '1', 'D'}
        },
        {
            {'D', '0', '0'},
            {'1', '1', '1'},
            {'D', '1', '0'}
        },
        {
            {'D', '1', 'D'},
            {'1', '1', '0'},
            {'0', '1', '0'}
        },
        {
            {'0', '1', '0'},
            {'1', '1', '0'},
            {'D', '1', 'D'}
        },
        {
            {'0', '1', '0'},
            {'1', '1', '0'},
            {'D', '1', 'D'}
        },
        {
            {'D', '1', 'D'},
            {'0', '1', '1'},
            {'0', '1', '0'}
        }
    };
    
    char cornerCluster [3][3] = {
        {'1', '1', 'D'},
        {'1', '1', 'D'},
        {'D', 'D', 'D'}
    };
    
    char veeBranch [4][3][3] = {
        {
            {'1', 'D', '1'},
            {'D', '1', 'D'},
            {'A', 'B', 'C'}
        },
        {
            {'1', 'D', 'C'},
            {'D', '1', 'B'},
            {'1', 'D', 'A'}
            
        },
        {
            {'C', 'B', 'A'},
            {'D', '1', 'D'},
            {'1', 'D', '1'}
        },
        {
            {'A', 'D', '1'},
            {'B', '1', 'D'},
            {'C', 'D', '1'}
        }
    };
    
    
    char diagonalBranch [4][3][3] = {
        
        {
            {'D', '1', '0'},
            {'0', '1', '1'},
            {'1', '0', 'D'}
        },
        {
            {'0', '1', 'D'},
            {'1', '1', '0'},
            {'D', '0', '1'}
        },
        {
            {'D', '0', '1'},
            {'1', '1', '0'},
            {'0', '1', 'D'}
        },
        {
            {'1', '0', 'D'},
            {'0', '1', '1'},
            {'D', '1', '0'}
        }
        
    };
    
    
    int total = matrix[0][0] * pow(2, 8) + matrix[0][1] * pow(2, 7) + matrix[0][2] * pow(2, 6) + matrix[1][0] * pow(2, 5) + matrix[1][1] * pow(2, 4) + matrix[1][2] * pow(2, 3) + matrix[2][0] * pow(2, 2) + matrix[2][1] * 2 + matrix[2][2];
    
    
    for (int i = 0; i < 8; i++) {
        if (total == LCluster[i]) {
            return 1;
        }
        
        if (matrix[1][1] == 1) {
            int semiMatrix[3][3] = {0};
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    semiMatrix[j][k] = teeBranch[i][j][k] - '0';
                }
            }
            
            
            int count = 0;
            int match = 0;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    
                    if (semiMatrix[j][k] != 20 ) {
                        if (semiMatrix[j][k] == matrix[j][k]) {
                            match++;
                        }
                        count++;
                    }
                    
                }
            }
            
            if (match == count) {
                return 1;
            }
            
        }
        
    }
    
    for (int i = 0; i < 2; i++) {
        if (total == spur[i] || total==single4Connection[i]) {
            return 1;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        if (total == connectedOffset[i]) {
            return 1;
        }
        
        if (matrix[1][1] == 1) {
            int semiMatrix[3][3] = {0};
            int count = 0;
            int match = 0;

            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    semiMatrix[j][k] = diagonalBranch[i][j][k] - '0';
                }
            }
            
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    
                    if (semiMatrix[j][k] == 0 ||semiMatrix[j][k] == 1) {
                        if (semiMatrix[j][k] == matrix[j][k]) {
                            match++;
                        }
                        count++;
                    }
                    
                }
            }
            
            if (match == count) {
                return 1;
            }
            
            
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    semiMatrix[j][k] = veeBranch[i][j][k] - '0';
                }
            }
            
            
            match = 0;
            count = 3;
            int Aj, Ak = 0;
            int Bj, Bk = 0;
            int Cj, Ck = 0;
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    if (semiMatrix[j][k] == 1 && (semiMatrix[j][k] == matrix[j][k])){
                        match++;
                    }
                    if (semiMatrix[j][k] == 17) {
                        Aj = j;
                        Ak = k;
                    }
                    if (semiMatrix[j][k] == 18) {
                        Bj = j;
                        Bk = k;
                    }
                    if (semiMatrix[j][k] == 19) {
                        Cj = j;
                        Ck = k;
                    }
                    
                }
            }
            if (match == 3 && (matrix[Aj][Ak] || matrix[Bj][Bk] || matrix[Cj][Ck])) {
                return 1;
            }
            
        }
        
        
        
    }
    
    int match = 0;
    int count = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int val = cornerCluster[i][j] - '0';
            
            if (val == 1) {
                if (val == matrix[i][j]) {
                    match++;
                }
                count++;
            }
                    

        }
    }
    
    if (match == count) {
        return 1;
    }
    

    return found;
    
    
}

void printMatrix(int matrix[3][3]){
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    
    printf("-------\n");

}

int shrinkFilter(int a){
    
   
    int s1[4] = {
        0b001010000,
        0b100010000,
        0b000010100,
        0b000010001
    };
    
    
    int s2[4] = {
        0b000011000,
        0b010010000,
        0b000110000,
        0b000010010
    };
    
    int s3[8] = {
        0b001011000,
        0b011010000,
        0b110010000,
        0b100110000,
        0b000110100,
        0b000010110,
        0b000010011,
        0b000011001
    };
    
    int stk4[4] = {
        0b001011001,
        0b111010000,
        0b100110100,
        0b000010111
    };
    
    int st5[4] = {
        0b110011000,
        0b010011001,
        0b011110000,
        0b001011010
    };
    
    int st51[4] = {
        0b011011000,
        0b110110000,
        0b000110110,
        0b000011011
    };
    
    int st6[2] = {
        0b110011001,
        0b011110100
    };
    
    int stk6[8] = {
        0b111011000,
        0b011011001,
        0b111110000,
        0b110110100,
        0b100110110,
        0b000110111,
        0b000011111,
        0b001011011
    };
    
    int stk7[4] = {
        0b111011001,
        0b111110100,
        0b100110111,
        0b001011111
    };
    
    int stk8[4] = {
        0b011011011,
        0b111110100,
        0b110110110,
        0b000111111
    };
    
    int stk9[8] = {
        0b111011011,
        0b011011111,
        0b111111100,
        0b111111001,
        0b111110110,
        0b110110111,
        0b100111111,
        0b001111111
    };
    
    int stk10[4] = {
        0b111011111,
        0b111111101,
        0b111110111,
        0b101111111
    };
    
    for (int i = 0; i < 4; i++) {
        
        if (a == s1[i] || a == s2[i] || a == stk4[i] || a == st5[i] || a == stk7[i] || a == stk8[i] || a == stk10[i] || a == st51[i]) {
            return 1;
        }
    }
    for (int i = 0; i < 8; i++) {
        if (a == s3[i] || a == stk6[i] || a == stk9[i]) {
            return 1;
        }
    }
    for (int i = 0 ; i < 2; i++) {
        if (a == st6[i]) {
            return 1;
        }
    }
    
    return 0;
}



byte *getPixel(byte *image, int width, int i, int j, int channels){
    return (image + width * j * channels + channels * i);
}

byte *expandImage(byte *image, int height, int width){
    
    byte *tempImageData = (byte*)malloc((width + 1) * (height + 1) * grayScaleChannel * sizeof(byte));
    
    for (int i = 0; i < width; i++) {
        byte *tempPixel = tempImageData + (width + 1) * 0 * grayScaleChannel + grayScaleChannel * i;
        byte * pixel = image + (height) * 0 * grayScaleChannel + grayScaleChannel * i;
        tempPixel[0] = pixel[0];
    }
    
    
    for (int j = 0; j < height; j++) {
        byte *tempPixel = tempImageData + (width + 1) * j * grayScaleChannel + grayScaleChannel * j;
        byte *pixel = image + (height) * j * grayScaleChannel + grayScaleChannel * 0;
        tempPixel[0] = pixel[0];
    }
    
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            byte *tempPixel = tempImageData + (width + 1) * (j + 1) * grayScaleChannel + grayScaleChannel * (i + 1);
            byte *pixel = image + height * (j) * grayScaleChannel + grayScaleChannel * (i);
            tempPixel[0] = pixel[0];
        }
    }
    
    byte *tempPixel = tempImageData + (width+1) * 0 * grayScaleChannel + grayScaleChannel * 0;;
    byte *pixel = image + (height) * 0 * grayScaleChannel + grayScaleChannel * 0;
    tempPixel[0] = pixel[0];
    
    return tempImageData;
}

void exportFile(char *fileName, byte *image, int offset, int width, int height){
    
    FILE* file = fopen(fileName, "wb");
    fwrite(image, sizeof(unsigned char), (height + offset) * (offset + width) * grayScaleChannel, file);
    fclose(file);
    
}

byte *readFile(char *fileName, int width, int height){
    
    FILE* file = fopen(fileName, "rb");
    if (!file) {
        printf("can't find file %s\n", fileName);
        exit(100);
    }
    byte *image = (byte *)malloc(height * width * channel * sizeof(byte));
    
    fread(image, sizeof(unsigned char), height * width * channel, file);
    fclose(file);
    return image;
    
}

byte* convertToGrayScale(byte *image, int width, int height){
    
    byte *newImage = (byte *)malloc(height * width * grayScaleChannel * sizeof(byte));
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            byte *pixel = image + height * j * channel + channel * i;
            byte *newpix = newImage + height * j * grayScaleChannel + grayScaleChannel * i;
            
            newpix[0] = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
            
        }
    }
    
    return newImage;
}

byte* displayWall(byte *image, int width, int height){
    
    byte *newImage = (byte *)malloc(height * width * grayScaleChannel * sizeof(byte));
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            byte *pixel = image + height * j * channel + channel * i;
            
            byte *newpix = newImage + height * j * grayScaleChannel + grayScaleChannel * i;
            if (pixel[2] >= 150 && pixel[0] < 150 && pixel[1] < 150) {
                newpix[0] = 255;

            }else newpix[0] = 0;
            
        }
    }
    
    return newImage;
}

byte* displayPoints(byte *image, int width, int height){
    
    byte *newImage = (byte *)malloc(height * width * grayScaleChannel * sizeof(byte));
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            byte *pixel = image + height * j * channel + channel * i;
            
            byte *newpix = newImage + height * j * grayScaleChannel + grayScaleChannel * i;
            if (pixel[0] >= 220 && pixel[1] > 220 && pixel[2] > 220) {
                
                newpix[0] = 255;
                
            }else newpix[0] = 0;
            
        }
    }
    
    
    
    
    return newImage;
}





void histogram(char *filename, byte *image, int channels, int w, int h){
    
    int red[256] = {0};
    int green[256] = {0};
    int blue[256] = {0};
    int intensity[256] = {0};
    
    
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            byte *pixel = image + h * j * channels + channels * i;
            if (channels == 1) {
                intensity[pixel[0]]++;
            }
            else{
                red[pixel[0]]++;
                green[pixel[1]]++;
                blue[pixel[2]]++;
            }
        }
    }
    
    FILE* file = fopen(filename, "w");
    if (channels == 1) {
        fprintf(file, "Intensity\n");
        for (int i = 0; i < 256; i++){
            fprintf(file, "%d\n", intensity[i]);
        }
    }else{
        fprintf(file, "Red,Green,Blue\n");
        for (int i = 0; i < 256; i++) {
            fprintf(file, "%d,%d,%d\n", red[i], blue[i], green[i]);
        }
    }
    
    fclose(file);
}


byte *copyImage(byte *image, int imgWidth, int imgHeight, int channels){
    byte *JJN = (byte *)malloc(imgWidth * imgHeight * channels * sizeof(byte));
    
    for (int i = 0; i < imgWidth; i++) {
        for (int j = 0; j < imgHeight; j++) {
            byte *pixel = getPixel(image, imgWidth, i, j, channels);
            byte *copy = getPixel(JJN, imgWidth, i, j, channels);
            copy[0] = pixel[0];
        }
    }
    
    return JJN;
}
