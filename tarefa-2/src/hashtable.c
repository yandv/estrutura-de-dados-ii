#include <stdio.h>
#include <stdlib.h>
#include "../include/bool.h"
#include "../include/client.h"

void* create_hashtable(int table_size) {
    FILE *table = fopen("tables/index_table", "wb");
    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    void *empty = (void *) -1;
    for (int i = 0; i < table_size; i++) {
        fwrite(&empty, sizeof(void*), 1, table);
    }

    fclose(table);
}

int hash_function(int cod, int table_size) {
    return cod % table_size;
}

void add(Client cliente, char* dir_elements_table, char* dir_index_table, int table_size) {
    int index = hash_function(cliente.codigo, table_size);

    // Abrindo e escrevendo o cliente em elements_table
    FILE* elements_table = fopen(dir_elements_table, "r+b");
    if (elements_table == NULL) {
        perror("Erro ao abrir elements_table");
        exit(1);
    }
    int offset = index * sizeof(Client);
    fseek(elements_table, offset, SEEK_SET);
    fwrite(&cliente, sizeof(Client), 1, elements_table);
    fclose(elements_table);

    // Abrindo e escrevendo o código do cliente em index_table
    FILE* index_table = fopen(dir_index_table, "r+b");
    if (index_table == NULL) {
        perror("Erro ao abrir index_table");
        exit(1);
    }
    offset = index * sizeof(void*);
    fseek(index_table, offset, SEEK_SET);
    fwrite(&cliente.codigo, sizeof(cliente.codigo), 1, index_table);
    fclose(index_table);

    printf("Cliente %d adicionado\n", cliente.codigo);
}

void print_table(char* dir_elements_table, char* dir_index_table, int table_size) {
    FILE* elements_table = fopen(dir_elements_table, "rb");
    FILE* index_table = fopen(dir_index_table, "rb");

    if (elements_table == NULL || index_table == NULL) {
        perror("Erro ao abrir o arquivo de tabela");
        exit(1);
    }

    int i = 0;
    void *empty = (void *) -1;

    while (i < table_size) {
        Client *pointer;
        int offset = i * sizeof(empty);
        fseek(index_table, offset, SEEK_SET);
        fread(&pointer, sizeof(empty), 1, index_table);

        if (pointer == empty) {
            printf("Endereço %d contém:\n", i);
            printf("    Cliente: ********VAZIO*******\n    Código:  ********VAZIO*******\n");
            i++;
            continue;
        }
        
        Client cliente;
        offset = i * sizeof(Client);
        fseek(elements_table, offset, SEEK_SET);
        fread(&cliente, sizeof(Client), 1, elements_table);

        printf("\nEndereço %d contém:\n", i);
        printf("    Cliente: %s\n    Código: %d\n", cliente.nome, cliente.codigo);
        i++;
    }

    fclose(index_table);
    fclose(elements_table);
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <tamanho da tabela>\n", argv[0]);
        return 1;
    }

    int table_size = atoi(argv[1]);
    create_hashtable(table_size);

    char *elements_table = "tables/elements_table";
    char *index_table = "tables/index_table";

    FILE *clientes = fopen("resources/clients.txt", "rb");
    if (clientes == NULL) {
        perror("Erro ao abrir arquivo de clientes");
        return 1;
    }

    Client um, dois;
    fseek(clientes, 0, SEEK_SET);
    fread(&um, sizeof(Client), 1, clientes);
    fseek(clientes, sizeof(Client), SEEK_SET);
    fread(&dois, sizeof(Client), 1, clientes);
    fclose(clientes);

    add(um, elements_table, index_table, table_size);
    add(dois, elements_table, index_table, table_size);

    print_table(elements_table, index_table, table_size);

    return 0;
}