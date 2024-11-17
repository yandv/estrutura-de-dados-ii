#ifndef RUNTIME_TYPE_CHECK
#define RUNTIME_TYPE_CHECK

#include "../include/client.h"

#include <string.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Utility!!
 */

size_t getOffsetOfMember(const char *memberName)
{
    if (strcmp(memberName, "nome") == 0)
    {
        return offsetof(Client, nome);
    }

    if (strcmp(memberName, "idade") == 0)
    {
        return offsetof(Client, idade);
    }

    if (strcmp(memberName, "codigo") == 0)
    {
        return offsetof(Client, codigo);
    }

    return -1;
}

size_t getSizeofMember(const char *memberName)
{
    if (strcmp(memberName, "nome") == 0)
    {
        return CHAR_SIZE * CLIENT_NAME_SIZE;
    }

    if (strcmp(memberName, "idade") == 0 || strcmp(memberName, "codigo") == 0)
    {
        return INT_SIZE;
    }

    return -1;
}

int compareElementWithValue(KeyInformation *keyInformation, uint8_t *element, uint8_t *value)
{
    if (strcmp(keyInformation->keyName, "nome") == 0)
    {
        return strcmp((char *)(element + INT_SIZE + INT_SIZE), (char *)value);
    }
    else if (strcmp(keyInformation->keyName, "idade") == 0)
    {
        return *(int *)(element + INT_SIZE) - *(int *)value;
    }
    else if (strcmp(keyInformation->keyName, "codigo") == 0)
    {
        return *(int *)(element + INT_SIZE) - *(int *)value;
    }
    else
    {
        return 0;
    }
}

int compare(KeyInformation *keyInformation, size_t offset, uint8_t *element1, uint8_t *element2)
{
    if (strcmp(keyInformation->keyName, "nome") == 0)
    {
        return strcmp((char *)(element1 + offset), (char *)(element2 + offset));
    }
    else if (strcmp(keyInformation->keyName, "idade") == 0)
    {
        return *(int *)(element1 + offset) - *(int *)(element2 + offset);
    }
    else if (strcmp(keyInformation->keyName, "codigo") == 0)
    {
        return *(int *)(element1 + offset) - *(int *)(element2 + offset);
    }
    else
    {
        return 0;
    }
}

KeyInformation *createKeyInformation(const char *keyName)
{
    KeyInformation *keyInformation = (KeyInformation *)malloc(sizeof(KeyInformation));
    strcpy(keyInformation->keyName, keyName);
    keyInformation->offset = getOffsetOfMember(keyName);
    keyInformation->sizeofKey = getSizeofMember(keyName);
    return keyInformation;
}

#endif