#include "../../include/interpolation.h"

// https://www.javatpoint.com/tournament-tree

typedef struct
{
    Client *client;
    bool valid;

    FILE *partitionFile;
} TreeNode;

TreeNode *createTreeNode(Client *client, FILE *partitionFile)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));

    if (node == NULL)
    {
        printf("Could not allocate memory to create new TreeNode for client %d", client->codigo);
        return NULL;
    }

    node->client = client;
    node->partitionFile = partitionFile;
    node->valid = true;

    return node;
}

TreeNode **createWinnerTree(FILE **inputFiles, int inputFilesCount)
{
    TreeNode **winnerTree = (TreeNode **)malloc((inputFilesCount * 2 - 1) * sizeof(TreeNode *));

    if (winnerTree == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < inputFilesCount; i++)
    {
        printf("Reading file %d to index %d\n", i, inputFilesCount - 1 + i);
        FILE *file = inputFiles[i];
        Client *client = readClient(file);

        TreeNode *treeNode = createTreeNode(client, file);

        if (client == NULL)
            treeNode->valid = false;

        winnerTree[inputFilesCount - 1 + i] = treeNode;
    }

    for (int i = inputFilesCount - 2; i >= 0; i--)
    {
        TreeNode *leftChild = winnerTree[2 * i + 1];
        TreeNode *rightChild = winnerTree[2 * i + 2];

        if (leftChild->valid && rightChild->valid)
        {
            if (leftChild->client->codigo < rightChild->client->codigo)
            {
                winnerTree[i] = leftChild;
            }
            else
            {
                winnerTree[i] = rightChild;
            }
        }
        else if (leftChild->valid)
        {
            winnerTree[i] = leftChild;
        }
        else
        {
            winnerTree[i] = rightChild;
        }
    }

    return winnerTree;
}

void updateWinnerTree(TreeNode **winnerTree, Client *client, FILE *inputFile, int inputFilesCount)
{
    int index = -1;

    for (int i = 0; i < inputFilesCount; i++)
    {
        if (winnerTree[inputFilesCount - 1 + i]->partitionFile == inputFile)
        {
            index = i;
            break;
        }
    }

    int position = inputFilesCount - 1 + index;

    winnerTree[position]->client = client;
    winnerTree[position]->valid = client != NULL;

    if (client != NULL)
    {
        printf("Lendo cliente %d %s\n", client->codigo, client->nome);
    }

    while (position > 0)
    {
        int parent = (position - 1) / 2;
        int brother = (position % 2 == 0) ? position - 1 : position + 1;

        if (brother < 2 * inputFilesCount - 1)
        {
            if (winnerTree[brother]->valid && winnerTree[brother]->valid)
            {
                if (winnerTree[brother]->client->codigo < client->codigo)
                {
                    winnerTree[parent] = winnerTree[brother];
                }
                else
                {
                    winnerTree[parent] = winnerTree[position];
                }
            }
            else if (winnerTree[brother]->valid)
            {
                winnerTree[parent] = winnerTree[brother];
            }
            else
            {
                winnerTree[parent] = winnerTree[position];
            }
        }

        position = parent;
    }
}

void startInterpolation(FILE *outputFile, FILE **inputFiles, int inputFilesCount)
{
    TreeNode **winnerTree = createWinnerTree(inputFiles, inputFilesCount);

    if (winnerTree == NULL)
    {
        fclose(outputFile);

        for (int i = 0; i < inputFilesCount; i++)
            fclose(inputFiles[i]);

        printf("Could not malloc memory to WinnerTree");
        exit(1);
    }

    TreeNode *winner = winnerTree[0];

    while (winner != NULL)
    {
        printf("Client %d %s inserted into sorted file\n", winner->client->codigo, winner->client->nome);
        writeClient(winner->client, outputFile);

        updateWinnerTree(winnerTree, readClient(winner->partitionFile), winner->partitionFile, inputFilesCount);
    }

    printf("Finish him!");
}