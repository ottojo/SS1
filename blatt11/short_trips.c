#include <stdio.h>
#include <stralloc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define HASHMAP_LENGTH 200
#define GEMEINDENFILE "gemeinden.txt"

typedef struct city {
    stralloc name;
    double latitude;
    double longitude;
} city;

typedef struct sameHashList {
    struct sameHashList *next;
    city data;
} sameHashList;

sameHashList hashMap[HASHMAP_LENGTH];

void print(stralloc *sa) {
    int i;
    for (i = 0; i < sa->len; i++)
        putchar(sa->s[i]);
}

/*
unsigned int calcHash(const stralloc string) {
    int hash = 0;
    for (int i = 0; i < string.len; i++) {
        hash += (int) string.s[i];
    }
    return hash % HASHMAP_LENGTH;
}
*/

/* hash algorithm from Dan Bernstein's cdbhash.c */
#define HASHSTART 5381

static unsigned int calcHash(stralloc s) {
    int hashval = HASHSTART;
    for (unsigned int index = 0; index < s.len; ++index) {
        char ch = s.s[index];
        hashval += hashval << 5;
        hashval ^= ch;
    }
    return (unsigned int) (hashval) % HASHMAP_LENGTH;
}

/**
 * Searches for a city in the hashmap by name
 * @param name Name of the city
 * @return 0 if city was not found
 */
city *getCity(stralloc name) {
    sameHashList *city = &hashMap[calcHash(name)];
    while (!stralloc_equal(&name, &(city->data.name))) {
        city = city->next;
        if (!city) return 0;
    }
    return &(city->data);
}

int readline(FILE *fp, stralloc *sa) {
    sa->len = 0;
    for (;;) {
        if (!stralloc_readyplus(sa, 1)) return 0;
        if (fread(sa->s + sa->len, sizeof(char), 1, fp) <= 0) return 0;
        if (sa->s[sa->len] == '\n') break;
        ++sa->len;
    }
    return 1;
}

double distance_meter(city c1, city c2) {
    if (c1.latitude == c2.latitude && c1.longitude == c2.longitude) {
        return 0;
    }

    double lat1rad = c1.latitude * 0.017453292519943295769;
    double long1rad = c1.longitude * 0.017453292519943295769;

    double lat2rad = c2.latitude * 0.017453292519943295769;
    double long2rad = c2.longitude * 0.017453292519943295769;

    double F = (lat1rad + lat2rad) / 2;
    double G = (lat1rad - lat2rad) / 2;
    double l = (long1rad - long2rad) / 2;

    double f = 3.3528106647474807198455286185205559557326361780097638e-12;
    double alpha = 6378137;
    double sinG = sin(G);
    double sinF = sin(F);
    double sinl = sin(l);
    double cosG = cos(G);
    double cosF = cos(F);
    double cosl = cos(l);

    double S = sinG * sinG * cosl * cosl + cosF * cosF * sinl * sinl;
    double C = cosG * cosG * cosl * cosl + sinF * sinF * sinl * sinl;

    double w = atan(sqrt(S / C));
    double D = 2 * w * alpha;

    double T = sqrt(S * C) / w;

    double H1 = (3 * T - 1) / (2 * C);
    double H2 = (3 * T + 1) / (2 * S);

    return D * (1 + f * H1 * sinF * sinF * cosG * cosG - f * H2 * cosF * cosF * sinG * sinG);
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: %s Startcity Endcity\n", argv[0]);
        exit(0);
    }

    FILE *fp = fopen(GEMEINDENFILE, "r");
    if (!fp) {
        perror(GEMEINDENFILE);
        exit(1);
    }

    // File parsing
    stralloc sa = {0};
    while (readline(fp, &sa)) {
        city newCity = {0};

        // Copy City Name:
        // Search first divider char
        size_t firstDivIndex = 0;
        while (sa.s[firstDivIndex] != ':') {
            ++firstDivIndex;
        }

        if (!stralloc_copyb(&(newCity.name), sa.s, firstDivIndex)) {
            perror(sa.s);
            return 0;
        }

        // Parse Latitude (between first and second divider)
        char *latText = &sa.s[firstDivIndex + 1];
        // Search second divider
        size_t secondDivOffset = 0;
        while (latText[secondDivOffset] != ':') {
            ++secondDivOffset;
        }
        latText[secondDivOffset] = 0;
        newCity.latitude = atof(latText);
        // Null terminate for parsing
        stralloc_0(&sa);
        // Parse Longitude (after second divider, 0 terminated)
        newCity.longitude = atof(&latText[secondDivOffset + 1]);


        unsigned int cityHash = calcHash(newCity.name);


        if (hashMap[cityHash].data.name.len == 0) {
            hashMap[cityHash].data = newCity;
            hashMap[cityHash].next = NULL;
        } else {
            int elementCount = 1;
            sameHashList *lastElement = &hashMap[cityHash];
            while (lastElement->next != NULL) {
                elementCount++;
                lastElement = lastElement->next;
            }

            //   printf("Appending to list of length %d\n", elementCount);
            lastElement->next = calloc(1, sizeof(sameHashList));
            lastElement->next->data = newCity;
        }
        stralloc_init(&sa);
    }
    fclose(fp);

    // Game Setup
    stralloc startCityName = {0};
    stralloc endCityName = {0};

    double totalDistanceKM = 0;
    double maxHop = 0;

    stralloc_copys(&startCityName, argv[1]);
    stralloc_copys(&endCityName, argv[2]);

    city *startCity = getCity(startCityName);
    city *endCity = getCity(endCityName);
    city *currentCity = startCity;

    if (startCity == 0) {
        printf("Invalid starting city.\n");
        exit(1);
    }
    if (endCity == 0) {
        printf("Invalid target city.\n");
        exit(1);
    }


    // Game
    do {
        printf("You are currently located in ");
        print(&currentCity->name);
        printf(".\n");

        city *nextCity = 0;
        stralloc nextCityName = {0};

        do {
            printf("Next town? ");
            fflush(stdout);
            readline(stdin, &nextCityName);
            nextCity = getCity(nextCityName);
        } while (nextCity == 0);


        double hopDistance = distance_meter(*currentCity, *nextCity) / 1000.0;
        totalDistanceKM += hopDistance;
        if (hopDistance > maxHop) maxHop = hopDistance;

        printf("Distance from ");
        print(&currentCity->name);
        printf(" to ");
        print(&nextCity->name);
        printf(": %.1f km\n", hopDistance);

        currentCity = nextCity;

    } while (currentCity != endCity);

    printf("Welcome to ");
    print(&endCity->name);
    printf("!\n Your maximal intermediate distance was %.1f km\n", maxHop);
}