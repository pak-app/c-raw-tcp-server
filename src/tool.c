
#include <stdio.h>

void readData(char *fileName)
{
    FILE *fptr;

    fptr = fopen(fileName, "r");

    char buffer[8];
    size_t bytesRead;

    if (fptr == NULL)
    {
        printf("File %s not found", fileName);
        return;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr)) > 0)
    {
        // Print each byte in hex
        for (size_t i = 0; i < bytesRead; i++)
        {
            printf("%c ", buffer[i]);
        }
        printf("\n");
    }

    fclose(fptr);
}

void readLines(char *fileName)
{
    FILE *fptr;

    fptr = fopen(fileName, "r");

    char buffer[8];
    size_t bytesRead;

    if (fptr == NULL)
    {
        printf("File %s not found", fileName);
        return;
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr)) > 0)
    {
        // Print each byte in hex
        for (size_t i = 0; i < bytesRead; i++)
        {
            if(buffer[i] == "\n") {
                printf("\n");
            }
            printf("%c", buffer[i]);
        }
    }

    fclose(fptr);
}