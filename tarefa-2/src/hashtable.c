#include <stdio.h>
#include <stdlib.h>
#include "../include/bool.h"
#include "../include/client.h"
#include "aux.h"
#include <math.h>

int L = 0;
int P = 0;

void create_hashtable(int table_size) {
    FILE *table = fopen("tables/tabHash", "wb");
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

int hash_function(int cod, int table_size, int L) {
    return cod % (int)((table_size * pow(2, L)));
}

void add(Client cliente, char* dir_tabClientes, char* dir_tabHash, int* table_size, int* P, float load_factor_limit) {

    int index = hash_function(cliente.codigo, table_size, L);
    int offset;

    // Abre a tabela de índices para verificar o índice
    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }
    
    // Verifica se o lugar está vazio (-1)
    int current_index_value;
    fseek(tabHash, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, tabHash);

    // Tenta abrir o arquivo tabClientes para inserir o cliente
    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    // Flag para verificar se encontramos um cliente inativo
    int found_inactive_slot = 0;
    int inactive_offset;

    // Percorre o arquivo tabClientes para encontrar um cliente com ocupado == false
    Client current_client;
    fseek(tabClientes, 0, SEEK_SET); // Volta ao início do arquivo

    while (fread(&current_client, sizeof(Client), 1, tabClientes) == 1) {
        if (!current_client.ocupado) {  // Se achou um cliente inativo
            inactive_offset = ftell(tabClientes) - sizeof(Client);  // Calcula o offset desse cliente
            found_inactive_slot = 1;
            break;
        }
    }

    // Se encontramos um cliente inativo, usaremos seu espaço
    if (found_inactive_slot) {
        cliente.proximo_offset = -1;  // Inicialmente define o proximo_offset como -1
        cliente.ocupado = true;
        fseek(tabClientes, inactive_offset, SEEK_SET);
        fwrite(&cliente, sizeof(Client), 1, tabClientes);
        
        // Atualiza o índice, se necessário, se o índice estava vazio
        if (current_index_value == -1) {
            fseek(tabHash, index * sizeof(int), SEEK_SET);
            fwrite(&inactive_offset, sizeof(int), 1, tabHash);
        } else {
            // Trata colisão: Atualiza o último elemento da lista encadeada para apontar para o novo cliente
            int last_offset = current_index_value;
            while (last_offset != -1) {
                fseek(tabClientes, last_offset, SEEK_SET);
                fread(&current_client, sizeof(Client), 1, tabClientes);
                
                if (current_client.proximo_offset == -1) {
                    current_client.proximo_offset = inactive_offset;
                    fseek(tabClientes, last_offset, SEEK_SET);
                    fwrite(&current_client, sizeof(Client), 1, tabClientes);
                    break;
                }
                
                last_offset = current_client.proximo_offset;
            }
        }

        printf("---------------------------------------\nCliente %s adicionado (ocupando espaço inativo de um cliente removido), hash: %d\n---------------------------------------\n", cliente.nome, index);

    } else {
        // Se não encontramos um cliente inativo, adicionamos o cliente ao final do arquivo
        fseek(tabClientes, 0, SEEK_END);
        offset = ftell(tabClientes);
        cliente.proximo_offset = -1;
        cliente.ocupado = true;
        fwrite(&cliente, sizeof(Client), 1, tabClientes);

        // Atualiza o índice com o offset do cliente em tabClientes, se o índice estava vazio
        if (current_index_value == -1) {
            fseek(tabHash, index * sizeof(int), SEEK_SET);
            fwrite(&offset, sizeof(int), 1, tabHash);
        } else {
            // Trata colisão: Atualiza o último elemento da lista encadeada para apontar para o novo cliente
            int last_offset = current_index_value;
            while (last_offset != -1) {
                fseek(tabClientes, last_offset, SEEK_SET);
                fread(&current_client, sizeof(Client), 1, tabClientes);

                if (current_client.proximo_offset == -1) {
                    current_client.proximo_offset = offset;
                    fseek(tabClientes, last_offset, SEEK_SET);
                    fwrite(&current_client, sizeof(Client), 1, tabClientes);
                    break;
                }

                last_offset = current_client.proximo_offset;
            }
        }

        printf("---------------------------------------\nCliente %s adicionado ao final, hash: %d\n---------------------------------------\n", cliente.nome, index);
    }
    

    /*AO FIM DE CADA ADICÇÃO, VERIFICAR SE PRECISA EXPANDIR
    VERIFICAR SE PRECISA EXPANDIR*/

    if(checks_fator(tabClientes, table_size, load_factor_limit)==1){ 
        expands_table(tabHash, table_size); //falta implementar expands table pq nao lembro o algoritmo 
    }

    fclose(tabClientes);
    fclose(tabHash);
}

void delete(int codigo_cliente, char* dir_tabClientes, char* dir_tabHash, int table_size) {

    int index = hash_function(codigo_cliente, table_size, L);

    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }
    
    int current_index_value;
    fseek(tabHash, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, tabHash);

    // Se o índice está vazio (-1), o cliente não existe
    if (current_index_value == -1) {
        printf("Cliente com código %d não encontrado.\n", codigo_cliente);
        fclose(tabHash);
        return;
    }

    // Abre a tabela de elementos para percorrer a lista encadeada
    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    int search_offset = current_index_value;  
    int previous_offset = -1;  // offset do cliente anterior, -1 para a cabeça da lista
    Client current_client;
    int found = 0;

    while (search_offset != -1) {
        fseek(tabClientes, search_offset, SEEK_SET);
        fread(&current_client, sizeof(Client), 1, tabClientes);

        if (current_client.codigo == codigo_cliente) {
            // Marca o cliente como inativo
            current_client.ocupado = false;

            //atualiza a lista encadeada
            if (previous_offset == -1) {
                // Caso 1: Cliente é a cabeça da lista
                fseek(tabHash, index * sizeof(int), SEEK_SET);
                fwrite(&current_client.proximo_offset, sizeof(int), 1, tabHash);
            } else {
                // Caso 2: Cliente não é a cabeça da lista
                Client previous_client;
                fseek(tabClientes, previous_offset, SEEK_SET);
                fread(&previous_client, sizeof(Client), 1, tabClientes);
                previous_client.proximo_offset = current_client.proximo_offset;
                
                // Escreve o cliente anterior com o próximo atualizado
                fseek(tabClientes, previous_offset, SEEK_SET);
                fwrite(&previous_client, sizeof(Client), 1, tabClientes);
            }

            // Marca o cliente atual como inativo
            fseek(tabClientes, search_offset, SEEK_SET);
            fwrite(&current_client, sizeof(Client), 1, tabClientes);

            printf("Cliente com código %d marcado como excluído.\n", codigo_cliente);
            found = 1;
            break;  // Encerra o loop para garantir que os arquivos serão fechados
        }

        previous_offset = search_offset;  // Atualiza o offset anterior
        search_offset = current_client.proximo_offset;  // Move para o próximo cliente na lista
    }

    if (!found) {
        printf("Cliente com código %d não encontrado.\n", codigo_cliente);
    }

    fclose(tabClientes);
    fclose(tabHash);
}

//MAIN PARA TESTES

int main(int argc, char const *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <tamanho da tabela>\n", argv[0]);
        return 1;
    }

    //configuração inicial
    int L = 0;
    int P = 0;
    int table_size = atoi(argv[1]);
    float limite_fator_de_carga = 1;

    create_hashtable(table_size);

    char *tabClientes = "tables/tabClientes";
    char *tabHash = "tables/tabHash";

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

    add(dois, tabClientes, tabHash, table_size);
    add(tres, tabClientes, tabHash, table_size);
    add(um, tabClientes, tabHash, table_size);

    print_tabHash(tabHash, table_size);
    print_tabClientes_sequencial(tabClientes);
    print_table(tabClientes, tabHash, table_size);

    //deletando

    delete(tres.codigo, tabClientes, tabHash, table_size);
    add(um, tabClientes, tabHash, table_size);
    printf("\n");
    print_tabClientes_sequencial(tabClientes);



    return 0;
}