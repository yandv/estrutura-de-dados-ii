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
        printf("Cabeças das linkedlists:\n");
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

void print_elements_table_sequencial(char* dir_elements_table) {
    FILE* elements_table = fopen(dir_elements_table, "rb");
    if (elements_table == NULL) {
        perror("Erro ao abrir elements_table");
        exit(1);
    }

    Client cliente;
    
    printf("Conteúdo da elements_table:\n");
    
    // Loop que lê os clientes em sequência até o fim do arquivo
    while (fread(&cliente, sizeof(Client), 1, elements_table)) {
        
        printf("Cliente: %s\n", cliente.nome);
        printf("Código: %d\n", cliente.codigo);
        printf("Próximo Offset: %d\n", cliente.proximo_offset);
        printf("---------------------------------------\n");
    }

    fclose(elements_table);
}

int hash_function(int cod, int table_size) {
    return cod % table_size;
}

void add(Client cliente, char* dir_elements_table, char* dir_index_table, int table_size) {

    int index = hash_function(cliente.codigo, table_size);
    int offset;

    // Abre a tabela de índices para verificar o índice
    FILE* index_table = fopen(dir_index_table, "r+b");
    if (index_table == NULL) {
        perror("Erro ao abrir index_table");
        exit(1);
    }
    
    // Verifica se o lugar está vazio (-1)
    int current_index_value;
    fseek(index_table, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, index_table);

    // Tenta abrir o arquivo elements_table para inserir o cliente
    FILE* elements_table = fopen(dir_elements_table, "r+b");
    if (elements_table == NULL) {
        perror("Erro ao abrir elements_table");
        fclose(index_table);
        exit(1);
    }

    // Flag para verificar se encontramos um cliente inativo
    int found_inactive_slot = 0;
    int inactive_offset;

    // Percorre o arquivo elements_table para encontrar um cliente com ocupado == false
    Client current_client;
    fseek(elements_table, 0, SEEK_SET); // Volta ao início do arquivo

    while (fread(&current_client, sizeof(Client), 1, elements_table) == 1) {
        if (!current_client.ocupado) {  // Se achou um cliente inativo
            inactive_offset = ftell(elements_table) - sizeof(Client);  // Calcula o offset desse cliente
            found_inactive_slot = 1;
            break;
        }
    }

    // Se encontramos um cliente inativo, usaremos seu espaço
    if (found_inactive_slot) {
        cliente.proximo_offset = -1;  // Inicialmente define o proximo_offset como -1
        cliente.ocupado = true;
        fseek(elements_table, inactive_offset, SEEK_SET);
        fwrite(&cliente, sizeof(Client), 1, elements_table);
        
        // Atualiza o índice, se necessário, se o índice estava vazio
        if (current_index_value == -1) {
            fseek(index_table, index * sizeof(int), SEEK_SET);
            fwrite(&inactive_offset, sizeof(int), 1, index_table);
        } else {
            // Trata colisão: Atualiza o último elemento da lista encadeada para apontar para o novo cliente
            int last_offset = current_index_value;
            while (last_offset != -1) {
                fseek(elements_table, last_offset, SEEK_SET);
                fread(&current_client, sizeof(Client), 1, elements_table);
                
                if (current_client.proximo_offset == -1) {
                    current_client.proximo_offset = inactive_offset;
                    fseek(elements_table, last_offset, SEEK_SET);
                    fwrite(&current_client, sizeof(Client), 1, elements_table);
                    break;
                }
                
                last_offset = current_client.proximo_offset;
            }
        }

        printf("---------------------------------------\nCliente %s adicionado (ocupando espaço inativo), hash: %d\n---------------------------------------\n", cliente.nome, index);

    } else {
        // Se não encontramos um cliente inativo, adicionamos o cliente ao final do arquivo
        fseek(elements_table, 0, SEEK_END);
        offset = ftell(elements_table);
        cliente.proximo_offset = -1;
        cliente.ocupado = true;
        fwrite(&cliente, sizeof(Client), 1, elements_table);

        // Atualiza o índice com o offset do cliente em elements_table, se o índice estava vazio
        if (current_index_value == -1) {
            fseek(index_table, index * sizeof(int), SEEK_SET);
            fwrite(&offset, sizeof(int), 1, index_table);
        } else {
            // Trata colisão: Atualiza o último elemento da lista encadeada para apontar para o novo cliente
            int last_offset = current_index_value;
            while (last_offset != -1) {
                fseek(elements_table, last_offset, SEEK_SET);
                fread(&current_client, sizeof(Client), 1, elements_table);

                if (current_client.proximo_offset == -1) {
                    current_client.proximo_offset = offset;
                    fseek(elements_table, last_offset, SEEK_SET);
                    fwrite(&current_client, sizeof(Client), 1, elements_table);
                    break;
                }

                last_offset = current_client.proximo_offset;
            }
        }

        printf("---------------------------------------\nCliente %s adicionado ao final, hash: %d\n---------------------------------------\n", cliente.nome, index);
    }

    fclose(elements_table);
    fclose(index_table);
}


/*

void add(Client cliente, char* dir_elements_table, char* dir_index_table, int table_size) {

    int index = hash_function(cliente.codigo, table_size);
    int offset;

    // Abrindo index_table para verificar o índice
    FILE* index_table = fopen(dir_index_table, "r+b");
    if (index_table == NULL) {
        perror("Erro ao abrir index_table");
        exit(1);
    }
    
    // Verifica se o lugar está vazio (-1)
    int current_index_value;
    fseek(index_table, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, index_table);

    // Se o índice está vazio (-1), adicionamos o cliente
    if (current_index_value == -1) {

        // Abrindo e adicionando o cliente ao final de elements_table
        FILE* elements_table = fopen(dir_elements_table, "a+b");
        if (elements_table == NULL) {
            perror("Erro ao abrir elements_table");
            fclose(index_table);
            exit(1);
        }

        // Pega a posição atual (final do arquivo) para o offset e escreve o cliente
        fseek(elements_table, 0, SEEK_END);
        offset = ftell(elements_table);
        cliente.proximo_offset = -1;  // O próximo offset será -1, pois é o último cliente da lista
        fwrite(&cliente, sizeof(Client), 1, elements_table);
        fclose(elements_table);

        // Atualiza o índice com o offset do cliente em elements_table
        fseek(index_table, index * sizeof(int), SEEK_SET);  // Reposiciona para escrever
        fwrite(&offset, sizeof(int), 1, index_table);
        fclose(index_table);

        printf("---------------------------------------\nCliente %s adicionado, hash: %d\n---------------------------------------\n", cliente.nome, index);
    } else {
        printf("LUGAR OCUPADO para o hash %d. Preparando tratamento de colisão...\n", index);
        
        // Abrindo elements_table para buscar o último cliente da lista encadeada
        FILE* elements_table = fopen(dir_elements_table, "r+b");
        if (elements_table == NULL) {
            perror("Erro ao abrir elements_table");
            fclose(index_table);
            exit(1);
        }

        int search_offset = current_index_value;  // Começa no primeiro cliente da lista encadeada
        int last_offset;
        Client current_client;
        
        //varre a lista encadeada até achar o -1 que indica seu fim
        while (search_offset != -1) {
            fseek(elements_table, search_offset, SEEK_SET);
            fread(&current_client, sizeof(Client), 1, elements_table);
            last_offset = search_offset;    //sempre salvando o penultimo offset pois o endereço vai ser perdido quando encontrarmos o -1
            search_offset = current_client.proximo_offset;
        }

        //adiciona o cliente ao final da linkedlist
        fseek(elements_table, 0, SEEK_END);
        offset = ftell(elements_table);
        cliente.proximo_offset = -1;
        fwrite(&cliente, sizeof(Client), 1, elements_table);
        
        //aqui atualizamos o proximo elemento do antigo ultimo da linkedlist
        fseek(elements_table, last_offset, SEEK_SET);
        current_client.proximo_offset = offset;
        fwrite(&current_client, sizeof(Client), 1, elements_table);
        
        fclose(elements_table);
        fclose(index_table);

        printf("---------------------------------------\nCliente %s adicionado, hash: %d\nTratamento de colisão realizado\n---------------------------------------\n", cliente.nome, index);
    }
}

*/void delete(int codigo_cliente, char* dir_elements_table, char* dir_index_table, int table_size) {

    int index = hash_function(codigo_cliente, table_size);

    FILE* index_table = fopen(dir_index_table, "r+b");
    if (index_table == NULL) {
        perror("Erro ao abrir index_table");
        exit(1);
    }
    
    int current_index_value;
    fseek(index_table, index * sizeof(int), SEEK_SET);
    fread(&current_index_value, sizeof(int), 1, index_table);

    // Se o índice está vazio (-1), o cliente não existe
    if (current_index_value == -1) {
        printf("Cliente com código %d não encontrado.\n", codigo_cliente);
        fclose(index_table);
        return;
    }

    // Abre a tabela de elementos para percorrer a lista encadeada
    FILE* elements_table = fopen(dir_elements_table, "r+b");
    if (elements_table == NULL) {
        perror("Erro ao abrir elements_table");
        fclose(index_table);
        exit(1);
    }

    int search_offset = current_index_value;  // Primeiro da lista
    int previous_offset = -1;  // Offset do cliente anterior, -1 para a cabeça da lista
    Client current_client;
    int found = 0;

    // Percorre a lista encadeada até encontrar o cliente ou chegar ao final
    while (search_offset != -1) {
        fseek(elements_table, search_offset, SEEK_SET);
        fread(&current_client, sizeof(Client), 1, elements_table);

        if (current_client.codigo == codigo_cliente) {
            // Marca o cliente como inativo
            current_client.ocupado = false;

            // Atualiza a lista encadeada
            if (previous_offset == -1) {
                // Caso 1: Cliente é a cabeça da lista
                fseek(index_table, index * sizeof(int), SEEK_SET);
                fwrite(&current_client.proximo_offset, sizeof(int), 1, index_table);
            } else {
                // Caso 2: Cliente não é a cabeça da lista
                Client previous_client;
                fseek(elements_table, previous_offset, SEEK_SET);
                fread(&previous_client, sizeof(Client), 1, elements_table);
                previous_client.proximo_offset = current_client.proximo_offset;
                
                // Escreve o cliente anterior com o próximo atualizado
                fseek(elements_table, previous_offset, SEEK_SET);
                fwrite(&previous_client, sizeof(Client), 1, elements_table);
            }

            // Marca o cliente atual como inativo
            fseek(elements_table, search_offset, SEEK_SET);
            fwrite(&current_client, sizeof(Client), 1, elements_table);

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

    fclose(elements_table);
    fclose(index_table);
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

    add(dois, elements_table, index_table, table_size);
    add(tres, elements_table, index_table, table_size);
    add(um, elements_table, index_table, table_size);

    print_index_table(index_table, table_size);
    print_elements_table_sequencial(elements_table);
    print_table(elements_table, index_table, table_size);

    //deletando

    delete(tres.codigo, elements_table, index_table, table_size);
    add(um, elements_table, index_table, table_size);
    printf("\n");
    print_elements_table_sequencial(elements_table);



    return 0;
}