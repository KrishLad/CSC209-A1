#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //for optarg to work
#include "addecho.h"
#define HEADER_SIZE 22
/**
Returns delay and volume according to input
*/
void parse_input(int argc, char **argv, int *delay, int *volume)
{

    int op;
    char *end;
    int D_CONST = 100;
    int V_CONST = 118;

    while ((op = getopt(argc, argv, "d:v:")) != -1)
    {
        if (op == D_CONST)
        { // character is d
            *delay = strtol(optarg, &end, 10);
        }
        else if (op == V_CONST)
        { // character is v
            *volume = strtol(optarg, &end, 10);
        }
    }
}

/*
    Edit the ChunkSize(byte 4 / short 2) and Chunk2Size (byte 40 / short 20) so that it doubles in the output.
*/
void edit_header(FILE **input, FILE **output, int *delay){
    /*
        Krishna's note:         
        This function is a a notable choke point in the program. Since this is where I modify the header file. 
        If you are currently debugging then and want to know why either

        i) The output file is not working c
        ii) The size of the output file isn't correct
        iii) There was a "1" at the end of the file input reading error

        Then check 
         - The "Write to output file" or "Editing shorts 20 and 2" sections
         - The "Editing shorts 20 and 2" section
         - The "Read Input File" section.
        
        for the respective issues.
    */
    int error; //for error checking 
    short header[HEADER_SIZE]; //stores the header
    unsigned int* sizeptr; 
    // == Read input file ==
    error = fread(header, HEADER_SIZE, 1, input); //reads in the header.
    if (error  != 1)
        fprintf(stderr, "There was an error in reading the input file: 1\n");
    
    // == Editing the shorts at 20 and 2 == 
    sizeptr = (unsigned int *)(header + 2); //points to byte 2
    sizeptr[0] += *delay * 2;
    sizeptr += 9; // now points to byte 20 
    sizeptr[0] += *delay *2; 
    
    // == Write to output file ==
    int error = fwrite(header,HEADER_SIZE,1,output);
    if (error != 1)
        fprintf(stderr, "Could not write to file :1\n");
}
int main(int argc, char **argv){

    // getting values of delay and volume
    int delay = 8000;
    int volume = 4;
    int error;

    if (argc < 3){
        fprintf(stderr, "Too few arguments, Usage: %s [-d delay] [-v volume_scale] sourcewav destwav\n", argv[0]);
    }

    parse_input(argc, argv, &delay, &volume);

    // getting files for input and output
    char *inputTitle = argv[argc - 2];
    char *outputTitle = argv[argc - 1];
    FILE *input = fopen(inputTitle, "rb"); //source 
    if (input == NULL) // check if file is ever not found
        fprintf(stderr, "File not found\n");
    FILE *output = fopen(outputTitle, "wb"); // destination

    
    /*
        Krishna's Note:
        This edit_header will advance the pointer forward for both input and output so that they are now pointing to after the header.
    */

    edit_header(input, output, &delay); //Edit the header so it increases in size. 

    // === Algorithim === 
    // Step 1: place <delay> amount of the original sound in a buffer

    short *echo_buffer = malloc(sizeof(short) * delay); //stores delay amount of the original sound.
    error = fread(echo_buffer, delay,1,input); //reads a first delay bytes
    if (error != 1)
        fprintf(stderr,"There was an error in reading the input file: 2\n");

    // Scales all <delay> samples of the file by <volume>


    for (int i = 0; i < delay; i++){
        echo_buffer[i] *= volume;
    }

    // Step 2a: If you are not at sample <delay> then copy over the sound of the original.




    return 0;
}