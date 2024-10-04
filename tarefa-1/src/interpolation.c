#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>

#include <dirent.h>
#include <string.h>

#include "../include/interpolation.h"

int main(int argc, char const *argv[])
{
    const char *inputPartitionsDirectory = argc > 1 ? argv[1] : "bin/output";
    const char *fileName = argc > 2 ? argv[2] : "bin/output/ordered/clients.txt";

    printf("\nInterpolation program configuration:\n");
    printf(" > Input partitions directory: \"%s\"\n", inputPartitionsDirectory);
    printf(" > Output file: \"%s\"\n\n", fileName);

    struct stat st = {0};
    char *outputDirectory = (char *)malloc(strlen(fileName) + 1);

    strcpy(outputDirectory, fileName);

    char *lastSlash = strrchr(outputDirectory, '/');

    if (lastSlash != NULL)
    {
        *lastSlash = '\0';
    }

    if (stat(outputDirectory, &st) == -1)
    {
        mkdir(outputDirectory, 0700);
    }

    FILE *outputFile = fopen(fileName, "wb+");

    if (outputFile == NULL)
    {
        printf("Error creating output file\n");
        return 1;
    }

    DIR *dir;
    struct dirent *ent;

    dir = opendir(inputPartitionsDirectory);

    if (dir == NULL)
    {
        fclose(outputFile);
        printf("Error opening directory\n");
        return 1;
    }

    int inputFilesCount = 0;
    int inputFilesMaxCount = 10;

    FILE **inputFiles = (FILE **)malloc(inputFilesMaxCount * sizeof(FILE *));

    while ((ent = readdir(dir)) != NULL)
    {
        char *filePath = (char *)malloc(strlen(inputPartitionsDirectory) + strlen(ent->d_name) + 2);
        sprintf(filePath, "%s/%s", inputPartitionsDirectory, ent->d_name);

        if (stat(filePath, &st) > 0)
        {
            printf("Could not load %s informations, skipping...\n", filePath);
            continue;
        }

        if (ent->d_name[0] == '.' || !S_ISREG(st.st_mode))
        {
            continue;
        }

        if (inputFilesCount == inputFilesMaxCount)
        {
            inputFilesMaxCount *= 2;
            inputFiles = (FILE **)realloc(inputFiles, inputFilesMaxCount * sizeof(FILE *));
        }

        printf("Added %s file to partition list.\n", ent->d_name);
        inputFiles[inputFilesCount] = fopen(filePath, "rb");
        inputFilesCount++;
    }

    closedir(dir);
    printf("\n");

    inputFiles = (FILE **)realloc(inputFiles, inputFilesCount * sizeof(FILE *));

    /*
     * Implementação básica é ler sempre o primeiro próximo cliente dos arquivos de partição e
     * escrever o menor deles no arquivo de saída
     */

    printf("Starting interpolation....\n\n");

    startInterpolation(outputFile, inputFiles, inputFilesCount);

    printf("Interpolation finished!\n");

    for (int i = 0; i < inputFilesCount; i++)
    {
        fclose(inputFiles[i]);
    }

    fclose(outputFile);

    return 0;
}
