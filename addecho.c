#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //for optarg to work
#include <getopt.h>
#include <errno.h>
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
    {   errno = 0;
        // check for optarg
        if (op == D_CONST)
        { // character is d
            *delay = strtol(optarg, &end, 10);
            if (errno != 0) {
                printf("Invalid optional argument types");
            }
        }
        else if (op == V_CONST)
        { // character is v
            *volume = strtol(optarg, &end, 10);
            if (errno != 0) {
                printf("Invalid optional argument types");
            }
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
    error = fread(header, sizeof(short), HEADER_SIZE, input); //reads in the header.
    if (error  != HEADER_SIZE){
        fprintf(stderr, "There was an error in reading the input file\n");
        exit(1);
    }

    // == Editing the shorts at 20 and 2 ==
    sizeptr = (unsigned int *)(header + 2);
    *sizeptr += delay * 2;
    sizeptr = (unsigned int *)(header + 20);
    *sizeptr += delay * 2;

    // == Write to output file ==
    error = fwrite(header, sizeof(short), HEADER_SIZE, output);
    if (error != HEADER_SIZE){
        fprintf(stderr, "Could not write to file :1\n");
        exit(1);
    }

}

/*
    Count the number of samples, not including the header, in the file
*/
void count_samples(FILE *input, int *count) {
    fseek(input, 44, SEEK_SET);
    short sample;
    while (!feof(input)) {
        fread(&sample, sizeof(short), 1, input);
        if (!feof(input)) {
            *count += 1;
        }
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
    } else if (argc > 3 && (argc != 5 || argc != 7)) {
        fprintf(stderr, "Too few arguments, Usage: %s [-d delay] [-v volume_scale] sourcewav destwav\n", argv[0]);
        exit(1);
    } 
    else if (argc > 7) {
        fprintf(stderr, "Too many arguments, Usage: %s [-d delay] [-v volume_scale] sourcewav destwav\n", argv[0]);
        exit(1);
    }

    // getting values of delay and volume
    parse_input(argc, argv, &delay, &volume);

    // getting files for input and output
    char *inputTitle = argv[argc - 2];
    char *outputTitle = argv[argc - 1];
    //error checking
    if (inputTitle == outputTitle) {
        fprintf(stderr, "Input and output file must be different files.");
        exit(1);
    }

    FILE *input = fopen(inputTitle, "rb"); //source 
    if (input == NULL) {
        fprintf(stderr, "File not found\n");
        exit(1);
    }
    FILE *output = fopen(outputTitle, "wb"); // destination

    // === Editing Header ===

    edit_header(input, output, delay); 

    // === Algorithim === 
    // This is something for step 3, but must be done now to not mess with file writing
    int count = 0;
    count_samples(input, &count);   
    fseek(input, 44, SEEK_SET); // sets the pointer to be after header

    // Step 1: place <delay> amount of the original sound in a buffer

    short *echo_buffer = malloc(sizeof(short) * delay); // has the echo scaled by <volume>
    short  *original_sound = malloc(sizeof(short) * delay);//stores delay amount of the original sound.

    // Step 2a: If you are not at sample <delay> then copy over the sound of the original.

    int file_size;
    file_size = fread(original_sound, sizeof(short), delay, input);

    if (file_size < delay) { // the file is smaller than delay
        error = fwrite(original_sound, sizeof(short), file_size, output);
        if (error != file_size) {
            fprintf(stderr, "Could not write to the file\n");
            exit(1);
        }

        //scales everything in original sound and places it in an echo buffer
        for (int i = 0; i < file_size; i++) {
            echo_buffer[i] = original_sound[i] / volume;
        }
    } else {
        error = fwrite(original_sound, sizeof(short), delay, output);
        if (error != delay) {
            fprintf(stderr, "Could not write to the file\n");
            exit(1);
        }

        //scales everything in original sound and places it in an echo buffer
        for (int i = 0; i < delay; i++) {
            echo_buffer[i] = original_sound[i] / volume;
        }
    }

    // we are now at sample <delay> or the end of the file if file_size is smaller than delay

    // Step 2b: Start mixing in the buffer.

    int shorts_left;
    short sample;
    while( (shorts_left = fread(original_sound, sizeof(short), delay, input)) == delay){
        // Mix them together.
        for (int i =0; i < delay; i++) {
            //at this point original sound has been updates but echo buffer has not
            sample = original_sound[i] + echo_buffer[i];
            error = fwrite(&sample, sizeof(short), 1, output);    
            if (error != 1) {
                fprintf(stderr, "Could not write to file\n");
                exit(1);
            }
            //now updating the echo buffer
            echo_buffer[i] = original_sound[i] / volume;
        }
    }

    short *samples = malloc(shorts_left * sizeof(short));
    if (shorts_left > 0){ //if we have bytes remaing to read
        for(int i = 0; i < shorts_left; i++) { 
            samples[i] = original_sound[i] + echo_buffer[i];
        }
        fwrite(samples, sizeof(short), shorts_left, output);
    }

    //Step 3 : Write 0 samples and Step 4: Clear out the echo buffer samples         
    int x = delay - count;
    short ZERO = 0;
    if (x > 0){
        for (int i = count; i < delay; i++){
            fwrite(&ZERO, sizeof(short), 1, output);
        }
        fwrite(echo_buffer, sizeof(short), count, output);
    } else {
        for (int i = shorts_left; i < delay; i++) {
            fwrite(&echo_buffer[i], sizeof(short), 1, output);
        }
        for (int i = 0; i < shorts_left; i++) {
            echo_buffer[i] = original_sound[i] / volume;
            fwrite(&echo_buffer[i], sizeof(short), 1, output);
        }
    }

    // === Clean up === 
    free(echo_buffer);
    free(original_sound);
    free(samples);

    fclose(input);
    fclose(output);

    return 0;
}