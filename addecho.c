#include <stdio.h>
#include <stdlib.h>
#include<stdint.h> // for uint32
#include <unistd.h> //for optarg to work
#include <getopt.h>
#include "addecho.h"
#define HEADER_SIZE 44
#define SIZE_OFFSET 40


void printArray(short arr[], int size) 
{
    printf("[");
    for (int i = 0; i < size; i++)
    {
        printf("%d", arr[i]);
        if (i < size - 1)
        {
            printf(", ");
        }
    }
    printf("]\n");
}
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
        // check for optarg
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
    Edits the 4th and 40th bytes to increase the file size
*/
void edit_header(FILE *input, FILE *output, int delay){
   
    int error;  
    short header[HEADER_SIZE];
    unsigned int* sizeptr; 
    
    // == Read input file ==
    error = fread(header, HEADER_SIZE, 1, input); //reads in the header.
    if (error  != 1){
        fprintf(stderr, "There was an error in reading the input file: 1\n");
        exit(1);
    }

    // == Editing the shorts at 20 and 2 ==
    sizeptr = (unsigned int *)(header + 2);
    sizeptr[0] += delay * 2;
    sizeptr = (unsigned int *)(header + 20);
    sizeptr[0] += delay * 2;
    // == Write to output file ==
    error = fwrite(header,HEADER_SIZE, 1, output);
    if (error != 1){
        fprintf(stderr, "Could not write to file :1\n");
        exit(1);
    }



}


int main(int argc, char **argv){

    // ===Input Parsing ===

    // getting values of delay and volume
    int delay = 8000;
    int volume = 4;
    int error;
    if (argc < 3){
        fprintf(stderr, "Too few arguments, Usage: %s [-d delay] [-v volume_scale] sourcewav destwav\n", argv[0]);
        exit(1);
    }

    parse_input(argc, argv, &delay, &volume);

    // getting files for input and output
    char *inputTitle = argv[argc - 2];
    char *outputTitle = argv[argc - 1];
    FILE *input = fopen(inputTitle, "rb"); //source 
    if (input == NULL) 
        fprintf(stderr, "File not found\n");
    FILE *output = fopen(outputTitle, "wb"); // destination

    // === Editing Header ===

    edit_header(input, output, delay); 
    fseek(input, HEADER_SIZE, SEEK_SET); // sets the pointer to be after header
    // === Algorithim === 
    // Step 1: place <delay> amount of the original sound in a buffer

    short *echo_buffer = malloc(sizeof(short) * delay); // has the echo scaled by <volume>
    short  *original_sound = malloc(sizeof(short) * delay);//stores delay amount of the original sound.
    for (int i =0; i< delay; i++){
        fread(&original_sound[i], sizeof(short),1, input);
    }
   
    // Scales all <delay> samples of the file by <volume>
    for (int i = 0; i < delay; i++){
        echo_buffer[i] = original_sound[i] / volume; // keeps a copy of the original sound.
    }
   
    // Step 2a: If you are not at sample <delay> then copy over the sound of the original.
    for (int i = 0; i < delay; i++){
        error = fwrite(&original_sound[i], sizeof(short), 1, output);
        if (error != 1){
            fprintf(stderr, "There was an error in writing to the output file: 2\n");
            exit(1);
        }
    }
    // we are now at sample <delay> (and both the files are sync'd)
    // Step 2b: Start mixing in the buffer.
    short sample; // variable to store the current sample read from the input file
    int index = 0; // index to keep track of the position in the echo buffer
    while(fread(&sample, sizeof(short), 1, input) == 1){
        short mixed = sample + echo_buffer[index]; // Mix the sample and the echo buffer together
        echo_buffer[index] = sample / volume; //replace the current position of the echo buffer with the current sample scaled to volume.
        error = fwrite(&mixed, sizeof(short),1,output); //write to the file.
        if (error != 1){
            fprintf(stderr, "There was an error in writing to the output file: 3\n");
            exit(1);
        } 
        index = (index + 1) % delay; // To wrap around.
    }

    //Step 3 : Write 0 samples
    fseek(input, SIZE_OFFSET, SEEK_SET);
    uint32_t data_chunk_size;
    fread(&data_chunk_size,sizeof(uint32_t),1,input);
    fseek(input, 0, SEEK_END);
    int x = delay - data_chunk_size;
    if (x > 0){
        for (int i=0; i < x; i++){
            short zero = 0;
            fwrite(&zero, sizeof(short), 1, output);
        }
    }
    //Step 4: Clear out the echo buffer samples
    for (int i = 0; i < delay; i++){
        short array_element = echo_buffer[i]; 
        error = fwrite(&array_element, sizeof(short), 1,output);
    }
    // === Clean up === 
    free(echo_buffer);
    free(original_sound);

    fclose(input);
    fclose(output);
    return 0;
}