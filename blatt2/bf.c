#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int kontostand = 1000;
    srand(time(NULL));

    printf("***** Banca Francesa *****\n");
    printf("In jeder Runde koennen Sie einen Teil Ihres Geldes auf eine der folgenden Kombinationen setzen:\n");
    printf(" 1: Aces, Augensumme: 3 mit einer Gewinnquote von 1 : 61.\n");
    printf("2: Pequeno, Augensumme: 5 6 7 mit einer Gewinnquote von 1 : 1.\n");
    printf("3: Grande, Augensumme: 14 15 16 mit einer Gewinnquote von 1 : 1."); 

    while (kontostand > 0) {

        printf("Kontostand: %d\n", kontostand);
        printf("Einsatz und Kombination: ");

        int kombination;
        int einsatz;

        int scanResult = scanf("%d %d", &einsatz, &kombination);
        switch (scanResult) {
            case EOF:
                printf("\nEnd of file.\n");
                exit(1);
            case 2:
                break;
            default:
                printf("Invalid input.\n");
                exit(1);
        }

        if (einsatz == 0){
            return 0;
        }

        if (einsatz < 0 || einsatz > kontostand || kombination < 1 || kombination > 3) {
            printf("Ungültige Einabe.\n");
            continue;
        }

        int gewinn = 0;
        while (1) {
            printf("Wuerfel: ");
            int summe = 0;
            for (int i = 0; i < 3; i++) {
                int augenzahl = rand() % 6 + 1;
                printf("%d ", augenzahl);
                summe += augenzahl;
            }
            printf("mit einer Augensumme von %d.\n", summe);

            if (summe == 3){
                printf("Aces!\n");
                if (kombination == 1) {
                    gewinn = 61 * einsatz;
                }

                break;
            } else if (summe >= 5 && summe <= 7) {
                printf("Pequeno!\n");
                if (kombination == 2) {
                    gewinn = einsatz;
                }
                break;
            } else if (summe >= 14 && summe <= 16) {
                printf("Grande!\n");
                if (kombination == 3) {
                    gewinn = einsatz;
                }
                break;
            } else {
                printf("Nichts passiert.\n");
            }
        }

        if (gewinn > 0) {
            printf("Sie gewinnen %d!\n", gewinn);
            kontostand += gewinn;
        } else {
            printf("Sie verlieren %d!\n", einsatz);
            kontostand -= einsatz;
        }
    }
}
