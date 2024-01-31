#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //for optarg to work
#include "addecho.h"

/**
Returns delay and volume according to input
*/
void *parse_input(int argc, char **argv, int *delay, int *volume) {

    int op;
    char *end;

    while ((op = getopt(argc, argv, "d:v:")) != -1) {
        if (op == 100) { // character is d
            *delay = strtol(optarg, &end, 10);
        } else if (op == 118) { // character is v
            *volume = strtol(optarg, &end, 10);
        }
    }
}

int main(int argc, char **argv) {

    int delay = 8000;
    int volume = 4;
    parse_input(argc, argv, &delay, &volume);

    return 0;

    // TAKE FIRST 44 BYTES AND EDIT FILE SIZE

}
