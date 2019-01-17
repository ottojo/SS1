#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

struct file {
    char *path;
    long int size;
};

char *stringConcat(const char *s1, const char *s2) {
    char *result = calloc(1, strlen(s1) + strlen(s2) + 1);
    strcat(result, s1);
    strcat(result, s2);
    return result;
}

char *tripleStringConcat(const char *s1, const char *s2, const char *s3) {
    char *tmp = stringConcat(s1, s2);
    char *result = stringConcat(tmp, s3);
    free(tmp);
    return result;
}

struct element {
    struct file data;
    struct element *next;
};

struct element *list;

void findLargestFile(char *directoryPath) {
    DIR *currentDir = opendir(directoryPath);
    if (!currentDir) {
        perror(directoryPath);
        exit(1);
    }
    struct dirent *directoryEntry;

    while ((directoryEntry = readdir(currentDir))) {
        if (directoryEntry->d_name[0] == '.') {
            continue;
        }

        struct stat s;
        char *filePath = tripleStringConcat(directoryPath, "/", directoryEntry->d_name);
        if (lstat(filePath, &s) != 0) {
            perror(directoryEntry->d_name);
            exit(1);
        }

        if (S_ISDIR(s.st_mode)) {
            // Current directory entry is a directory.
            // Search directory
            findLargestFile(filePath);
            // Free name, as it's not going to be saved
            free(filePath);
            continue;
        }

        struct file f;
        f.path = filePath;
        f.size = s.st_size;

        // List may be empty
        if (!list) {
            list = calloc(1, sizeof(struct element));
            list->data = f;
            list->next = 0;
            continue;
        }

        // List not empty
        struct element *prevElement = 0;
        struct element *nextElement = list;

        while (nextElement && nextElement->data.size > f.size) {
            prevElement = nextElement;
            nextElement = nextElement->next;
        }

        //Insert between prev and next element
        // Prev may be 0
        if (!prevElement) {
            // Insert at start
            list = calloc(1, sizeof(struct element));
            list->data = f;
            list->next = nextElement;
        } else {
            prevElement->next = calloc(1, sizeof(struct element));
            prevElement->next->data = f;
            prevElement->next->next = nextElement;
        }
    }
    closedir(currentDir);
}

int main(int argc, char *argv[]) {
    int c = 10;
    char *directory = ".";

    if (argc == 2) {
        if (argv[1][0] == '-') {
            // Only number supplied
            if (argv[1][1] == '?') {
                printf("Usage: ./du_hogs [-N] [dir]\n");
                exit(EXIT_FAILURE);
            }
            c = atoi(argv[1] + 1);
        } else {
            // Only dir supplied
            directory = argv[1];
        }
    }

    if (argc == 3) {
        // Both number and dir supplied
        c = atoi(argv[1] + 1);
        directory = argv[2];
    }

    findLargestFile(directory);

    int i = 0;
    struct element *e = list;
    while (i < c && e) {
        printf("%10jd %s\n", (intmax_t) e->data.size, e->data.path);
        e = e->next;
        i++;
    }
}
