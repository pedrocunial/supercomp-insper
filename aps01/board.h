#ifndef __BOARD_H__
#define __BOARD_H__

#define BUF              256
#define DEFAULT_DT       1.0
#define DEFAULT_EPS      0.000001
#define DEFAULT_VIZ_ITER 1000
#define DEFAULT_MAX_ITER 1000000000000
#define ISOLATED_TEMP    -274
#define itomi(R, C, N)   ((R) * (N) + (C))  // indexes to matrix index

#include <stdio.h>   // FILE struct
#include <omp.h>

typedef struct _board_t {
    int nx, ny;
    double len_step, f0, h, w, alpha;
    double *board;
} board_t;

double min(double *arr, int len);
double max(double *arr, int len);
double parallel_avg(double *arr, int len);
double avg(double *arr, int len);

void save_to_pgm(board_t *board, const char *str);
void generate_board(board_t *board, FILE *file);
void free_board(board_t *board);
void print_board(board_t *board);
int calculate_borders(board_t *board, double *new_board, double *diffs);
int sequencial_calculate_borders(board_t *board, double *new_board, double *diffs);
double update_temp(board_t *board, double *diffs);
double sequencial_update(board_t *board, double *diffs);
board_t * init_board(char *file_name);

#endif
