#include<stdio.h>
#include "../include/bool.h"
#include "../include/client.h"
#include<stdlib.h>

void create_hashtable(int table_size) {
    FILE *table = fopen("tables/hashtable", "wb");
    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    void *empty = (void *)-1;

    for (int i = 0; i < table_size; i++) {
        fwrite(&empty, sizeof(void*), 1, table);
    }

    fclose(table);
}

void *read_entry(int index, int table_size) {
    FILE *table = fopen("tables/hashtable", "rb");

    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    if (index < 0 || index >= table_size) {
        fprintf(stderr, "Erro: Índice fora dos limites da tabela hash.\n");
        exit(1);
    }

    void *address;

    fseek(table, index * sizeof(void *), SEEK_SET);
    fread(&address, sizeof(void *), 1, table);

    fclose(table);
    return address;
}


int main(int argc, char const *argv[])
{
    int table_size = atoi(argv[1]);
    create_hashtable(table_size);
    printf("%p\n", read_entry(20, table_size));
    return 0;
}


