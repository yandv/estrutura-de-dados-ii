#include<stdio.h>
#include "../include/bool.h"
#include "../include/client.h"
#include<stdlib.h>

int* create_hashtable(int table_size) {
    FILE *table = fopen("tables/hashtable", "wb");
    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    void *empty = (void *) -1;

    for (int i = 0; i < table_size; i++) {
        fwrite(&empty, sizeof(void*), 1, table);
    }

    fclose(table);


    return table;
}

void *read_entry(int index, FILE* table, int table_size) {
    table = fopen("tables/hashtable", "rb");

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

int hash_function(int cod, int table_size){
    int address = cod % table_size;
    return address;
}



int main(int argc, char const *argv[])
{
    int table_size = atoi(argv[1]);
    
    int* table = create_hashtable(table_size);

    for (int i = 0; i < table_size; i++)
    {
        printf("%d\n", read_entry(i, table, table_size));
    }
    
    return 0;
}


