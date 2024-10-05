#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../include/client.h"

int compare(const void *a, const void *b)
{
    return atoi(*(const char **)a + 10) - atoi(*(const char **)b + 10);
}

int main(int argc, char const *argv[])
{
    const char *outputDirectory = argc > 1 ? argv[1] : "bin/output";

    printf("\nProgram configuration:\n");
    printf(" > Output directory: \"%s\"\n\n", outputDirectory);

    DIR *dir;
    struct dirent *ent;
    char *files[1000];
    int count = 0;

    dir = opendir(outputDirectory);

    if (dir == NULL)
    {
        printf("Error opening directory\n");
        return 1;
    }

    while ((ent = readdir(dir)) != NULL)
    {
        if (ent->d_name[0] == '.')
        {
            continue; // Ignora arquivos ocultos e '.' e '..'
        }
        files[count] = malloc(strlen(ent->d_name) + 1);
        strcpy(files[count], ent->d_name);
        count++;
    }

    closedir(dir);

    qsort(files, count, sizeof(char *), compare);

    for (int i = 0; i < count; i++)
    {
        char *filePath = (char *)malloc(strlen(outputDirectory) + strlen(files[i]) + 2);
        sprintf(filePath, "%s/%s", outputDirectory, files[i]);

        FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            printf("Error opening file \"%s\"\n", filePath);
            return 1;
        }

        printf("Partition file \"%s\"\n", files[i]);

        Client *client = readClient(file);
        int totalCount = 0;

        while (client != NULL)
        {
            printf("%d (%s) | ", client->codigo, client->nome);
            totalCount++;
            client = readClient(file);
        }

        printf("Lido %d\n", totalCount);

        free(client);
        fclose(file);
        free(filePath);
    }

    for (int i = 0; i < count; i++)
    {
        free(files[i]);
    }
    return 0;
}
