#include "../../includes/sort.h"

/**
 * Esse algoritmo considera o tamanho do reservatório (n) igual ao número de partições (M)
 */

int findFirstNullReservoirSpace(PartitionElement **partitionReservoir, const int partitionSize)
{
    for (int i = 0; i < partitionSize; i++)
    {
        if (partitionReservoir[i] == NULL)
        {
            return i;
        }
    }

    return -1;
}

void startSort(FILE *file, const int partitionSize, const char *outputDirectory)
{
    Memory memory = createMemory(file, partitionSize, outputDirectory);

    PartitionElement **fastMemory = memory.fastMemory;
    FILE *partitionFile = memory.partitionFile;

    PartitionElement **partitionReservoir = (PartitionElement **)malloc(partitionSize * sizeof(PartitionElement));

    if (partitionReservoir == NULL)
    {
        printf("Error creating partition reservoir\n");
        free(fastMemory);
        free(partitionFile);
        fclose(file);
        fclose(partitionFile);
        exit(1);
    }

    for (int i = 0; i < partitionSize; i++)
    {
        partitionReservoir[i] = NULL;
    }

    int partitionCount = 1;

    while (countNonNullPartitions(fastMemory, partitionSize) + countNonNullPartitions(partitionReservoir, partitionSize) > 0)
    {
        int firstMinIdx = findFirstNonFrozenMin(fastMemory, partitionSize);

        if (firstMinIdx == -1) // esvazia o reservatório, poderia já escrever tudo que está no reservatório em ordem crescente, mas assim já está bom
        {
            printf("No more elements in fast memory... moving all elements in reservoir to fast memory\n");

            for (int i = 0; i < partitionSize; i++)
            {
                if (partitionReservoir[i] != NULL)
                {
                    fastMemory[i] = partitionReservoir[i];
                    partitionReservoir[i] = NULL;
                }
            }

            firstMinIdx = findFirstNonFrozenMin(fastMemory, partitionSize);
        }

        PartitionElement *firstMin = fastMemory[firstMinIdx];
        fastMemory[firstMinIdx] = readNextClient(file);

        printf("Client %d %s %s written to partition %d\n", firstMin->client->codigo, firstMin->client->nome, firstMin->client->dataNascimento, partitionCount);

        if (fastMemory[firstMinIdx] != NULL)
        {
            printf("New client %d %s %s read from file\n", fastMemory[firstMinIdx]->client->codigo, fastMemory[firstMinIdx]->client->nome, fastMemory[firstMinIdx]->client->dataNascimento);
        }

        fwrite(firstMin->client, sizeof(Client), 1, partitionFile);

        int firstNullReservoirSpaceIdx = findFirstNullReservoirSpace(partitionReservoir, partitionSize);

        while (fastMemory[firstMinIdx] != NULL && fastMemory[firstMinIdx]->client->codigo < firstMin->client->codigo && firstNullReservoirSpaceIdx >= 0)
        {
            printf("New client %d %s %s %s moved to reservoir \n", fastMemory[firstMinIdx]->client->codigo, fastMemory[firstMinIdx]->client->nome, fastMemory[firstMinIdx]->client->dataNascimento, fastMemory[firstMinIdx]->frozen ? "(frozen)" : "");
            partitionReservoir[firstNullReservoirSpaceIdx] = fastMemory[firstMinIdx];
            fastMemory[firstMinIdx] = readNextClient(file);

            if (fastMemory[firstMinIdx] != NULL)
            {
                printf("New client %d %s %s read from file\n", fastMemory[firstMinIdx]->client->codigo, fastMemory[firstMinIdx]->client->nome, fastMemory[firstMinIdx]->client->dataNascimento);
            }

            firstNullReservoirSpaceIdx = findFirstNullReservoirSpace(partitionReservoir, partitionSize);
        }

        printf("\nMemory state: \n");
        printMemoryState(fastMemory, partitionSize);
        printf("Reservoir state: \n");
        printMemoryState(partitionReservoir, partitionSize);

        if (firstNullReservoirSpaceIdx == -1)
        {

            printf("Reservoir is full... moving all elements in fast memory to partition and reservoir elements to fast memory\n");
            int idx = findFirstNonFrozenMin(fastMemory, partitionSize);

            while (idx != -1)
            {
                fwrite(fastMemory[idx]->client, sizeof(Client), 1, partitionFile);
                fastMemory[idx] = NULL;
                idx = findFirstNonFrozenMin(fastMemory, partitionSize);
            }

            fclose(partitionFile);
            partitionFile = createPartitionFile(outputDirectory, ++partitionCount);

            for (int i = 0; i < partitionSize; i++)
            {
                fastMemory[i] = partitionReservoir[i];
                partitionReservoir[i] = NULL;
            }

            if (partitionFile == NULL)
            {
                fclose(file);
                printf("Error creating partition file\n");
                exit(1);
            }
            continue;
        }
    }
}