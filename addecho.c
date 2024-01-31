#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for optarg to work

int main(int argc, char **argv) {

    // Get delay and volume_scale and set default values if not provided
    int op;
    int delay = 8000;
    int volume_scale = 4;
    extern int optarg;  // this may cause issues!
    char *end;

    while ((op = getopt(argc, argv, "d:v:")) != -1) {
        switch(op)
        {
        case 'd':
            delay = strtol(optarg, &end, 10);

        case 'v':
            volume_scale = strtol(optarg, &end, 10);
        }
    }

    // Get the names of the files and store them in variables
    char source_name[strlen(argv[3])];
    char dest_name[strlen(argv[4])];

}
