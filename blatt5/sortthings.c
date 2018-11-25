#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 200
#define MOUNTAIN_COUNT 8

typedef struct Mountains {
    char name[MAX_NAME_LENGTH];
    int height;
} Mountain;

Mountain R[MOUNTAIN_COUNT];

void saveMountain(int i, char line[]){
    int separatorIndex = 0;
    while (line[separatorIndex] != ':') {
        ++separatorIndex;
    }
    line[separatorIndex] = 0;
    strcpy(R[i].name, line);
    R[i].height = atoi(&line[separatorIndex+1]);
}

void reservoirSampling(){
    int t = 0;
    FILE* fp = fopen("berge.txt", "r");
    if (fp == 0){
        printf("Error opening file.\n");
        exit(1);
    }

    // Read initial data
    for (int i = 0; i < MOUNTAIN_COUNT; ++i) {
        char lineBuffer[MAX_NAME_LENGTH];
        if(!fgets(lineBuffer, MAX_NAME_LENGTH, fp)) {
            printf("Error reading from file.\n");
        }
        saveMountain(i, lineBuffer);
        ++t;
    }

    char lineBuffer[MAX_NAME_LENGTH];
    while (fgets(lineBuffer, MAX_NAME_LENGTH, fp)){
        ++t;
        int M = rand() % t;
        if (M >= MOUNTAIN_COUNT){
            continue;
        } 
        saveMountain(M, lineBuffer);
    }

    // Print selected mountains (debug)
    for (int i = 0; i < MOUNTAIN_COUNT; i++) {
        printf("%d %s %d\n", i, R[i].name, R[i].height);
    }
}

int sortedMountains[MOUNTAIN_COUNT];


bool isSorted(int mIndex){
    for (int i = 0; i < MOUNTAIN_COUNT; i++){
        if (sortedMountains[i] == mIndex){
            return true;
        }
    }
    return false;
}

int main() {
    srand(time(0));
    reservoirSampling();

    for(int i = 0; i < MOUNTAIN_COUNT; ++i){
        sortedMountains[i] = -1;
    }

    int points = 0;

    while (true){
        printf("Still to be sorted:\n");
        int unsorted = 0;
        for (int i = 0; i < MOUNTAIN_COUNT; i++){
            if (!isSorted(i)){
                ++unsorted;
                printf("\t%d: %s\n", i, R[i].name);
            }
        }
        if (unsorted == 0){
            printf("Congratulations! You sorted all the mountains!\n");
            break;
        }

        printf("What is to be inserted where? ");
        int sourceIndex = 0;
        int targetIndex = 0;
        if (scanf("%d %d", &sourceIndex, &targetIndex) != 2) {
            printf("Invalid input.\n");
            exit(1);
        }

        if (sourceIndex > MOUNTAIN_COUNT-1 || targetIndex > MOUNTAIN_COUNT-1){
            printf("out of bounds\n");
            continue;
        }

        if (isSorted(sourceIndex)){
            printf("Already sorted this mountain.\n");
            continue;
        }

        sortedMountains[targetIndex] = sourceIndex;

        printf("Current state:\n");
        for(int i = 0; i < MOUNTAIN_COUNT; i++) {
            if (sortedMountains[i] != -1){
                printf("\t%d: %s\n", i, R[sortedMountains[i]].name);
            }
        }

        int lastHeight = 0;
        
        // Set lastHeight to at most the height of smallest mountain
        for (int i = 0; i < MOUNTAIN_COUNT; i++){
            if (R[i].height < lastHeight){
                lastHeight = R[i].height;
            }
        }
        
        // Check if a sorted mountain is smaller that the previous mountain.
        bool finished = false;
        for (int i = 0; i < MOUNTAIN_COUNT; i++){
            if (sortedMountains[i] != -1){
                if (R[sortedMountains[i]].height < lastHeight){
                    printf("Sorry, then it is no longer sorted:\n");
                    for(int b = 0; b < MOUNTAIN_COUNT; ++b){
                        if ( sortedMountains[b] != -1){
                            printf("%d \t %s\n", R[sortedMountains[b]].height,
                                    R[sortedMountains[b]].name);
                        }
                    }
                    finished = true;
                    break;
                }
                lastHeight = R[sortedMountains[i]].height; 
            }
        }
        if(finished){
            break;
        }
        ++points;
    }
    printf("Bye!\nYou got %d points.\n", points);
}
