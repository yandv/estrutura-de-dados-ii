#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>

#define CLIENT_NAME_SIZE 50

typedef struct
{
    int codigo;
    char nome[CLIENT_NAME_SIZE];
    int idade;
} Client;

static size_t writeClient(Client *client, FILE *file)
{
    return fwrite(client, sizeof(Client), 1, file);
}

static Client *readClient(FILE *file)
{
    Client *client = (Client *)malloc(sizeof(Client));

    if (client == NULL)
    {
        return NULL;
    }

    if (fread(client, sizeof(Client), 1, file) != 1)
    {
        if (feof(file))
        {
            return NULL;
        }

        printf("Error reading client in file\n");
        free(client);
        exit(1);
    }

    return client;
}

static void* readElementFromFile(FILE *file, size_t size) {
    void *element = malloc(size);

    if (element == NULL)
    {
        printf("aq\n");
        return NULL;
    }

    if (fread(element, size, 1, file) != 1)
    {
        if (feof(file))
        {
            return NULL;
        }

        printf("Error reading element in file\n");
        free(element);
        exit(1);
    }

    return element;
}

#endif