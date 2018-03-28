#include <stdio.h>
#include <stdlib.h>
#include "board.h"


board_t * init_board(char *file_name)
{
    int ny, nx;
    double h, w, alpha, delta_x, delta_t;
    FILE *file;
    board_t *board = (board_t *) calloc(sizeof(board_t), 1);

    file = fopen(file_name, "r");
    fscanf(file, "%lf %lf %d %d", &h, &w, &ny, &nx);
    fscanf(file, "%lf", &alpha);
    fclose(file);

    char *str_dt = getenv("DELTA_T");
    if (str_dt == NULL) {
        delta_t = (double) DEFAULT_DT;
    } else {
        sscanf(str_dt, "%lf", &delta_t);
    }
    delta_x = (double) w / ((double) nx);

    board->h = h;
    board->w = w;
    board->ny = ny;
    board->nx = nx;
    board->alpha = alpha;
    board->f0 = alpha * delta_t / (delta_x * delta_x);
    board->board = (double *) calloc(sizeof(double), h * w);

    printf("%lf %lf %d %d\n", h, w, ny, nx);
    printf("%lf\n", alpha);
    printf("%lf\n", delta_t);
    return board;
}
