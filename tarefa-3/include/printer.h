#ifndef PRINTER
#define PRINTER

#include <stdio.h>
#include <ctype.h>

char *firstLetterUpperCase(char *str)
{
    char *newStr = (char *)malloc(strlen(str) + 1);
    strcpy(newStr, str);
    newStr[0] = toupper(newStr[0]);
    return newStr;
}

char *convertKeyToString(uint8_t *element, int offset, KeyInformation *keyInformation)
{
    if (strcmp(keyInformation->keyName, "nome") == 0)
    {
        char *str = (char *)malloc(keyInformation->sizeofKey + 1);
        memcpy(str, element + offset, keyInformation->sizeofKey);
        str[keyInformation->sizeofKey] = '\0';
        return str;
    }
    else if (strcmp(keyInformation->keyName, "idade") == 0 || strcmp(keyInformation->keyName, "codigo") == 0)
    {
        char *str = (char *)malloc(keyInformation->sizeofKey + 1);
        sprintf(str, "%d", *(int *)(element + offset));
        return str;
    } else {
        printf("eita\n");
    }
}

void printElement(uint8_t *element, KeyInformation *keyInformation)
{
    if (strcmp(keyInformation->keyName, "nome") == 0)
    {
        printf("%s", (char *)(element));
    }
    else if (strcmp(keyInformation->keyName, "idade") == 0)
    {
        printf("%d", *(int *)(element));
    }
}

void printElementCreated(uint8_t *element, KeyInformation *keyInformation)
{
    printf("%-6d | %-15d | ", *(int *)element, *(int *)(element + INT_SIZE));
    printElement(element + INT_SIZE + INT_SIZE, keyInformation);
    printf("\n");
}

#endif