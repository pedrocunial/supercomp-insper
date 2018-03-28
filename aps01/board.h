#ifndef __BOARD_H__
#define __BOARD_H__

#define DEFAULT_DT    1.0
#define ISOLATED_TEMP 274

typedef struct _board_t {
    int nx, ny;
    double len_step, f0, h, w, alpha;
    double *board;
} board_t;

void update_temp(board_t *board);
board_t * init_board(char *file_name);

#endif
