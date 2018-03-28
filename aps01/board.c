#include <stdlib.h>
#include "board.h"


board_t * init_board(char *file_name)
{
    int ny, nx;
    double h, w, alpha, delta_x, delta_t;
    char **str_board;
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
    board->f0 = alpha * delta_t / (delta_x * delta_x);
    board->alpha = alpha;
    board->board = (double *) calloc(sizeof(double), h * w);
    generate_board(board->board, str_board);

    printf("%lf %lf %d %d\n", h, w, ny, nx);
    printf("%lf\n", alpha);
    printf("%lf\n", delta_t);
    return board;
}

void generate_board(board_t *board, char **b)
{

}

void update_temp(board_t *board) { }
