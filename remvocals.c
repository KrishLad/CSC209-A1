#include <stdio.h>
#include <stdlib.h>

// first 44 bytes need to be copied into the other file

int main(int argc, char **argv) {
    
    // Get the names of the files and store them in variables
    char source_name[strlen(argv[1])];
    char dest_name[strlen(argv[2])];

    // Open files and copy first 44 bytes
    FILE *source_file;
    FILE *dest_file;

    source_file = fopen(source_name, "rb");
    dest_file = fopen(dest_name, "wb");

    // Declaration for the pointer
    int read_value = 44;

    void *to_read = &read_value;
    size_t to_write = fread(to_read, 1, 44, source_file);
    
    // Write to dest_file
    fwrite(&to_write, 1, 44, source_file);
    
    fclose(source_file);
    fclose(dest_file);
}
