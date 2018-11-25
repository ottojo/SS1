#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Word {
    struct Word *nextFirst;
    struct Word *nextLast;
    char word[];
};

struct WordSet {
    struct Word *byFirstLetter[25];
    struct Word *byLastLetter[25];
};

int charToIndex(char c){
    if (c > 90){
        c -= 32;
    }
    c -= 65;
    return c;
}


void insertWord(struct WordSet *ws, char word[]){
    //Allocate memory
    int wordLength = 0;
    while (word[wordLength]){
        ++wordLength;
    }
    ++wordLength; // Nullbyte
    struct Word *wordptr = calloc(1, sizeof(struct Word) + sizeof(char[wordLength]));
    if(!wordptr){
        printf("COULD NOT ALLOCATE MEMORY\n");
        exit(1);
    }
    
    //Copy data
    strcpy(wordptr->word, word);
    
    //Insert into first-letter index
    int firstLetterIndex = charToIndex(word[0]);
    if(ws->byFirstLetter[firstLetterIndex] == 0){
        ws->byFirstLetter[firstLetterIndex] = wordptr;
    } else {
        struct Word *c = ws->byFirstLetter[firstLetterIndex];

        while(c->nextFirst != 0){
            c = c->nextFirst;
        }
        c->nextFirst = wordptr;
    }

    //Insert into last-letter index
    int lastLetterIndex = charToIndex(word[wordLength-2]);
    if(ws->byLastLetter[lastLetterIndex] == 0){
        ws->byLastLetter[lastLetterIndex] = wordptr;
    } else {
        struct Word *c = ws->byLastLetter[lastLetterIndex];
        while(c->nextLast != 0){
            c = c->nextLast;
        }
        c->nextLast = wordptr;
    }
}

struct WordSet ws;

int main() {

    insertWord(&ws, "test");
    insertWord(&ws, "ates^1t");
    insertWord(&ws, "ste3st");
    insertWord(&ws, "ftes3t");
    insertWord(&ws, "dtest2a");
    insertWord(&ws, "dtest3a");
    insertWord(&ws, "dtest4a");

    printf("By First Letter:\n"); 
    for(int i = 0; i < 26; ++i){
        printf("%c:\n", i+65);
        if (ws.byFirstLetter[i]!=0){
            struct Word *w = ws.byFirstLetter[i];
            int d = 0;
            do{
                printf("%d %s\n", d, w->word);
                w = w->nextFirst;
                d++;
            } while (w != 0);
        }
    }

    printf("By Last Letter:\n"); 
    for(int i = 0; i < 26; ++i){
        printf("%c:\n", i+65);
        if (ws.byLastLetter[i]!=0){
            struct Word *w = ws.byLastLetter[i];
            int d = 0;
            do{
                printf("%d %s\n", d, w->word);
                w = w->nextLast;
                d++;
            } while (w != 0);
        }
    }
}
