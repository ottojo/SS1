#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define MIN_KEGEL 10
#define MAX_KEGEL 15

unsigned long int playingField;
int pinCount;
typedef enum { USER, COMPUTER} player;

// Executes move, does not check if move is valid
void executeMove(int z) {
    if(z>0){
        playingField = ~(~playingField | 1<<(z-1));
    } else {
        z = -z;
        playingField = ~(~playingField | 0b11<<(z-1));
    }
}

void printField(){
    for (int i = 0; i < pinCount; i++) {
        if(playingField & (1<<i)){
            printf("/\\ ");
        } else {
            printf("   ");
        }
    }
    printf("\n");
    for (int i = 0; i < pinCount; i++) {
            printf("%2d ",i+1);
    }
    printf("\n");
}

// Check if move is valid and print error message if printError == true.
bool isValidMove(int z, bool printError){
    if (z==0 || z>pinCount || z < -pinCount+1){
        if (printError) {printf("Invalid pin number\n");}
        return false;
    }
    if (z > 0){
        if (!(playingField & 1<<(z-1))){
            if (printError) {printf("Pin %d is not longer standing.\n", z);}
            return false;
            }
    } else {
        z=-z;
        if (! ((playingField & 1<<(z-1)) && (playingField & 1<<z))){
            if (printError) {printf("Pins %d and %d are not longer standing.\n", z, z+1);}
            return false;
        }
    }
    return true;
}

int main(){
    srand(time(0));
    
    // Init field
    pinCount = rand() % (MAX_KEGEL - MIN_KEGEL + 1) + MIN_KEGEL;
    playingField = 0xFFFFFFFFul>>(32-pinCount);

    player currentPlayer = USER;

    while(playingField){
        printField(playingField);
        switch (currentPlayer){
            case USER:
                {
                    printf("Your move: ");
                    int z;
                    if (scanf("%d", &z) != 1){
                        printf("Ungültige Eingabe\n");
                        exit(1);
                    }
                    if(isValidMove(z, true)){
                           executeMove(z);
                           currentPlayer = COMPUTER;
                    } else {
                        continue;
                    }
                    break;
                    case COMPUTER:
                    {
                        int randMove;
                        do {
                            randMove = rand() % (2*pinCount+1) - pinCount;
                        } while (!isValidMove(randMove, false));
                        printf("My move: %d\n", randMove);
                        executeMove(randMove);
                        currentPlayer = USER;
                    }
                    break;
                }
        }
    }
    printf("%s has won.\n", currentPlayer == USER ? "Computer" : "Player");
    return 0;
}

