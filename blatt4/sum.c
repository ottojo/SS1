#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//#define DEBUG

double readDouble();

int main(){
    double sum = 0;
    double kahan_sum = 0;
    double c = 0;
    while (1){
        double read = readDouble();
        sum += read;
        double y = read - c;
        double t = kahan_sum + y;
        c = (t - kahan_sum) - y;
        kahan_sum = t;
        printf("Regular sum: %.17lg\n", sum);
        printf("Kahan sum: %.17lg\n", kahan_sum);
        printf("Difference: %lg\n", fabs(sum - kahan_sum));
    }
}

double readDouble(){
    int state = 0;
    int sign = 1;              // Vorzeichen
    double unsignedResult = 0; // Vorzeichenloses Resultat
    int decimals = 0;          // Anzahl der Dezimalstellen nach dem Komma
    int exponent = 0;          // Exponent
    int exponentSign = 1;      // Vorzeichen des Exponenten
    while (1){
        int couldUseChar = 1;
        int inputchar = getchar();
        if (inputchar == EOF){
#ifdef DEBUG
            printf("[readDouble] EOF\n"); 
#endif
            exit(0);
            break;
        }
#ifdef DEBUG
        printf("[readDouble] Read %c\n", inputchar); 
        printf("[readDouble] State %d\n", state);
#endif
        switch (state){
            case 0:
                // First char
                switch (inputchar){
                    case '-':
                        sign = -1;
                        state = 1;
                        break;
                    case '+':
                        sign = 1;
                        state = 1;
                        break;
                    case '.':
                        state = 3;
                        break;
                    case '0' ... '9':
                        unsignedResult = (inputchar - '0') + unsignedResult * 10;
                        state = 2;
                        break;
                    default:
                        couldUseChar = 0;
                        break;
                }
                break;
            case 1:
                state = 2;
            case 2:
                // Numbers before decimal point
                switch (inputchar){
                    case '0' ... '9':
                        unsignedResult = (inputchar - '0') + unsignedResult * 10;
                        break;
                    case '.':
                        state = 4;
                        break;
                    case 'e':
                    case 'E':
                        state = 5;
                        break;
                    default:
                        couldUseChar = 0;      
                        break;
                }
                break;
            case 3:
                state = 4;
            case 4:
                // Numbers behind decimal point
                switch (inputchar){
                    case '0' ... '9':
                        unsignedResult += ((double) (inputchar - '0')) / pow(10, decimals+1);
                        decimals++;
                        break;
                    case 'e':
                    case 'E':
                        state = 5;
                        break;
                    default:
                        couldUseChar = 0;
                }
                break;
            case 5:
                // Start of exponent
                switch (inputchar){
                    case '+':
                        exponentSign = 1;
                        state = 6;
                        break;
                    case '-':
                        exponentSign = -1;
                        state = 6;
                        break;
                    case '0' ... '9':
                        exponent = exponent * 10 + (inputchar - '0');
                        state = 7;
                        break;
                    default:
                        couldUseChar = 0;
                        break;
                }
                break;
            case 6:
                state = 7;
            case 7:
                switch (inputchar){
                    case '0' ... '9':
                        exponent = exponent * 10 + (inputchar - '0');
                        break;
                    default:
                        couldUseChar = 0;;
                        break;
                }
                break;
        }
        if (couldUseChar) {
#ifdef DEBUG
            printf("[readDouble] Intermediate Result: %f\n", sign * unsignedResult);
            printf("[readDouble] Exponent: %d\n", exponentSign * exponent);
#endif
        } else if (state == 2 || state == 4 || state == 7) {
#ifdef DEBUG
            printf("[readDouble] Got invalid char at valid end state. Returning.\n");
#endif
            ungetc(inputchar, stdin);
            break;
        } else {
#ifdef DEBUG
            printf("[readDouble] Got invalid char at invalid end state. Continuing.\n");
#endif
        }
    }
#ifdef DEBUG
    printf("[readDouble] Result: %.17f\n",sign*unsignedResult * pow(10, exponentSign * exponent)); 
#endif
    return sign*unsignedResult * pow(10, exponentSign * exponent);
}
