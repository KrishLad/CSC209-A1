#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //for optarg to work
#include "addecho.h"

/**
Returns delay and volume according to input
*/
void parse_input(int argc, char **argv, int *delay, int *volume) {

    int op;
    char *end;
    int D_CONST = 100;
    int V_CONST = 118;

    while ((op = getopt(argc, argv, "d:v:")) != -1) {
        if (op == D_CONST) { // character is d
            *delay = strtol(optarg, &end, 10);
        } else if (op == V_CONST) { // character is v
            *volume = strtol(optarg, &end, 10);
        }
    }
}

/*
Read the 44 bytes in from the header, edit them as necessary, and then put them in the output
*/
void edit_header(FILE **input, FILE **output) {

    //put the header into an array of shorts
    int HEADER_SHORTS = 22;
    
    short *header_arr = malloc(HEADER_SHORTS * sizeof(short));
    fread(header_arr, sizeof(short), HEADER_SHORTS, *input);
    fwrite(header_arr, sizeof(short), HEADER_SHORTS, *output);

    free(header_arr);
}

int main(int argc, char **argv) {

    //getting values of delay and volume
    int delay = 8000;
    int volume = 4;
    parse_input(argc, argv, &delay, &volume);
    
    //getting files for input and output
    char *inputTitle = argv[argc-2];
    char *outputTitle = argv[argc-1];
    FILE *input = fopen(inputTitle, "rb");
    FILE *output = fopen(outputTitle, "wb");

    //this method call should read the 44 bytes in the header, edit them as necessary, 
    edit_header(&input, &output);

    return 0;
}