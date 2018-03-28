#ifndef __BOARD_H__
#define __BOARD_H__

#define DEFAULT_DT    1.0
#define ISOLATED_TEMP 274
#define matrix_index(R, C, N) (R * N + C)

#include <stdio.h>

typedef struct _board_t {
    int nx, ny;
    double len_step, f0, h, w, alpha;
    double *board;
} board_t;

void update_temp(board_t *board);
void generate_board(board_t *board, char **b);
board_t * init_board(char *file_name);

#endif
