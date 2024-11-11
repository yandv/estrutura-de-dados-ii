#ifndef CLIENT
#define CLIENT

#define CLIENT_NAME_SIZE 100

typedef struct
{
    int codigo;
    char nome[CLIENT_NAME_SIZE]; 
    int proximo_offset;
    bool ocupado;
} Client;

static size_t writeClient(Client *client, FILE *file)
{
    return fwrite(client, sizeof(Client), 1, file);
}

static Client *readClient(FILE *file)
{
    Client *client = (Client *)malloc(sizeof(Client));

    if (client == NULL)
    {
        return NULL;
    }

    if (fread(client, sizeof(Client), 1, file) != 1)
    {
        if (feof(file))
        {
            return NULL;
        }

        printf("Error reading client in file\n");
        free(client);
        exit(1);
    }

    return client;
}

#endif