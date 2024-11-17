#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/client.h"
#include "../include/sort.h"

struct stat st = {0};

void startRoutine(FILE *file, const int partitionSize, const char *outputDirectory);

const bool isDebugMode = true;

int main(int argc, char const *argv[])
{
    const int partitionSize = argc > 1 ? atoi(argv[1]) : 10;
    const char *fileName = argc > 2 ? argv[2] : "resources/clients.txt";
    const char *outputDirectory = argc > 3 ? argv[3] : "bin/output";

    if (partitionSize <= 0)
    {
        printf("Invalid partition size, must be greater than 0\n");
        return 1;
    }

    printf("\nProgram configuration:\n");
    printf(" > Partition size: %d\n", partitionSize);
    printf(" > File name: \"%s\"\n", fileName);
    printf(" > Output directory: \"%s\"\n\n", outputDirectory);

    if (stat(outputDirectory, &st) == -1)
    {
        mkdir(outputDirectory, 0700);
        printf("Partition output directory \"%s\" created successfully\n", outputDirectory);
    }

    FILE *file = fopen(fileName, "r");

    if (file == NULL)
    {
        printf("Error opening file \"%s\"\n", fileName);
        return 1;
    }

    printf("File \"%s\" opened\n", fileName);
    printf("Starting %s sort routine...\n\n", "select");

    startSort(file, partitionSize, outputDirectory);

    printf("Sort routine finished\n");

    fclose(file);
    printf("File \"%s\" closed\n", fileName);
    return 0;
}