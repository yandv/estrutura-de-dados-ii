#include "../../include/interpolation.h"
#include <math.h>

// https://www.javatpoint.com/tournament-tree

typedef struct
{
    Client *client;
    bool valid;

    FILE *partitionFile;
    int partitionIndex;
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

void printTree(TreeNode **winnerTree, int inputFilesCount)
{
    int level = 0;
    int levelSize = 1;

    printf("\n");

    for (int i = 0; i < inputFilesCount * 2 - 1; i++)
    {
        if (i == levelSize)
        {
            printf("\n");
            level++;
            levelSize += pow(2, level);
        }

        if (winnerTree[i]->valid)
        {
            printf("%d (%d) ", winnerTree[i]->client->codigo, winnerTree[i]->partitionIndex);
        }
        else
        {
            printf("NULL (%d) ", winnerTree[i]->partitionIndex);
        }
    }

    printf("\n\n");
}

TreeNode **createWinnerTree(FILE **inputFiles, int inputFilesCount)
{
    TreeNode **winnerTree = (TreeNode **)malloc((inputFilesCount * 2 - 1) * sizeof(TreeNode *));

    if (winnerTree == NULL)
    {
        return NULL;
    }

    printf("Creating winner tree with %d nodes\n\n", inputFilesCount * 2 - 1);

    for (int i = 0; i < inputFilesCount; i++)
    {
        printf("Created node %d\n", i);
        FILE *file = inputFiles[i];
        Client *client = readClient(file);

        TreeNode *treeNode = createTreeNode(client, file);

        if (client == NULL)
            treeNode->valid = false;

        treeNode->partitionIndex = i;
        winnerTree[inputFilesCount - 1 + i] = treeNode;
    }

    printf("\nCreated %d nodes successfully\n", inputFilesCount);

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

    printTree(winnerTree, inputFilesCount);

    return winnerTree;
}

void updateWinnerTree(TreeNode **winnerTree, Client *client, FILE *inputFile, int inputFilesCount)
{
    int position = -1;

    for (int i = inputFilesCount * 2 - 2; i >= 0; i--)
    {
        if (winnerTree[i]->partitionFile == inputFile)
        {
            position = i;
            break;
        }
    }

    winnerTree[position]->client = client;
    winnerTree[position]->valid = client != NULL;

    while (position > 0)
    {
        int parent = (position - 1) / 2;
        int brother = (position % 2 == 0) ? position - 1 : position + 1;

        if (brother < 2 * inputFilesCount - 1)
        {
            if (winnerTree[brother]->valid && winnerTree[position]->valid)
            {
                if (winnerTree[brother]->client->codigo < winnerTree[position]->client->codigo)
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

    while (winner->valid)
    {
        printf("Client %d %s from %d inserted into sorted file\n", winner->client->codigo, winner->client->nome, winner->partitionIndex);
        writeClient(winner->client, outputFile);

        updateWinnerTree(winnerTree, readClient(winner->partitionFile), winner->partitionFile, inputFilesCount);
        winner = winnerTree[0];
    }

    free(winnerTree);
    free(winner);
    printf("\n");
}