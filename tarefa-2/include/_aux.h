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

    printf("Conteudo de tabHash:\n");
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
    printf("=========================================\n");
    fclose(tabHash);
}

void print_table(char* dir_tabClientes, char* dir_tabHash, int table_size) {
    FILE* tabClientes = fopen(dir_tabClientes, "rb");
    FILE* tabHash = fopen(dir_tabHash, "rb");

    if (tabClientes == NULL || tabHash == NULL) {
        perror("Erro ao abrir o arquivo de tabela");
        exit(1);
    }

    int empty = -1;

    printf("=========================================\n");

    printf("Listas encadeadas da tabela:\n\n");

    for (int i = 0; i < table_size; i++) {
        int pointer;
        int offset = i * sizeof(int);

        fseek(tabHash, offset, SEEK_SET);
        fread(&pointer, sizeof(int), 1, tabHash);

        printf("Index %d: ", i);
        if (pointer == empty) { 
            printf(" ---> VAZIO   ");
        } else {
            while (pointer != empty) {
                Client cliente;

                fseek(tabClientes, pointer, SEEK_SET);
                fread(&cliente, sizeof(Client), 1, tabClientes);

                if (cliente.ocupado) {
                    printf(" ---> [Cliente: %s, Codigo: %d] ", cliente.nome, cliente.codigo);
                } else {
                    printf("[Cliente: ********REMOVIDO********] ");
                }

                pointer = cliente.proximo_offset;
            }
        }
        printf("\n");
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
    
    printf("=========================================\n");
    printf("Conteudo completo de tabClientes (sequencial):\n");
    printf("-----------------------------------------\n");
    
    // Loop que lê os clientes em sequência até o fim do arquivo
    while (fread(&cliente, sizeof(Client), 1, tabClientes)) {
        
        printf("Cliente: %s\n", cliente.nome);
        printf("Codigo: %d\n", cliente.codigo);
        printf("Proximo Offset: %d\n", cliente.proximo_offset);

        printf("-----------------------------------------\n");
    }

    fclose(tabClientes);
}
