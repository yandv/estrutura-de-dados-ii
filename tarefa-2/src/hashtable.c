#include <stdio.h>
#include <stdlib.h>
#include "../include/bool.h"
#include "../include/client.h"
#include "../include/_aux.h"
#include <math.h>


void create_tabHash(int table_size) {
    FILE *table = fopen("tables/tabHash", "wb");
    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    int empty = -1;  //usando o  -1 para indicar uma entrada vazia
    for (int i = 0; i < table_size; i++) {
        fwrite(&empty, sizeof(int), 1, table);
    }

    fclose(table);
}

void create_tabClientes() {
    FILE *table = fopen("tables/tabClientes", "wb");
    if (table == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    fclose(table);
}

int hash_function(int cod, int table_size, int L) {
    /* printf("CODIGO: %d\n", cod);
    printf("L: %d\n", L);
    printf("TABLE SIZE: %d\n", table_size);
    printf("%d\n", (int)(pow(2, L))); */
    return cod % (int)((table_size * pow(2, L)));
}

//Leva em consideração se o endereço é menor que P. Dessa maneira dita para onde deve ser distribuido os indices.
int mapear(int codigo, int table_size, int P, int L) {
    int endereco = hash_function(codigo, table_size, L); 

    // printf("P É %d\n", P);

    if (endereco < P) {
        endereco = hash_function(codigo, table_size, L + 1); 
        // printf("O ENDEREÇO DO CODIGO %d é %d USANDO HASH 1\n", codigo, endereco);
    } else {
        
        endereco = hash_function(codigo, table_size, L);
        // printf("O ENDEREÇO DO CODIGO %d é %d, USANDO A HASH 2\n", codigo, endereco);
    }
    return endereco;
}



void expands_table(char* dir_tabClientes, char* dir_tabHash, int* table_size, int* P, int* L) {
    int empty = -1;

    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    int new_table_size = *table_size * 2;  
    int new_offset = *table_size;  

    fseek(tabHash, new_offset * sizeof(int), SEEK_SET);
    for (int i = *table_size; i < new_table_size; i++) {
        fwrite(&empty, sizeof(int), 1, tabHash);  // Preenche os novos slots com -1
    }


    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }


    for (int i = 0; i < *table_size; i++) {
        int current_offset;
        fseek(tabHash, i * sizeof(int), SEEK_SET);
        fread(&current_offset, sizeof(int), 1, tabHash);


        while (current_offset != -1) {
            Client cliente;
            fseek(tabClientes, current_offset, SEEK_SET);
            fread(&cliente, sizeof(Client), 1, tabClientes);


            int new_index = mapear(cliente.codigo, new_table_size, *P, *L);
            

            int next_offset;
            fseek(tabHash, new_index * sizeof(int), SEEK_SET);
            fread(&next_offset, sizeof(int), 1, tabHash);

            if (next_offset == -1) {
               
                fseek(tabHash, new_index * sizeof(int), SEEK_SET);
                fwrite(&current_offset, sizeof(int), 1, tabHash);
            } else {
         
                int last_offset = current_offset;
                while (next_offset != -1) {
                    last_offset = next_offset;
                    fseek(tabClientes, next_offset, SEEK_SET);
                    fread(&cliente, sizeof(Client), 1, tabClientes);
                    next_offset = cliente.proximo_offset;
                }

                
                cliente.proximo_offset = -1; // fim da lista encadeada
                fseek(tabClientes, current_offset, SEEK_SET);
                fwrite(&cliente, sizeof(Client), 1, tabClientes); // atualiza o cliente original

              
                fseek(tabHash, new_index * sizeof(int), SEEK_SET);
                fwrite(&current_offset, sizeof(int), 1, tabHash); // atualiza o index de hash com o novo cliente
            }

         
            current_offset = cliente.proximo_offset;
        }
    }

    *P = (*P + 1) % new_table_size;
    if (*P == 0) {
        *table_size = new_table_size;
        (*L)++;
    }

    fclose(tabClientes);
    fclose(tabHash);
}


void add(Client cliente, char* dir_tabClientes, char* dir_tabHash, int* table_size, int* P, float load_factor_limit, int* L) {
    int index = hash_function(cliente.codigo, *table_size, *L);
    int offset;

    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    int current_index_value;
    fseek(tabHash, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, tabHash);

    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    int found_inactive_slot = 0;
    int inactive_offset;
    Client current_client;
    fseek(tabClientes, 0, SEEK_SET);

    while (fread(&current_client, sizeof(Client), 1, tabClientes) == 1) {
        if (!current_client.ocupado) {
            inactive_offset = ftell(tabClientes) - sizeof(Client);
            found_inactive_slot = 1;
            break;
        }
    }

    if (found_inactive_slot) {
        cliente.proximo_offset = -1;
        cliente.ocupado = true;
        fseek(tabClientes, inactive_offset, SEEK_SET);
        fwrite(&cliente, sizeof(Client), 1, tabClientes);

        if (current_index_value == -1) {
            fseek(tabHash, index * sizeof(int), SEEK_SET);
            fwrite(&inactive_offset, sizeof(int), 1, tabHash);
        } else {
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
    } else {
        fseek(tabClientes, 0, SEEK_END);
        offset = ftell(tabClientes);
        cliente.proximo_offset = -1;
        cliente.ocupado = true;
        fwrite(&cliente, sizeof(Client), 1, tabClientes);

        if (current_index_value == -1) {
            fseek(tabHash, index * sizeof(int), SEEK_SET);
            fwrite(&offset, sizeof(int), 1, tabHash);
        } else {
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
    }

    if (checks_fator(tabClientes, *table_size, load_factor_limit) == 1) {
        printf("=========================================");
        printf("\nEXPANDINDO\n");
        printf("=========================================\n");
        expands_table(dir_tabClientes, dir_tabHash, table_size, P, L);
    }

    fclose(tabClientes);
    fclose(tabHash);
}

void delete(int codigo_cliente, char* dir_tabClientes, char* dir_tabHash, int *table_size, int* P, int *L) {
    int index = mapear(codigo_cliente, *table_size, *P, *L);

    FILE* tabHash = fopen(dir_tabHash, "r+b");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        exit(1);
    }

    int current_index_value;
    fseek(tabHash, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, tabHash);

    //indice está vazio (-1), o cliente não existe
    if (current_index_value == -1) {
        printf("Cliente com código %d não encontrado.\n", codigo_cliente);
        fclose(tabHash);
        return;
    }

    //abre a tabela de elementos para percorrer a linkedlist
    FILE* tabClientes = fopen(dir_tabClientes, "r+b");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        exit(1);
    }

    int search_offset = current_index_value; //search_offset é o offset do cliente atual que estamos vendo em tabCLientes 
    int previous_offset = -1;  // offset do cliente anterior, -1 para a cabeça da lista
    Client current_client;
    int found = 0;

    while (search_offset != -1) {
        fseek(tabClientes, search_offset, SEEK_SET);
        fread(&current_client, sizeof(Client), 1, tabClientes);

        if (current_client.codigo == codigo_cliente) {
            //marcando inativo
            current_client.ocupado = false;

            // atualizando os ponteiros das listas encadeadas
            if (previous_offset == -1) {
                //se for head
                fseek(tabHash, index * sizeof(int), SEEK_SET);
                fwrite(&current_client.proximo_offset, sizeof(int), 1, tabHash);
            } else {
                Client previous_client;
                fseek(tabClientes, previous_offset, SEEK_SET);
                fread(&previous_client, sizeof(Client), 1, tabClientes);
                previous_client.proximo_offset = current_client.proximo_offset;

                //aqui é atualizado o cliente, de modo a mudar APENAS o cliente.proximo_offset 
                fseek(tabClientes, previous_offset, SEEK_SET);
                fwrite(&previous_client, sizeof(Client), 1, tabClientes);
            }

            //marca atual como inativo
            fseek(tabClientes, search_offset, SEEK_SET);
            fwrite(&current_client, sizeof(Client), 1, tabClientes);

            printf("Cliente com código %d marcado como excluído.\n", codigo_cliente);
            found = 1;
            break;  //usando break e nao return pra garantir o fechametno dos arquivos
        }

        previous_offset = search_offset;
        search_offset = current_client.proximo_offset;  
    }

    if (!found) {
        printf("Cliente com código %d não encontrado.\n", codigo_cliente);
    }

    fclose(tabClientes);
    fclose(tabHash);
}

Client* find_client_by_code(int codigo, char* dir_tabClientes, char* dir_tabHash, int original_table_size, int P, int L) {
    int index = mapear(codigo, original_table_size, P, L);
    int table_size = original_table_size + P;

    FILE* tabHash = fopen(dir_tabHash, "rb");
    if (tabHash == NULL) {
        perror("Erro ao abrir tabHash");
        return NULL;
    }

    //pega o index que estaria o cliente com base na função mapear
    int current_offset;
    fseek(tabHash, index * sizeof(int), SEEK_SET);
    fread(&current_offset, sizeof(int), 1, tabHash);

    if (current_offset == -1) { //nao tem head no tabHash
        fclose(tabHash);
        printf("Cliente com código %d não encontrado.\n", codigo);
        return NULL;
    }

    FILE* tabClientes = fopen(dir_tabClientes, "rb");
    if (tabClientes == NULL) {
        perror("Erro ao abrir tabClientes");
        fclose(tabHash);
        return NULL;
    }

    //enquanto nao encontrar ou chegar ao final (-1) percorre a lista
    while (current_offset != -1) {
        Client cliente;
        fseek(tabClientes, current_offset, SEEK_SET);
        fread(&cliente, sizeof(Client), 1, tabClientes);

        if (cliente.codigo == codigo && cliente.ocupado) {
            Client* found_client = malloc(sizeof(Client));
            *found_client = cliente;
            fclose(tabClientes);
            fclose(tabHash);
            printf("Cliente com código %d encontrado.\n", codigo);
            return found_client;
        }

        current_offset = cliente.proximo_offset;
    }

    fclose(tabClientes);
    fclose(tabHash);
    printf("Cliente com código %d não encontrado.\n", codigo);
    return NULL;
}

// MAIN PARA TESTES

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <tamanho da tabela>\n", argv[0]);
        return 1;
    }

    int L = 0;
    int P = 0;

    int table_size = atoi(argv[1]);
    float limite_fator_de_carga = 1;
    
    create_tabHash(table_size);
    create_tabClientes();

    char *tabClientes = "tables/tabClientes";
    char *tabHash = "tables/tabHash";

    FILE *clientes = fopen("resources/clients.txt", "rb");
    if (clientes == NULL) {
        perror("Erro ao abrir arquivo de clientes");
        return 1;
    }

    Client um, dois, tres, quatro, cinco;

    fseek(clientes, 0 * sizeof(Client), SEEK_SET);
    fread(&tres, sizeof(Client), 1, clientes);

    fseek(clientes, 1 * sizeof(Client), SEEK_SET);
    fread(&um, sizeof(Client), 1, clientes);

    fseek(clientes, 2 * sizeof(Client), SEEK_SET);
    fread(&dois, sizeof(Client), 1, clientes);

    fseek(clientes, 3 * sizeof(Client), SEEK_SET);
    fread(&quatro, sizeof(Client), 1, clientes);

    fseek(clientes, 4 * sizeof(Client), SEEK_SET);
    fread(&cinco, sizeof(Client), 1, clientes);

    fclose(clientes);


    add(dois, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);
    add(tres, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);


    print_tabHash(tabHash, table_size);
    printf("\n");


    //aqui ele vai expandir
    add(um, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);
    printf("\n");

    add(quatro, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);

    add(cinco, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);

    print_tabHash(tabHash, table_size + P);
    printf("\n");

    print_table(tabClientes, tabHash, table_size + P);
    printf("\n");

    
    print_tabClientes_sequencial(tabClientes);
    printf("\n");


    return 0;
}
