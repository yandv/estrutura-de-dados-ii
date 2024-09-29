#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../includes/client.h"

int compare(const void *a, const void *b)
{
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;

    int num1 = atoi(str1 + 10);
    int num2 = atoi(str2 + 10);

    return num1 - num2;
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

        Client *client = (Client *)malloc(sizeof(Client));

        while (fread(client, sizeof(Client), 1, file))
        {
            printf("%d ", client->codigo);
        }

        printf("\n");

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
