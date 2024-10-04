#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../include/client.h"

const char *names[] = {
    "Alice",
    "Bob",
    "Charlie",
    "David",
    "Eve",
    "Frank",
    "Grace",
    "Heidi",
    "Ivan",
    "Judy",
    "Allan",
    "Mariana",
    "Raquel",
    "Elienar",
    "João Pedro",
    "Sylvino",
    "Ubiratam",
    "Marcel",
    "Filipe Braida",
    "Juliana",
    "Marcelo",
    "Lucas",
    "Rafael",
    "Gustavo",
    "Luis",
    "Pedro",
    "Paulo",
    "Maria",
    "João",
    "José",
    "Carlos",
    "Antônio",
    "Francisco",
    "Ana",
    "Luiza",
    "Luiz",
    "Fernando",
};

int main(int argc, char const *argv[])
{
    const int clientsCount = argc > 1 ? atoi(argv[1]) : 10;
    const char *fileName = argc > 2 ? argv[2] : "resources/clients.txt";

    if (clientsCount <= 0)
    {
        printf("Invalid partition size, must be greater than 0\n");
        return 1;
    }

    printf("\nProgram configuration:\n");
    printf(" > Client Count: %d\n", clientsCount);
    printf(" > File name: \"%s\"\n", fileName);

    FILE *file = fopen(fileName, "wb+");

    if (file == NULL)
    {
        printf("Error opening file \"%s\"\n", fileName);
        return 1;
    }

    printf("File \"%s\" opened, starting creation of %d clients...\n", fileName, clientsCount);

    srand(time(NULL));
    Client *client = (Client *)malloc(sizeof(Client));

    for (int i = 0; i < clientsCount; i++)
    {
        client->codigo = rand() % 10000;
        strcpy(client->nome, names[rand() % (sizeof(names) / sizeof(names[0]))]);
        sprintf(client->dataNascimento, "%02d/%02d/%04d", rand() % 30 + 1, rand() % 12 + 1, rand() % 130 + (2024 - 130));
        writeClient(client, file);
    }

    printf("%d clients created successfully, listing all of created itens...\n\n", clientsCount);
    rewind(file);

    while ((client = readClient(file)) != NULL)
    {
        printf("Created client %d %s %s\n", client->codigo, client->nome, client->dataNascimento);
    }

    printf("\nAll %d clients listed\n", clientsCount);

    fclose(file);
    printf("File \"%s\" closed\n", fileName);
    return 0;
}
