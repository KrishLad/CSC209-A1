#include <stdio.h>
#define HEADER_SIZE 44


int main(int argc, char **argv){
    char *input_name, *output_name;
    FILE *sourcewav, *destwav;
    short header[HEADER_SIZE];
    int error;
    short samples[2];
    short combined;

    // === Opening of files === 
    if (argc != 3){ //check for arguments.
        fprintf(stderr, "Usage: %s sourcewav destwav\n", argv[0]);
        return 1; //provide appropriate erro if they do not have sufficent args.
    }

    input_name = argv[1];
    output_name = argv[2];

    //Open input source for reading
    sourcewav = fopen(input_name, "rb"); //opens the file in binary mode.
    //Check for bad file 
    if (sourcewav == NULL){
        fprintf(stderr, "Error: cannot open sourcewav\n");
        return 1;   
    }
    //Open output source for writing 
    destwav = fopen(output_name,"wb");
    //Check for bad file
    if (destwav == NULL){
        fprintf(stderr, "Error: cannot open deswav\n");
        return 1;
    }


    // ==== Remove Vocals Algo ====
    // Step 1: Copy over header from sourcewav
    fread(header, HEADER_SIZE, 1, sourcewav);
    error = fwrite(header, HEADER_SIZE, 1, destwav);
    // if error == 1 then we have succesfully copied it over.
    if (error != 1){ 
        fprintf(stderr, "Error: could not write a full audio header\n");
        return 1;
    }
    //Step 2: Rest are 2 byte shorts, take each pair and get the difference over 2
    while(fread(samples, sizeof(short), 2, sourcewav) == 2){
        combined = (samples[0] - samples[1])/2;
        error = fwrite(&combined,sizeof(short),1,destwav); //ask Rutwa for a better way.
        if (error != 1){
            fprintf(stderr, "Error: could not write a left sample\n");
            return 1;
        }
        //write it twice.
        error = fwrite(&combined, sizeof(short), 1, destwav);
        if (error != 1)
        {
            fprintf(stderr, "Error: could not write a left sample\n");
            return 1;
        }
    }
    // === Closing the files === 
    error = fclose(sourcewav);
    if (error != 0){
        fprintf(stderr, "Error: cannot close sourcewav\n");
        return 1;
    }
    error = fclose(destwav);
    if (error != 0)
    {
        fprintf(stderr, "Error: cannot close destwav\n");
        return 1;
    }
    
    return 0;
}