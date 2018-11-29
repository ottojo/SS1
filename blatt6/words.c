#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STRING_BUFFER 200

struct Word {
    struct Word *nextFirst;
    struct Word *nextLast;
    bool used;
    char word[];
};

struct WordSet {
    struct Word *byFirstLetter[26];
    struct Word *byLastLetter[26];
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

// Count number of words stored in wordset
int wordCount(struct WordSet *ws) {
    int i = 0;
    for (int b = 0; b<26; ++b){
        if (ws->byFirstLetter[b] != 0) {
            struct Word *w = ws->byFirstLetter[b];
            do {
                ++i;
                w = w->nextFirst;
            } while (w != 0);
        }
    }
    return i;
}

// Returns c in lower case if c is an upper case character
char lowerCase(char c) {
    if (c <= 90 && c >=65){
        return c + 32;
    }
    return c;
}

// Returns true if strings are equal, case insensitive
bool equalIgnoreCase(char *w1, char *w2){
    int i = 0;
    while(w1[i] != 0 && w2[i] != 0){
        if (lowerCase(w1[i]) != lowerCase(w2[i])) {
            return false;
        }
        ++i;
    }
    return w1[i] == w2[i];
}

// Returns a pointer to the given word in the wordset
struct Word* findWord(struct WordSet *ws, char *w){
    struct Word *wp = ws->byFirstLetter[charToIndex(w[0])];
    while (wp != 0) {
        if (equalIgnoreCase(w, wp->word)) {
            return wp;
        }
        wp = wp->nextFirst;
    }
    return 0;
}

// Returns length of string
int wordLength(char word[]){
    int i = 0;
    while(word[i]!=0){
        ++i;
    }
    return i;
}

// Reads from stdin until newline, max length of STRING_BUFFER
char* readLine(char dest[]) {
    char *c = fgets(dest, STRING_BUFFER, stdin);
    dest[STRING_BUFFER-1]=0;
    for (int i = 0; dest[i] != 0; i++) {
        if (dest[i] == '\r' || dest[i] == '\n') {
            dest[i] = 0;
            break;
        }
    }
    return c;
}

struct WordSet ws;
struct Word *leftWord;
struct Word *rightWord;

int main() {

    // Read words from words.txt, insert into wordset
    FILE *fp = fopen("words.txt", "r");
    if (!fp) {
        printf("COULD NOT OPEN FILE\n");
    }
    char buf[STRING_BUFFER];
    while (fgets(buf, STRING_BUFFER, fp)) {
        int i = 0;
        while (buf[i] != 0 && buf[i] != '\r' && buf[i] != '\n' && i<STRING_BUFFER){
            ++i;
        }
        buf[i] = 0;
        insertWord(&ws, buf);
    }

    // Print all words sorted by first and last letter (debug)
    printf("%d words\n", wordCount(&ws));
    printf("By First Letter:\n"); 
    for(int i = 0; i < 26; ++i){
        if (ws.byFirstLetter[i]!=0){
            printf("%c:\n", i+65);
            struct Word *w = ws.byFirstLetter[i];
            int d = 0;
            do{
                printf("%d %s\n", d, w->word);
                w = w->nextFirst;
                d++;
            } while (w != 0);
        }
    }

    printf("\nBy Last Letter:\n"); 
    for(int i = 0; i < 26; ++i){
        if (ws.byLastLetter[i]!=0){
            printf("%c:\n", i+65);
            struct Word *w = ws.byLastLetter[i];
            int d = 0;
            do{
                printf("%d %s\n", d, w->word);
                w = w->nextLast;
                d++;
            } while (w != 0);
        }
    }

    // First word
    struct Word *selectedWord = 0;    
    while (!selectedWord) {
        printf("Choose the first word!\n");
        if (!readLine(buf)){
            printf("ERROR READING WORD\n");
        } else {
            selectedWord = findWord(&ws, buf);    
        }
    }
    selectedWord->used = true;
    leftWord = selectedWord;
    rightWord = selectedWord;
    // Game
    while (true) {
        char currentFirst = leftWord->word[0];
        char currentLast = rightWord->word[wordLength(rightWord->word)-1];
        
        // Test if valid words exits
        int usableWords = 0;

        struct Word* w = ws.byLastLetter[charToIndex(currentFirst)];
        while (w) {
            if (!w->used) {
                printf("Usable word: %s\n", w->word);
                usableWords++;
            }

            w = w->nextLast;
        }

        w = ws.byFirstLetter[charToIndex(currentLast)];
        while (w) {
            if (!w->used) {
                printf("Usable word: %s\n", w->word);
                usableWords++;
            }
            w = w->nextFirst;
        }

        if (usableWords == 0) {
            printf("Game end. No valid word found.\n");
            exit(0); //?
            printf("STARTING NEW GAME\n");

            // First word (restarting game)
            struct Word *selectedWord = 0;    
            while (!selectedWord || selectedWord->used) {
                printf("Choose the first word!\n");
                if (!readLine(buf)){
                    printf("ERROR READING WORD\n");
                } else {
                    if (equalIgnoreCase(buf, "Bye.")) {
                        exit(0);
                    }
                    selectedWord = findWord(&ws, buf);    
                }
            }
            selectedWord->used = true;
            leftWord = selectedWord;
            rightWord = selectedWord;
            continue;
        } else {
            printf("%d valid words left.\n", usableWords);
        }

        printf("%s ... %s\n", leftWord->word, rightWord->word);
        selectedWord = 0;
        while (!selectedWord || selectedWord->used) {
            printf("Next word: ");
            if (!readLine(buf)){
                printf("ERROR READING WORD\n");
            }
            selectedWord = findWord(&ws, buf);    
        }
        char selectedLast = selectedWord->word[wordLength(selectedWord->word)-1];
        char selectedFirst = selectedWord->word[0];

        if (lowerCase(selectedLast) == lowerCase(currentFirst)) {
            leftWord = selectedWord;
            selectedWord->used = true;
        } else if (lowerCase(selectedFirst) == lowerCase(currentLast)) {
            rightWord = selectedWord;
            selectedWord->used = true;
        }
    }
}
