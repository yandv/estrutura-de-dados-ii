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

void bubbleSort(Client **clientsArray, int clientsCount)
{
    for (int i = 0; i < clientsCount; i++)
    {
        for (int j = 0; j < clientsCount - i - 1; j++)
        {
            if (clientsArray[j]->codigo > clientsArray[j + 1]->codigo)
            {
                Client *temp = clientsArray[j];
                clientsArray[j] = clientsArray[j + 1];
                clientsArray[j + 1] = temp;
            }
        }
    }
}

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
    Client **clientsArray = (Client **)malloc(clientsCount * sizeof(Client *));

    for (int i = 0; i < clientsCount; i++)
    {
        clientsArray[i] = (Client *)malloc(sizeof(Client));
        clientsArray[i]->codigo = rand() % 10000;
        strcpy(clientsArray[i]->nome, names[rand() % (sizeof(names) / sizeof(names[0]))]);
        clientsArray[i]->idade = rand() % 100 + 1;
    }

    bubbleSort(clientsArray, clientsCount);

    for (int i = 0; i < clientsCount; i++)
    {
        writeClient(clientsArray[i], file);
    }

    printf("%d clients created successfully, listing all of created itens...\n\n", clientsCount);
    rewind(file);

    Client* client = readClient(file);

    while (client != NULL)
    {
        printf("Created client %d %s %d\n", client->codigo, client->nome, client->idade);
        client = readClient(file);
    }

    printf("\nAll %d clients listed\n", clientsCount);

    free(clientsArray);
    free(client);
    fclose(file);
    printf("File \"%s\" closed\n", fileName);
    return 0;
}
