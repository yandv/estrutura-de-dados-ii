#ifndef BOOL
#define BOOL

#include <stdlib.h>
#include <string.h>

#define bool int
#define false 0
#define true 1

char* concat(const char *s1, const char *s2) {
    char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

#endif