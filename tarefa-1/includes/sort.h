#ifndef SORT
#define SORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "bool.h"

typedef struct
{
    Client *client;
    bool frozen;
} PartitionElement;

typedef struct
{
    PartitionElement **fastMemory;
    FILE *partitionFile;
} Memory;

void startSort(FILE *file, const int partitionSize, const char *outputDirectory);

static FILE *createPartitionFile(const char *outputDirectory, const int partitionCount)
{
    char *partitionFullName = (char *)malloc(strlen(outputDirectory) + 20);
    sprintf(partitionFullName, "%s/partition-%d.txt", outputDirectory, partitionCount);
    FILE *partitionFile = fopen(partitionFullName, "wb+");

    if (partitionFile == NULL)
    {
        free(partitionFullName);
        return NULL;
    }

    printf("Partition file \"%s\" created\n\n", partitionFullName);
    free(partitionFullName);

    return partitionFile;
}

static int countNonNullPartitions(PartitionElement **fastMemory, const int partitionSize)
{
    int count = 0;

    for (int i = 0; i < partitionSize; i++)
    {
        if (fastMemory[i] != NULL)
        {
            count++;
        }
    }

    return count;
}

static int findFirstNonFrozenMin(PartitionElement **fastMemory, const int partitionSize)
{
    PartitionElement *min = NULL;

    for (int i = 0; i < partitionSize; i++)
    {
        if (fastMemory[i] != NULL && !fastMemory[i]->frozen)
        {
            if (min == NULL || fastMemory[i]->client->codigo < min->client->codigo)
            {
                return i;
            }
        }
    }

    return -1;
}

static PartitionElement *readNextClient(FILE *file)
{
    Client *client = (Client *)malloc(sizeof(Client));

    if (fread(client, sizeof(Client), 1, file) != 1)
    {
        if (feof(file))
        {
            return NULL;
        }

        printf("Error reading file\n");
        free(client);
        exit(1);
    }

    PartitionElement *partitionElement = (PartitionElement *)malloc(sizeof(PartitionElement));

    partitionElement->client = client;
    partitionElement->frozen = false;

    return partitionElement;
}

static void printMemoryState(PartitionElement **fastMemory, const int partitionSize)
{
    printf("Memory state: \n");

    for (int i = 0; i < partitionSize; i++)
    {
        if (fastMemory[i] == NULL)
            printf("  > %d - Empty\n", i);
        else
            printf("  > %d - Client %d %s %s %s\n", i, fastMemory[i]->client->codigo, fastMemory[i]->client->nome, fastMemory[i]->client->dataNascimento, fastMemory[i]->frozen ? "(frozen)" : "");
    }

    printf("\n");
}

static Memory createMemory(FILE *file, const int partitionSize, const char *outputDirectory)
{
    PartitionElement **fastMemory = (PartitionElement **)malloc(partitionSize * sizeof(PartitionElement));

    for (int i = 0; i < partitionSize; i++)
    {
        fastMemory[i] = readNextClient(file);
    }

    printMemoryState(fastMemory, partitionSize);

    FILE *partitionFile = createPartitionFile(outputDirectory, 1);

    if (partitionFile == NULL)
    {
        printf("Error creating partition file\n");

        fclose(file);

        free(fastMemory);
        free(partitionFile);

        exit(1);
    }

    Memory memory = {fastMemory, partitionFile};
    return memory;
}

#endif