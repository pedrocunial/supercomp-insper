#ifndef __BOARD_H__
#define __BOARD_H__

#define BUF              256
#define DEFAULT_DT       1.0
#define DEFAULT_EPS      0.000001
#define DEFAULT_MAX_ITER 1000000000000
#define ISOLATED_TEMP    -274
#define itomi(R, C, N)   (R * N + C)  // indexes to matrix index
#define upd_tmp(board, i, j, xmax, f0) (board[itomi(i, j, xmax)] + f0 * \
                                        (board[itomi(i-1, j, xmax)] + \
                                         board[itomi(i+1, j, xmax)] + \
                                         board[itomi(i, j-1, xmax)] + \
                                         board[itomi(i, j+1, xmax)] + \
                                         4 * board[itomi(i, j, xmax)]))

#include <stdio.h>

typedef struct _board_t {
    int nx, ny;
    double len_step, f0, h, w, alpha;
    double *board;
} board_t;

void generate_board(board_t *board, FILE *file);
void free_board(board_t *board);
void print_board(board_t *board);
double calculate_borders(board_t *board, double *new_board);
double update_temp(board_t *board);
board_t * init_board(char *file_name);

#endif
