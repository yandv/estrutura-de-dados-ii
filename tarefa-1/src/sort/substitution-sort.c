#include "../../include/sort.h"

void startSort(FILE *file, const int partitionSize, const char *outputDirectory)
{
    Memory memory = createMemory(file, partitionSize, outputDirectory);

    PartitionElement **fastMemory = memory.fastMemory;
    FILE *partitionFile = memory.partitionFile;

    int partitionCount = 1;

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
        writeClient(firstMin->client, partitionFile);

        if (fastMemory[firstMinIdx] != NULL && fastMemory[firstMinIdx]->client->codigo < firstMin->client->codigo)
        {
            printf("New client %d %s %s %s have been frozen \n", fastMemory[firstMinIdx]->client->codigo, fastMemory[firstMinIdx]->client->nome, fastMemory[firstMinIdx]->client->dataNascimento, fastMemory[firstMinIdx]->frozen ? "(frozen)" : "");
            fastMemory[firstMinIdx]->frozen = true;
        }

        printf("\n");

        printf("Memory state: \n");
        printMemoryState(fastMemory, partitionSize);
    }

    fclose(partitionFile);
    free(fastMemory);
}