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
    int idx = -1;

    for (int i = 0; i < partitionSize; i++)
    {
        if (fastMemory[i] != NULL && !fastMemory[i]->frozen)
        {
            if (idx == -1 || fastMemory[i]->client->codigo < fastMemory[idx]->client->codigo)
            {
                idx = i;
            }
        }
    }

    return idx;
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
    printf(" > ");
    for (int i = 0; i < partitionSize; i++)
    {
        if (fastMemory[i] == NULL)
            printf("%d - Empty ", i);
        else
            printf("%d - %d, %s %s", i, fastMemory[i]->client->codigo, fastMemory[i]->client->nome, fastMemory[i]->frozen ? "(frozen)" : "");
        if (i < partitionSize - 1)
            printf("| ");
    }

    printf("\n\n");
}

static Memory createMemory(FILE *file, const int partitionSize, const char *outputDirectory)
{
    PartitionElement **fastMemory = (PartitionElement **)malloc(partitionSize * sizeof(PartitionElement));

    if (fastMemory == NULL)
    {
        printf("Error allocating memory for partition elements\n");
        fclose(file);
        exit(1);
    }

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