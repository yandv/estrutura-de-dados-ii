#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../include/bool.h"


int checks_fator(FILE* tabClientes, int table_size, float load_factor_limit) {
    Client cliente;
    int active_count = 0;

    //contar numero de registros (ATIVOS!!!)
    fseek(tabClientes, 0, SEEK_SET); 
    while (fread(&cliente, sizeof(Client), 1, tabClientes) == 1) {
        if (cliente.ocupado) {
            active_count++;
        }
    }

    
    float load_factor = (float)active_count / table_size;
    return load_factor >= load_factor_limit ? 1 : 0;
}

void print_tabHash(char* dir_tabHash, int table_size) {
    FILE* tabHash = fopen(dir_tabHash, "rb");

    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    int empty = -1;
    int pointer;

    printf("Conteúdo de tabHash:\n");
    for (int i = 0; i < table_size; i++) {
        int offset = i * sizeof(int);

        // Posiciona o ponteiro no offset correto e lê o valor
        fseek(tabHash, offset, SEEK_SET);
        fread(&pointer, sizeof(int), 1, tabHash);

        if (pointer == empty) {
            printf("Index %d: Vazio (-1)\n", i);
        } else {
            printf("Index %d: Offset %d em tabClientes\n", i, pointer);
        }
    }
    printf("---------------------------------------\n");
    fclose(tabHash);
}

void print_table(char* dir_tabClientes, char* dir_tabHash, int table_size) {
    FILE* tabClientes = fopen(dir_tabClientes, "rb");
    FILE* tabHash = fopen(dir_tabHash, "rb");

    if (tabClientes == NULL || tabHash == NULL) {
        perror("Erro ao abrir o arquivo de tabela");
        exit(1);
    }

    int i = 0;
    int empty = -1;  // Usando -1 para indicar uma entrada vazia no tabHash

    printf("-----------------------------------------\nCabeças das linked lists em tabHash:\n\n");
    while (i < table_size) {
        int pointer;
        int offset = i * sizeof(int);
        
        // Lê o valor no tabHash
        fseek(tabHash, offset, SEEK_SET);
        fread(&pointer, sizeof(int), 1, tabHash);
        if (pointer == empty) {  // Se o valor for -1, o índice está vazio
            printf("Index %d contém:\n", i);
            printf("    Cliente: ********VAZIO*******\n    Código:  ********VAZIO*******\n");
        } else {
            Client cliente;
            // Posiciona o ponteiro em tabClientes com o endereço obtido em pointer
            fseek(tabClientes, pointer, SEEK_SET);
            fread(&cliente, sizeof(Client), 1, tabClientes);

            printf("Index %d contém:\n", i);
            printf("    Cliente: %s\n    Código: %d\n", cliente.nome, cliente.codigo);
        }
        i++;
    }

    fclose(tabHash);
    fclose(tabClientes);
}

void print_tabClientes_sequencial(char* dir_tabClientes) {
    FILE* tabClientes = fopen(dir_tabClientes, "rb");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        exit(1);
    }

    Client cliente;
    
    printf("Conteúdo da tabClientes:\n");
    
    // Loop que lê os clientes em sequência até o fim do arquivo
    while (fread(&cliente, sizeof(Client), 1, tabClientes)) {
        
        printf("Cliente: %s\n", cliente.nome);
        printf("Código: %d\n", cliente.codigo);
        printf("Próximo Offset: %d\n", cliente.proximo_offset);
        printf("---------------------------------------\n");
    }

    fclose(tabClientes);
}
