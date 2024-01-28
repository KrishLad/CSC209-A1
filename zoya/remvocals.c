#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getFileName(char **argv, char *title, int i) {

    char *parseInput = argv[i];
    char holder[strlen(parseInput) + 5];
    strcpy(holder, parseInput);
    strcat(holder, ".wav");

    strcpy(title, holder);
}

int main(int argc, char **argv) {

    // === ERROR CHECK ===
    if (argc != 3) {
        return 1;
    } // any other error checks needed? for example, any restriction on file names?

    // === GET THE FILE NAMES === 
    char inputTitle[strlen(argv[1]) + 5];
    char outputTitle[strlen(argv[2]) + 5];
    getFileName(argv, &(inputTitle[0]), 1);
    getFileName(argv, &(outputTitle[0]), 2);
    //printf("input : %s output : %s\n", inputTitle, outputTitle);

    // === INIT THE FILES ===
    FILE *input = fopen(inputTitle, "rb");
    FILE *output = fopen(outputTitle, "wb");

    // === GET THE FIRST 44 BYTES AND MOVE THEM INTO OUTPUT FILE ===
    short *readIn = malloc(44); 
    fread(readIn, 1, 44, input);
    fwrite(readIn, 1, 44, output);
    free(readIn);

    short buffer[2];
    short toWrite[1];
    while (!feof(input)) {
        fread(buffer, sizeof(short), 2, input);

        toWrite[0] = (buffer[1] - buffer[0]) / 2;
        fwrite(toWrite, sizeof(short), 1, output);
        fwrite(toWrite, sizeof(short), 1, output);
    }

    fclose(input);
    fclose(output);

    return 0;
}