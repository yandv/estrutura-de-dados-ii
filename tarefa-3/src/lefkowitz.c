#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "../include/bool.h"
#include "../include/runtime-type-check.h"
#include "../include/printer.h"

FILE *startFirstStepLefkowitz(FILE *primaryKeyOrderedFile, size_t totalStructSize, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startSecondStepLefkowitz(FILE *lefkowitzFile, size_t totalStructSize, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startThirdStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startFourthStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startFifthStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE *startSixthStepLefkowitz(FILE **lefkowitzFiles, FILE *primaryKeyOrderedFile, size_t totalStructSize, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);

int main(int argc, char const *argv[])
{
    const char *primaryKeyOrderedFileName = argc > 1 ? argv[1] : "resources/clients.txt";
    const int totalStructSize = argc > 2 ? atoi(argv[2]) : 60;

    KeyInformation *primaryKey = argc > 3 ? createKeyInformation(argv[3]) : createKeyInformation("codigo");
    int secondaryKeysSize = argc > 4 ? argc - 4 : 2;
    KeyInformation **secondaryKeysInformations = (KeyInformation **)malloc(secondaryKeysSize * sizeof(KeyInformation *));

    if (argc > 4)
    {
        for (int i = 4; i < argc; i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (strcmp(argv[i], argv[j]) == 0)
                {
                    printf("Error: key \"%s\" already exists\n", argv[i]);
                    return 1;
                }
            }
            secondaryKeysInformations[i - 4] = createKeyInformation(argv[i]);
        }
    }
    else
    {
        secondaryKeysInformations[0] = createKeyInformation("nome");
        secondaryKeysInformations[1] = createKeyInformation("idade");
    }

    printf("\nLefkowitz program configuration:\n");
    printf(" > Total struct size: %d\n", totalStructSize);
    printf(" > Primary key: \"%s\"\n", primaryKey->keyName);
    printf(" > Primary key ordered file: \"%s\"\n", primaryKeyOrderedFileName);

    for (int i = 0; i < secondaryKeysSize; i++)
    {
        printf(" > Secondary key %d: \"%s\"\n", i + 1, secondaryKeysInformations[i]->keyName);
    }

    printf("\n");

    FILE *primaryKeyOrderedFile = fopen(primaryKeyOrderedFileName, "rb");

    if (primaryKeyOrderedFile == NULL)
    {
        printf("Error opening file \"%s\"\n", primaryKeyOrderedFileName);
        return 1;
    }

    FILE *lefkowitzFileFirstStep = startFirstStepLefkowitz(primaryKeyOrderedFile, totalStructSize, primaryKey, secondaryKeysInformations, secondaryKeysSize);

    if (lefkowitzFileFirstStep == NULL)
    {
        fclose(primaryKeyOrderedFile);
        return 1;
    }

    FILE **lefkowitzFilesSecondStep = startSecondStepLefkowitz(lefkowitzFileFirstStep, totalStructSize, primaryKey, secondaryKeysInformations, secondaryKeysSize);

    if (lefkowitzFilesSecondStep == NULL)
    {
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        return 1;
    }

    FILE **lefkowitzFilesThirdStep = startThirdStepLefkowitz(lefkowitzFilesSecondStep, primaryKey, secondaryKeysInformations, secondaryKeysSize);

    if (lefkowitzFilesThirdStep == NULL)
    {
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        return 1;
    }

    FILE **lefkowitzFilesFourthStep = startFourthStepLefkowitz(lefkowitzFilesThirdStep, primaryKey, secondaryKeysInformations, secondaryKeysSize);

    if (lefkowitzFilesFourthStep == NULL)
    {
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        return 1;
    }

    FILE **lefkowitzFilesFifthStep = startFifthStepLefkowitz(lefkowitzFilesFourthStep, primaryKey, secondaryKeysInformations, secondaryKeysSize);

    if (lefkowitzFilesFifthStep == NULL)
    {
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        return 1;
    }

    FILE *lefkowitzFilesSixthStep = startSixthStepLefkowitz(lefkowitzFilesFifthStep, primaryKeyOrderedFile, totalStructSize, primaryKey, secondaryKeysInformations, secondaryKeysSize);

    if (lefkowitzFilesSixthStep == NULL)
    {
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        return 1;
    }

    free(primaryKey);
    free(secondaryKeysInformations);
    return 0;
}

FILE *startFirstStepLefkowitz(FILE *primaryKeyOrderedFile, size_t totalStructSize, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting first step of Lefkowitz\n");
    printf("Creating A2 file\n\n");

    FILE *lefkowitzFile = fopen("resources/lefkowitz-a2.txt", "wb+");

    if (lefkowitzFile == NULL)
    {
        printf("Error opening file \"resources/lefkowitz-a2.txt\"\n");
        return NULL;
    }

    uint8_t *originalElement;
    uint8_t *elementBuffer = createBuffer(INT_SIZE + totalStructSize);
    int currentOffset = 1;

    printf("-------------------\n");
    printf("%-6s | %-16s ", "ED", firstLetterUpperCase(primaryKey->keyName));
    printf("\n-------------------\n");

    while ((originalElement = readElementAsBuffer(primaryKeyOrderedFile, totalStructSize)) != NULL)
    {
        memcpy(elementBuffer, &(currentOffset), INT_SIZE);
        memcpy(elementBuffer + INT_SIZE, originalElement, totalStructSize);
        printf("%-6d | %-15d\n", currentOffset, *(int *)(elementBuffer + INT_SIZE));
        writeElementFromBufferToFile(elementBuffer, INT_SIZE + totalStructSize, lefkowitzFile);
        currentOffset++;
    }

    printf("-------------------\n");

    return lefkowitzFile;
}

FILE **startSecondStepLefkowitz(FILE *lefkowitzFile, size_t totalStructSize, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting second step of Lefkowitz\n\n");
    printf("Creating A3 file(s) (%d file count)\n\n", secondaryKeysInformationsSize);

    FILE **lefkowitzFilesSecondStep = (FILE **)malloc(secondaryKeysInformationsSize * sizeof(FILE *));

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        rewind(lefkowitzFile);
        KeyInformation *secondaryKey = secondaryKeysInformations[i];

        // endereço, chave primaria, chave secundária
        size_t size = INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey;
        uint8_t writerBuffer[size];

        char lefkowitzFileName[100];
        sprintf(lefkowitzFileName, "resources/lefkowitz-a3-%s.txt", secondaryKey->keyName);

        lefkowitzFilesSecondStep[i] = fopen(lefkowitzFileName, "wb+");

        if (lefkowitzFilesSecondStep[i] == NULL)
        {
            printf("Error opening file \"%s\"\n", lefkowitzFileName);
            return NULL;
        }

        printf("Creating A3 file \"%s\" (%s)\n\n", lefkowitzFileName, secondaryKey->keyName);

        uint8_t *lefkowitzFileElement;

        printf("---------------------------------------\n");
        printf("%-6s | %-16s | %-20s \n", "ED", firstLetterUpperCase(primaryKey->keyName), firstLetterUpperCase(secondaryKey->keyName));
        printf("---------------------------------------\n");

        while ((lefkowitzFileElement = readElementAsBuffer(lefkowitzFile, INT_SIZE + totalStructSize)) != NULL)
        {
            memcpy(writerBuffer, lefkowitzFileElement, INT_SIZE);
            memcpy(writerBuffer + INT_SIZE, lefkowitzFileElement + INT_SIZE + primaryKey->offset * CHAR_SIZE, primaryKey->sizeofKey);
            memcpy(writerBuffer + INT_SIZE + primaryKey->sizeofKey, lefkowitzFileElement + INT_SIZE + secondaryKey->offset * CHAR_SIZE, secondaryKey->sizeofKey);

            printElementCreated(writerBuffer, secondaryKey);

            fwrite(writerBuffer, size, 1, lefkowitzFilesSecondStep[i]);
        }

        printf("---------------------------------------\n");

        printf("\n");
        printf("Finished creating A2 file \"%s\"\n\n", lefkowitzFileName);
    }

    return lefkowitzFilesSecondStep;
}

FILE **startThirdStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting third step of Lefkowitz\n\n");
    printf("Creating A4 file\n\n");

    FILE **lefkowitzFileThirdStep = (FILE **)malloc(secondaryKeysInformationsSize * sizeof(FILE *));

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        rewind(lefkowitzFiles[i]);
        KeyInformation *secondaryKey = secondaryKeysInformations[i];

        char lefkowitzFileName[100];
        sprintf(lefkowitzFileName, "resources/lefkowitz-a4-%s.txt", secondaryKey->keyName);

        lefkowitzFileThirdStep[i] = fopen(lefkowitzFileName, "wb+");

        if (lefkowitzFileThirdStep[i] == NULL)
        {
            printf("Error opening file \"%s\"\n", lefkowitzFileName);
            return NULL;
        }

        printf("Creating A4 file \"%s\" (%s)\n\n", lefkowitzFileName, secondaryKey->keyName);

        uint8_t **elementBuffer = createBufferArray(primaryKey, secondaryKey, 100);
        uint8_t *element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey);
        int index = 0;

        while (element != NULL)
        {
            if (index > 0 && index % 100 == 0)
            {
                elementBuffer = (uint8_t **)realloc(elementBuffer, 100 * sizeof(uint8_t *));
            }
            elementBuffer[index++] = element;
            element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey);
        }

        bubbleSort(elementBuffer, index, secondaryKey, INT_SIZE + primaryKey->sizeofKey);

        printf("---------------------------------------\n");
        printf("%-6s | %-15s | %-15s \n", "ED", firstLetterUpperCase(primaryKey->keyName), firstLetterUpperCase(secondaryKey->keyName));
        printf("---------------------------------------\n");

        for (int j = 0; j < index; j++)
        {
            printf("%-6d | %-15s | %-15s \n", *(int *)elementBuffer[j], convertKeyToString(elementBuffer[j], INT_SIZE, primaryKey), convertKeyToString(elementBuffer[j], INT_SIZE + primaryKey->sizeofKey, secondaryKey));
            writeElementFromBufferToFile(elementBuffer[j], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey, lefkowitzFileThirdStep[i]);
        }

        printf("---------------------------------------\n");
    }

    return lefkowitzFileThirdStep;
}

FILE **startFourthStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting fourth step of Lefkowitz\n\n");
    printf("Creating A5 file\n\n");

    typedef struct
    {
        uint8_t *primaryKey;
        int address;
        int nextAddress;
    } Address;

    typedef struct
    {
        uint8_t *secondaryKey;   // CSi
        int firstElementAddress; // PT
        int elementCount;        // Q
        Address **address;       // P1, P2, ..., Pq
    } SecondaryKeyInfo;

    FILE **lefkowitzFileFourthStep = (FILE **)malloc(secondaryKeysInformationsSize * 2 * sizeof(FILE *));

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        KeyInformation *secondaryKey = secondaryKeysInformations[i];
        FILE *thirdStepFile = lefkowitzFiles[i];

        rewind(thirdStepFile);

        char lefkowitzFileName[100];
        sprintf(lefkowitzFileName, "resources/lefkowitz-a5-%s.txt", secondaryKey->keyName);

        lefkowitzFileFourthStep[i] = fopen(lefkowitzFileName, "wb+");

        if (lefkowitzFileFourthStep[i] == NULL)
        {
            printf("Error opening file \"%s\"\n", lefkowitzFileName);
            return NULL;
        }

        printf("Creating A5 file \"%s\" (%s)\n", lefkowitzFileName, secondaryKey->keyName);

        sprintf(lefkowitzFileName, "resources/lefkowitz-a6-%s.txt", secondaryKey->keyName);

        lefkowitzFileFourthStep[i + secondaryKeysInformationsSize] = fopen(lefkowitzFileName, "wb+");

        if (lefkowitzFileFourthStep[i + secondaryKeysInformationsSize] == NULL)
        {
            printf("Error opening file \"%s\"\n", lefkowitzFileName);
            return NULL;
        }

        printf("Creating A6 file \"%s\" (%s)\n\n", lefkowitzFileName, secondaryKey->keyName);

        // endereço, chave primaria, chave secundária

        int secondaryKeyInfoSize = 5;
        SecondaryKeyInfo **secondaryKeyInfos = (SecondaryKeyInfo **)malloc(secondaryKeyInfoSize * sizeof(SecondaryKeyInfo *));
        memset(secondaryKeyInfos, 0, secondaryKeyInfoSize * sizeof(SecondaryKeyInfo *));

        uint8_t **elementBuffer = createBufferArray(primaryKey, secondaryKey, 100);

        uint8_t *element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey);
        int index = 0;

        while (element != NULL)
        {
            if (index > 0 && index % 100 == 0)
            {
                elementBuffer = (uint8_t **)realloc(elementBuffer, 100 * sizeof(uint8_t *));
            }
            elementBuffer[index++] = element;

            int j = 0, k = 0;
            SecondaryKeyInfo *fifthStepElement = NULL;

            for (; j < secondaryKeyInfoSize; j++)
            {
                if (secondaryKeyInfos[j] == NULL)
                    break;

                if (compareElementWithValue(secondaryKey, element, secondaryKeyInfos[j]->secondaryKey) == 0)
                {
                    fifthStepElement = secondaryKeyInfos[j];
                    j = 0;
                    break;
                }
            }

            if (fifthStepElement == NULL)
            {
                if (j == secondaryKeyInfoSize)
                {
                    secondaryKeyInfoSize *= 2;
                    secondaryKeyInfos = (SecondaryKeyInfo **)realloc(secondaryKeyInfos, secondaryKeyInfoSize * sizeof(SecondaryKeyInfo *));
                    memset(secondaryKeyInfos + secondaryKeyInfoSize / 2, 0, secondaryKeyInfoSize / 2 * sizeof(SecondaryKeyInfo *));

                    if (secondaryKeyInfos == NULL)
                    {
                        printf("Error reallocating fifthStepElements\n");
                        return NULL;
                    }

                    secondaryKeyInfos[j] = (SecondaryKeyInfo *)malloc(sizeof(SecondaryKeyInfo));
                }
                else
                {
                    secondaryKeyInfos[j] = (SecondaryKeyInfo *)malloc(sizeof(SecondaryKeyInfo));
                }

                fifthStepElement = secondaryKeyInfos[j];
                fifthStepElement->secondaryKey = (uint8_t *)malloc(secondaryKey->sizeofKey);
                memcpy(fifthStepElement->secondaryKey, element + INT_SIZE + primaryKey->sizeofKey, secondaryKey->sizeofKey);
                fifthStepElement->firstElementAddress = *(int *)element;
                fifthStepElement->address = (Address **)malloc(5 * sizeof(Address *));
            }

            if (fifthStepElement->elementCount > 0 && (fifthStepElement->elementCount - 1) % 5 == 0)
            {
                fifthStepElement->address = (Address **)realloc(fifthStepElement->address, sizeof(Address *) * (fifthStepElement->elementCount + 5));
            }

            fifthStepElement->address[fifthStepElement->elementCount] = (Address *)malloc(sizeof(Address));
            fifthStepElement->address[fifthStepElement->elementCount]->address = *(int *)element;
            fifthStepElement->address[fifthStepElement->elementCount]->nextAddress = -1;
            fifthStepElement->address[fifthStepElement->elementCount]->primaryKey = (uint8_t *)(element + INT_SIZE);

            if (fifthStepElement->elementCount > 0)
            {
                fifthStepElement->address[fifthStepElement->elementCount - 1]->nextAddress = *(int *)element;
            }

            fifthStepElement->elementCount++;

            element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey);
        }

        for (int j = 0; j < secondaryKeyInfoSize; j++)
        {
            if (secondaryKeyInfos[j] == NULL)
                break;

            size_t writerBufferSize = INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey;
            uint8_t *writerBuffer = createBuffer(writerBufferSize);

            memcpy(writerBuffer, &secondaryKeyInfos[j]->firstElementAddress, INT_SIZE);
            memcpy(writerBuffer + INT_SIZE, &secondaryKeyInfos[j]->elementCount, primaryKey->sizeofKey);
            memcpy(writerBuffer + INT_SIZE + primaryKey->sizeofKey, secondaryKeyInfos[j]->secondaryKey, secondaryKey->sizeofKey);

            fwrite(writerBuffer, writerBufferSize, 1, lefkowitzFileFourthStep[i]);

            for (int k = 0; k < secondaryKeyInfos[j]->elementCount; k++)
            {
                Address *address = secondaryKeyInfos[j]->address[k];
                writerBufferSize = INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey + INT_SIZE;
                writerBuffer = createBuffer(writerBufferSize);

                memcpy(writerBuffer, &address->address, INT_SIZE);
                memcpy(writerBuffer + INT_SIZE, address->primaryKey, primaryKey->sizeofKey);
                memcpy(writerBuffer + INT_SIZE + primaryKey->sizeofKey, secondaryKeyInfos[j]->secondaryKey, secondaryKey->sizeofKey);
                memcpy(writerBuffer + INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey, &address->nextAddress, INT_SIZE);

                fwrite(writerBuffer, writerBufferSize, 1, lefkowitzFileFourthStep[i + secondaryKeysInformationsSize]);
            }
        }

        printf("----TABELA A5----\n");
        printf("%-2s | %-2s | %-2s \n", "PT", "Q", firstLetterUpperCase(secondaryKey->keyName));
        printf("-----------------\n");

        for (int j = 0; j < secondaryKeyInfoSize; j++)
        {
            if (secondaryKeyInfos[j] == NULL)
                break;

            const char str[10] = " ";

            printf("%-2d | %-2d | %-2s", secondaryKeyInfos[j]->firstElementAddress, secondaryKeyInfos[j]->elementCount, convertKeyToString(secondaryKeyInfos[j]->secondaryKey, 0, secondaryKey));
            printf("\n");
        }

        printf("-----------------\n");

        printf("---------------------------TABELA A6---------------------------\n");
        printf("%-4s | %-15s | %-18s | %s \n", "ED", firstLetterUpperCase(primaryKey->keyName), firstLetterUpperCase(secondaryKey->keyName), "Próximo elemento");
        printf("---------------------------------------------------------------\n");

        for (int j = 0; j < secondaryKeyInfoSize; j++)
        {
            if (secondaryKeyInfos[j] == NULL)
                break;

            for (int k = 0; k < secondaryKeyInfos[j]->elementCount; k++)
            {
                printf("%-4d | %-15s | %-18s | %d\n", secondaryKeyInfos[j]->address[k]->address, convertKeyToString(secondaryKeyInfos[j]->address[k]->primaryKey, 0, primaryKey), convertKeyToString(secondaryKeyInfos[j]->secondaryKey, 0, secondaryKey), secondaryKeyInfos[j]->address[k]->nextAddress);
            }
        }

        printf("-------------------------------------------------------------\n");
    }

    return lefkowitzFileFourthStep;
}

FILE **startFifthStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting fifth step of Lefkowitz\n\n");
    printf("Creating A7 file\n\n");

    FILE **lefkowitzFileFifthStep = (FILE **)malloc(secondaryKeysInformationsSize * 2 * sizeof(FILE *));

    // ordene todos os arquivos lefkowitzFiles pela chave primária

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        rewind(lefkowitzFiles[i + secondaryKeysInformationsSize]);

        KeyInformation *secondaryKey = secondaryKeysInformations[i];

        char lefkowitzFileName[100];
        sprintf(lefkowitzFileName, "resources/lefkowitz-a7-%s.txt", secondaryKey->keyName);

        lefkowitzFileFifthStep[i] = lefkowitzFiles[i];
        lefkowitzFileFifthStep[i + secondaryKeysInformationsSize] = fopen(lefkowitzFileName, "wb+");

        if (lefkowitzFileFifthStep[i + secondaryKeysInformationsSize] == NULL)
        {
            printf("Error opening file \"%s\"\n", lefkowitzFileName);
            return NULL;
        }

        printf("Creating A7 file \"%s\" (%s)\n\n", lefkowitzFileName, secondaryKey->keyName);

        uint8_t **elementBuffer = createBufferArray(primaryKey, secondaryKey, 100);
        uint8_t *element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i + secondaryKeysInformationsSize], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey + INT_SIZE);

        if (element == NULL)
        {
            printf("Element is null\n");
            return NULL;
        }

        int index = 0;

        while (element != NULL)
        {
            if (index > 0 && index % 100 == 0)
            {
                elementBuffer = (uint8_t **)realloc(elementBuffer, 100 * sizeof(uint8_t *));
            }
            elementBuffer[index++] = element;
            element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i + secondaryKeysInformationsSize], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey + INT_SIZE);
        }

        bubbleSort(elementBuffer, index, primaryKey, INT_SIZE);

        printf("--------------------------------------------------------------\n");
        printf("%-6s | %-15s | %-15s | %s \n", "ED", firstLetterUpperCase(primaryKey->keyName), firstLetterUpperCase(secondaryKey->keyName), "Próximo elemento");
        printf("--------------------------------------------------------------\n");

        for (int j = 0; j < index; j++)
        {
            printf("%-6d | %-15s | %-15s | %d \n", *(int *)elementBuffer[j], convertKeyToString(elementBuffer[j], INT_SIZE, primaryKey), convertKeyToString(elementBuffer[j], INT_SIZE + primaryKey->sizeofKey, secondaryKey), *(int *)(elementBuffer[j] + INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey));
            writeElementFromBufferToFile(elementBuffer[j], INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey + INT_SIZE, lefkowitzFileFifthStep[i + secondaryKeysInformationsSize]);
        }

        printf("--------------------------------------------------------------\n");
    }

    return lefkowitzFileFifthStep;
}

FILE *startSixthStepLefkowitz(FILE **lefkowitzFiles, FILE *primaryKeyOrderedFile, size_t totalStructSize, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting sixth step of Lefkowitz\n");

    FILE *lefkowitzFileSixthStep = (FILE *)malloc(secondaryKeysInformationsSize * 2 * sizeof(FILE *));

    rewind(primaryKeyOrderedFile);

    char lefkowitzFileName[100] = "resources/lefkowitz-a8.txt";

    lefkowitzFileSixthStep = fopen(lefkowitzFileName, "wb+");

    if (lefkowitzFileSixthStep == NULL)
    {
        printf("Error opening file \"%s\"\n", lefkowitzFileName);
        return NULL;
    }

    printf("Creating A8 file \"%s\"\n\n", lefkowitzFileName);

    // pegar todos os elementos do arquivo primaryKeyOrderedFile (A1) e juntar com os arquivos A7 gerados

    size_t totalAdditionalColumnsSize = 0;

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        rewind(lefkowitzFiles[i + secondaryKeysInformationsSize]);
        totalAdditionalColumnsSize += INT_SIZE;
    }

    uint8_t *writterBuffer = createBuffer(totalStructSize + totalAdditionalColumnsSize);

    uint8_t *originalElement;
    uint8_t *element = NULL;

    printf("----------------------------------------------------------------------------------------------\n");
    printf("%-8s | ", firstLetterUpperCase(primaryKey->keyName));

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        printf("%-16s | %-25s ", firstLetterUpperCase(secondaryKeysInformations[i]->keyName), concat("Próximo(a) ", firstLetterUpperCase(secondaryKeysInformations[i]->keyName)));

        if (i != secondaryKeysInformationsSize - 1)
        {
            printf("| ");
        }
    }

    printf("\n----------------------------------------------------------------------------------------------\n");

    while ((originalElement = (uint8_t *)readElementAsBuffer(primaryKeyOrderedFile, totalStructSize)) != NULL)
    {
        memcpy(writterBuffer, originalElement, totalStructSize);

        printf("%-8d | ", *(int *)originalElement);

        for (int i = 0; i < secondaryKeysInformationsSize; i++)
        {
            element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i + secondaryKeysInformationsSize], INT_SIZE + primaryKey->sizeofKey + secondaryKeysInformations[i]->sizeofKey + INT_SIZE);
            memcpy(writterBuffer + totalStructSize + (i * INT_SIZE), element + INT_SIZE + primaryKey->sizeofKey + secondaryKeysInformations[i]->sizeofKey, INT_SIZE);
            printf("%-16s | %-25d ", convertKeyToString(element, INT_SIZE + primaryKey->sizeofKey, secondaryKeysInformations[i]), *(int *)(element + INT_SIZE + primaryKey->sizeofKey + secondaryKeysInformations[i]->sizeofKey));

            if (i != secondaryKeysInformationsSize - 1)
            {
                printf("| ");
            }
        }

        printf("\n");

        fwrite(writterBuffer, totalStructSize + totalAdditionalColumnsSize, 1, lefkowitzFileSixthStep);
    }

    printf("----------------------------------------------------------------------------------------------\n");

    return lefkowitzFileSixthStep;
}