#ifndef CLIENT
#define CLIENT

#include <stdio.h>
#include <stdlib.h>

#define CLIENT_NAME_SIZE 50

#define INT_SIZE sizeof(int)
#define CHAR_SIZE sizeof(char)

#define KEY_NAME_SIZE 50

typedef struct
{
    char keyName[KEY_NAME_SIZE];
    int offset;
    unsigned long sizeofKey;
} KeyInformation;

typedef struct
{
    int codigo;
    char nome[CLIENT_NAME_SIZE];
    int idade;
} Client;

static void *readElementFromFile(FILE *file, size_t size)
{
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

uint8_t *createBuffer(size_t size)
{
    return (uint8_t *)malloc(size * sizeof(uint8_t));
}

uint8_t **createBufferArray(KeyInformation *primaryKey, KeyInformation *keyInformation, int totalSize)
{
    size_t size = INT_SIZE + primaryKey->sizeofKey + keyInformation->sizeofKey;
    return (uint8_t **)malloc(size * totalSize * sizeof(uint8_t *));
}

uint8_t *readElementAsBuffer(FILE *file, size_t size)
{
    return readElementFromFile(file, size);
}

void writeElementFromBufferToFile(uint8_t *element, size_t totalSize, FILE *file)
{
    fwrite(element, totalSize, 1, file);
}

void bubbleSort(uint8_t **toSortArray, int toSortArraySize, KeyInformation *currentKey, size_t offset)
{
    int i, j;
    bool swapped;
    uint8_t *temp;

    for (int i = 0; i < toSortArraySize; i++)
    {
        swapped = false;

        for (int j = 0; j < toSortArraySize - i - 1; j++)
        {
            if (compare(currentKey, offset, toSortArray[j], toSortArray[j + 1]) > 0)
            {
                temp = toSortArray[j];
                toSortArray[j] = toSortArray[j + 1];
                toSortArray[j + 1] = temp;
                swapped = true;
            }
        }

        if (!swapped)
            break;
    }
}

#endif