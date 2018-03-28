#include <stdio.h>
#include "board.h"

#define DEFAULT_FNAME "exemplo1"

int main(int argc, char *argv[])
{
    char *fname;
    if (argc == 2) {
        fname = argv[1];
    } else if (argc == 1) {
        fname = DEFAULT_FNAME;
    } else {
        puts("Too many arguments, terminating");
        return 2;
    }

    board_t *board = init_board(fname);
    return 0;
}
