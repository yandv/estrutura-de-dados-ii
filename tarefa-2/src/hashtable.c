#include <stdio.h>
#include <stdlib.h>
#include "../include/bool.h"
#include "../include/client.h"

void create_hashtable(int table_size) {
    FILE *table = fopen("tables/index_table", "wb");
    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    int empty = -1;  // Usando o valor -1 para indicar uma entrada vazia
    for (int i = 0; i < table_size; i++) {
        fwrite(&empty, sizeof(int), 1, table);  // Escrevendo um int em vez de um ponteiro
    }

    fclose(table);
}

int hash_function(int cod, int table_size) {
    return cod % table_size;
}

void add(Client cliente, char* dir_elements_table, char* dir_index_table, int table_size) {
    int index = hash_function(cliente.codigo, table_size);
    int offset;

    // Abrindo e adicionando o cliente ao final de elements_table
    FILE* elements_table = fopen(dir_elements_table, "a+b");  
    if (elements_table == NULL) {
        perror("Erro ao abrir elements_table");
        exit(1);
    }

    // Pega a posição atual (final do arquivo) para o offset e escreve o cliente
    fseek(elements_table, 0, SEEK_END);
    offset = ftell(elements_table);
    fwrite(&cliente, sizeof(Client), 1, elements_table);
    fclose(elements_table);

    // Abrindo index_table para verificar e atualizar o índice
    FILE* index_table = fopen(dir_index_table, "r+b");
    if (index_table == NULL) {
        perror("Erro ao abrir index_table");
        exit(1);
    }

    // Posiciona o cursor no índice correto e lê o valor atual
    int current_index_value;
    fseek(index_table, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, index_table);

    // Se o índice estiver vazio (-1), atualiza com o offset do cliente em elements_table
    if (current_index_value == -1) {
        fseek(index_table, index * sizeof(int), SEEK_SET);  // Reposiciona para escrever
        fwrite(&offset, sizeof(int), 1, index_table);
    }

    fclose(index_table);

    printf("---------------------------------------\nCliente %s adicionado, hash: %d\n---------------------------------------\n", cliente.nome, index);
}


void print_index_table(char* dir_index_table, int table_size) {
    FILE* index_table = fopen(dir_index_table, "rb");

    if (index_table == NULL) {
        perror("Erro ao abrir index_table");
        exit(1);
    }

    int empty = -1;
    int pointer;

    printf("Conteúdo de index_table:\n");
    for (int i = 0; i < table_size; i++) {
        int offset = i * sizeof(int);

        // Posiciona o ponteiro no offset correto e lê o valor
        fseek(index_table, offset, SEEK_SET);
        fread(&pointer, sizeof(int), 1, index_table);

        if (pointer == empty) {
            printf("Posição %d: Vazio (-1)\n", i);
        } else {
            printf("Posição %d: Offset %d em elements_table\n", i, pointer);
        }
    }
    printf("---------------------------------------\n");
    fclose(index_table);
}



void print_table(char* dir_elements_table, char* dir_index_table, int table_size) {
    FILE* elements_table = fopen(dir_elements_table, "rb");
    FILE* index_table = fopen(dir_index_table, "rb");

    if (elements_table == NULL || index_table == NULL) {
        perror("Erro ao abrir o arquivo de tabela");
        exit(1);
    }

    int i = 0;
    int empty = -1;  // Usando -1 para indicar uma entrada vazia no index_table

    while (i < table_size) {
        int pointer;
        int offset = i * sizeof(int);
        
        // Lê o valor no index_table
        fseek(index_table, offset, SEEK_SET);
        fread(&pointer, sizeof(int), 1, index_table);

        if (pointer == empty) {  // Se o valor for -1, o índice está vazio
            printf("Endereço %d contém:\n", i);
            printf("    Cliente: ********VAZIO*******\n    Código:  ********VAZIO*******\n");
        } else {
            Client cliente;
            // Posiciona o ponteiro em elements_table com o endereço obtido em pointer
            fseek(elements_table, pointer, SEEK_SET);
            fread(&cliente, sizeof(Client), 1, elements_table);

            printf("Endereço %d contém:\n", i);
            printf("    Cliente: %s\n    Código: %d\n", cliente.nome, cliente.codigo);
        }
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

    Client um, dois, tres;
    
    fseek(clientes, 0 * sizeof(Client) , SEEK_SET);
    fread(&tres, sizeof(Client), 1, clientes);

    fseek(clientes, 1 * sizeof(Client), SEEK_SET);
    fread(&um, sizeof(Client), 1, clientes);

    fseek(clientes, 2* sizeof(Client), SEEK_SET);
    fread(&dois, sizeof(Client), 1, clientes);
    fclose(clientes);

    add(um, elements_table, index_table, table_size);
    add(dois, elements_table, index_table, table_size);
    add(tres, elements_table, index_table, table_size);

    print_index_table(index_table, table_size);
    print_table(elements_table, index_table, table_size);

    return 0;
}