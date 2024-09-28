#include "../../includes/sort.h"

void startSort(FILE *file, const int partitionSize, const char *outputDirectory)
{
    PartitionElement **fastMemory = (PartitionElement **)malloc(partitionSize * sizeof(PartitionElement));

    for (int i = 0; i < partitionSize; i++)
    {
        fastMemory[i] = readNextClient(file);
    }

    printMemoryState(fastMemory, partitionSize);

    int partitionCount = 1;

    FILE *partitionFile = createPartitionFile("bin/output", partitionCount);

    if (partitionFile == NULL)
    {
        printf("Error creating partition file\n");

        fclose(file);

        free(fastMemory);
        free(partitionFile);

        exit(1);
    }

    while (countNonNullPartitions(fastMemory, partitionSize) > 0)
    {
        int firstMinIdx = findFirstNonFrozenMin(fastMemory, partitionSize);

        if (firstMinIdx == -1)
        {
            fclose(partitionFile);

            for (int i = 0; i < partitionSize; i++)
            {
                if (fastMemory[i] != NULL)
                {
                    fastMemory[i]->frozen = false;
                }
            }

            partitionFile = createPartitionFile(outputDirectory, ++partitionCount);

            if (partitionFile == NULL)
            {
                fclose(file);
                printf("Error creating partition file\n");
                exit(1);
            }
            continue;
        }

        PartitionElement *firstMin = fastMemory[firstMinIdx];
        fastMemory[firstMinIdx] = readNextClient(file);

        printf("Client %d %s %s %s written to partition %d\n", firstMin->client->codigo, firstMin->client->nome, firstMin->client->dataNascimento, firstMin->frozen ? "(frozen)" : "", partitionCount);
        fwrite(firstMin->client, sizeof(Client), 1, partitionFile);

        if (fastMemory[firstMinIdx] != NULL && fastMemory[firstMinIdx]->client->codigo < firstMin->client->codigo)
        {

            printf("New client %d %s %s %s have been frozen \n", fastMemory[firstMinIdx]->client->codigo, fastMemory[firstMinIdx]->client->nome, fastMemory[firstMinIdx]->client->dataNascimento, fastMemory[firstMinIdx]->frozen ? "(frozen)" : "");
            fastMemory[firstMinIdx]->frozen = true;
        }

        printf("\n");

        printMemoryState(fastMemory, partitionSize);
    }
}