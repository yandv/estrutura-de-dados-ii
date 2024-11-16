#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "../include/bool.h"
#include "../include/runtime-type-check.h"
#include "../include/printer.h"

typedef struct
{

    int offset;
    Client client;

} LefkowitzFileElement;

FILE *startFirstStepLefkowitz(FILE *primaryKeyOrderedFile, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startSecondStepLefkowitz(FILE *lefkowitzFile, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startThirdStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);
FILE **startFourthStepLefkowitz(FILE **lefkowitzFiles, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize);

void bubbleSort(uint8_t **clientsArray, int clientsArraySize, KeyInformation *keyInformation)
{
    int i, j;
    bool swapped;
    uint8_t *temp;

    for (int i = 0; i < clientsArraySize; i++)
    {
        swapped = false;

        for (int j = 0; j < clientsArraySize - i - 1; j++)
        {
            if (compare(keyInformation, clientsArray[j], clientsArray[j + 1]) > 0)
            {
                temp = clientsArray[j];
                clientsArray[j] = clientsArray[j + 1];
                clientsArray[j + 1] = temp;
                swapped = true;
            }
        }

        if (!swapped)
            break;
    }
}

uint8_t **createBuffer(KeyInformation *primaryKey, KeyInformation *keyInformation)
{
    size_t size = INT_SIZE + primaryKey->sizeofKey + keyInformation->sizeofKey;
    return (uint8_t **)malloc(size * sizeof(uint8_t *));
}

uint8_t *readElementAsBuffer(FILE *file, KeyInformation *primaryKey, KeyInformation *secondaryKey)
{
    return readElementFromFile(file, INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey);
}

void writeElementFromBufferToFile(uint8_t *element, KeyInformation *primaryKey, KeyInformation *secondaryKey, FILE *file)
{
    size_t size = INT_SIZE + primaryKey->sizeofKey + secondaryKey->sizeofKey;
    fwrite(element, size, 1, file);
}

int main(int argc, char const *argv[])
{
    const char *primaryKeyOrderedFileName = argc > 1 ? argv[1] : "resources/clients.txt";

    printf("\nLefkowitz program configuration:\n");
    printf(" > Primary key ordered file: \"%s\"\n\n", primaryKeyOrderedFileName);

    FILE *primaryKeyOrderedFile = fopen(primaryKeyOrderedFileName, "rb");

    if (primaryKeyOrderedFile == NULL)
    {
        printf("Error opening file \"%s\"\n", primaryKeyOrderedFileName);
        return 1;
    }

    int secondaryKeysInformationsSize = 2;
    char **keyInformationsName = (char **)malloc(secondaryKeysInformationsSize * sizeof(char *));

    keyInformationsName[0] = "nome";
    keyInformationsName[1] = "idade";

    KeyInformation **secondaryKeysInformations = (KeyInformation **)malloc(secondaryKeysInformationsSize * sizeof(KeyInformation *));

    for (int i = 0; i < secondaryKeysInformationsSize; i++)
    {
        secondaryKeysInformations[i] = createKeyInformation(keyInformationsName[i]);
    }

    KeyInformation *primaryKey = createKeyInformation("codigo");

    free(keyInformationsName);

    FILE *lefkowitzFileFirstStep = startFirstStepLefkowitz(primaryKeyOrderedFile, secondaryKeysInformations, secondaryKeysInformationsSize);

    if (lefkowitzFileFirstStep == NULL)
    {
        free(secondaryKeysInformations);
        fclose(primaryKeyOrderedFile);
        return 1;
    }

    FILE **lefkowitzFilesSecondStep = startSecondStepLefkowitz(lefkowitzFileFirstStep, primaryKey, secondaryKeysInformations, secondaryKeysInformationsSize);

    if (lefkowitzFilesSecondStep == NULL)
    {
        free(secondaryKeysInformations);
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        return 1;
    }

    FILE **lefkowitzFilesThirdStep = startThirdStepLefkowitz(lefkowitzFilesSecondStep, primaryKey, secondaryKeysInformations, secondaryKeysInformationsSize);

    if (lefkowitzFilesThirdStep == NULL)
    {
        free(secondaryKeysInformations);
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        free(lefkowitzFilesSecondStep);
        return 1;
    }

    FILE **lefkowitzFilesFourthStep = startFourthStepLefkowitz(lefkowitzFilesThirdStep, primaryKey, secondaryKeysInformations, secondaryKeysInformationsSize);

    if (lefkowitzFilesFourthStep == NULL)
    {
        free(secondaryKeysInformations);
        fclose(primaryKeyOrderedFile);
        fclose(lefkowitzFileFirstStep);
        free(lefkowitzFilesSecondStep);
        free(lefkowitzFilesThirdStep);
        return 1;
    }

    return 0;
}

FILE *startFirstStepLefkowitz(FILE *primaryKeyOrderedFile, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
{
    printf("Starting first step of Lefkowitz\n");
    printf("Creating A2 file\n\n");

    FILE *lefkowitzFile = fopen("resources/lefkowitz-a2.txt", "wb+");

    if (lefkowitzFile == NULL)
    {
        printf("Error opening file \"resources/lefkowitz-a2.txt\"\n");
        return NULL;
    }

    Client *client = readClient(primaryKeyOrderedFile);
    LefkowitzFileElement *lefkowitzFileElement = (LefkowitzFileElement *)malloc(sizeof(LefkowitzFileElement));
    int currentOffset = 1;

    while (client != NULL)
    {
        lefkowitzFileElement->offset = currentOffset++;
        lefkowitzFileElement->client = *client;
        printf("Writing element %d %d\n", lefkowitzFileElement->offset, lefkowitzFileElement->client.codigo);
        fwrite(lefkowitzFileElement, sizeof(LefkowitzFileElement), 1, lefkowitzFile);
        client = readClient(primaryKeyOrderedFile);
    }

    printf("\n");

    free(lefkowitzFileElement);
    return lefkowitzFile;
}

FILE **startSecondStepLefkowitz(FILE *lefkowitzFile, KeyInformation *primaryKey, KeyInformation **secondaryKeysInformations, int secondaryKeysInformationsSize)
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

        LefkowitzFileElement *lefkowitzFileElement = (LefkowitzFileElement *)readElementFromFile(lefkowitzFile, sizeof(LefkowitzFileElement));

        printf("---------------------------------------\n");
        printf("%-6s | %-16s | %-20s \n", "ED", firstLetterUpperCase(primaryKey->keyName), firstLetterUpperCase(secondaryKey->keyName));
        printf("---------------------------------------\n");

        while (lefkowitzFileElement != NULL)
        {
            // transformar o lefkowitzFileElement para ponteiro do tipo char (porque char pula de 1 em 1) e assim conseguimos acessar qualquer parte do struct
            memcpy(writerBuffer, &lefkowitzFileElement->offset, INT_SIZE);
            memcpy(writerBuffer + INT_SIZE, &lefkowitzFileElement->client.codigo, INT_SIZE);
            memcpy(writerBuffer + INT_SIZE + INT_SIZE, (char *)(&lefkowitzFileElement->client) + secondaryKey->offset * CHAR_SIZE, secondaryKey->sizeofKey);

            printElementCreated(writerBuffer, secondaryKey);

            fwrite(writerBuffer, size, 1, lefkowitzFilesSecondStep[i]);
            lefkowitzFileElement = (LefkowitzFileElement *)readElementFromFile(lefkowitzFile, sizeof(LefkowitzFileElement));
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

        uint8_t **elementBuffer = createBuffer(primaryKey, secondaryKey);
        uint8_t *element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], primaryKey, secondaryKey);
        int index = 0;

        while (element != NULL)
        {
            elementBuffer[index++] = element;
            element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], primaryKey, secondaryKey);
        }

        bubbleSort(elementBuffer, index, secondaryKey);

        printf("---------------------------------------\n");
        printf("%-6s | %-15s | %-15s \n", "ED", firstLetterUpperCase(primaryKey->keyName), firstLetterUpperCase(secondaryKey->keyName));
        printf("---------------------------------------\n");

        for (int j = 0; j < index; j++)
        {
            printf("%-6d | %-15s | %-15s \n", *(int *)elementBuffer[j], convertKeyToString(elementBuffer[j], INT_SIZE, primaryKey), convertKeyToString(elementBuffer[j], INT_SIZE + primaryKey->sizeofKey, secondaryKey));
            writeElementFromBufferToFile(elementBuffer[j], primaryKey, secondaryKey, lefkowitzFileThirdStep[i]);
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
        uint8_t *keyInformation; // CSi
        int firstElementAddress; // PT
        int elementCount;        // Q
        Address **address;       // P1, P2, ..., Pq
    } FifthStepElement;

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

        int fifthStepElementsSize = 5;
        FifthStepElement **fifthStepElements = (FifthStepElement **)malloc(fifthStepElementsSize * sizeof(FifthStepElement *));
        memset(fifthStepElements, 0, fifthStepElementsSize * sizeof(FifthStepElement *));

        uint8_t **elementBuffer = createBuffer(primaryKey, secondaryKey);

        uint8_t *element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], primaryKey, secondaryKey);
        int index = 0;

        while (element != NULL)
        {
            elementBuffer[index++] = element;

            int j = 0, k = 0;
            FifthStepElement *fifthStepElement = NULL;

            for (; j < fifthStepElementsSize; j++)
            {
                if (fifthStepElements[j] == NULL)
                    break;

                if (compareElementWithValue(secondaryKey, element, fifthStepElements[j]->keyInformation) == 0)
                {
                    fifthStepElement = fifthStepElements[j];
                    j = 0;
                    break;
                }
            }

            if (fifthStepElement == NULL)
            {
                if (j == fifthStepElementsSize)
                {
                    fifthStepElementsSize *= 2;
                    fifthStepElements = (FifthStepElement **)realloc(fifthStepElements, fifthStepElementsSize * sizeof(FifthStepElement *));
                    memset(fifthStepElements + fifthStepElementsSize / 2, 0, fifthStepElementsSize / 2 * sizeof(FifthStepElement *));

                    if (fifthStepElements == NULL)
                    {
                        printf("Error reallocating fifthStepElements\n");
                        return NULL;
                    }

                    fifthStepElements[j] = (FifthStepElement *)malloc(sizeof(FifthStepElement));
                }
                else
                {
                    fifthStepElements[j] = (FifthStepElement *)malloc(sizeof(FifthStepElement));
                }

                fifthStepElement = fifthStepElements[j];
                fifthStepElement->keyInformation = (uint8_t *)malloc(secondaryKey->sizeofKey);
                memcpy(fifthStepElement->keyInformation, element + INT_SIZE + INT_SIZE, secondaryKey->sizeofKey);
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

            if (fifthStepElement->elementCount > 0)
            {
                fifthStepElement->address[fifthStepElement->elementCount - 1]->nextAddress = *(int *)element;
            }

            fifthStepElement->elementCount++;

            element = (uint8_t *)readElementAsBuffer(lefkowitzFiles[i], primaryKey, secondaryKey);
        }

        for (int j = 0; j < fifthStepElementsSize; j++)
        {
            if (fifthStepElements[j] == NULL)
                break;

            size_t writerBufferSize = INT_SIZE + INT_SIZE + secondaryKey->sizeofKey;
            uint8_t writerBuffer[INT_SIZE + INT_SIZE + secondaryKey->sizeofKey];

            memcpy(writerBuffer, &fifthStepElements[j]->firstElementAddress, INT_SIZE);
            memcpy(writerBuffer + INT_SIZE, &fifthStepElements[j]->elementCount, INT_SIZE);
            memcpy(writerBuffer + INT_SIZE + INT_SIZE, fifthStepElements[j]->keyInformation, secondaryKey->sizeofKey);

            fwrite(writerBuffer, INT_SIZE + INT_SIZE + secondaryKey->sizeofKey, 1, lefkowitzFileFourthStep[i]);
        }

        printf("----TABELA A5----\n");
        printf("%-2s | %-2s | %-2s \n", "PT", "Q", firstLetterUpperCase(secondaryKey->keyName));
        printf("-----------------\n");

        for (int j = 0; j < fifthStepElementsSize; j++)
        {
            if (fifthStepElements[j] == NULL)
                break;

            const char str[10] = " ";

            printf("%-2d | %-2d | ", fifthStepElements[j]->firstElementAddress, fifthStepElements[j]->elementCount);
            printElement(fifthStepElements[j]->keyInformation, secondaryKey);
            printf("\n");
        }

        printf("-----------------\n");

        printf("-------------------TABELA A5-------------------\n");
        printf("%-2s | %-2s | %-2s | %-10s \n", "ED", firstLetterUpperCase(primaryKey->keyName), "Próximo elemento", firstLetterUpperCase(secondaryKey->keyName));
        printf("-----------------------------------------------\n");

        for (int j = 0; j < fifthStepElementsSize; j++)
        {
            if (fifthStepElements[j] == NULL)
                break;

            for (int k = 0; k < fifthStepElements[j]->elementCount; k++)
            {
                printf("%-2d %-2d", fifthStepElements[j]->address[k]->address);
                printElement(fifthStepElements[j]->keyInformation, secondaryKey);
                printf(" next element address %d\n", fifthStepElements[j]->address[k]->nextAddress);
            }
        }

        printf("-----------------------------------------------\n");
    }

    return lefkowitzFileFourthStep;
}