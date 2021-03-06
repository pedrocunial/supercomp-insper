#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "board.h"

extern inline double upd_tmp(double *board, int i, int j, int xmax, double f0)
{
  return board[itomi(i, j, xmax)] + f0 * (board[itomi(i-1, j, xmax)] +
                                          board[itomi(i+1, j, xmax)] +
                                          board[itomi(i, j-1, xmax)] +
                                          board[itomi(i, j+1, xmax)] -
                                          4 * board[itomi(i, j, xmax)]);
}

extern inline void reinit_diffs(double *diffs, int len)
{
  for (int i=0; i<len; i++) diffs[i] = 0;
}

board_t * init_board(char *file_name)
{
  int ny, nx;
  double h, w, alpha, delta_x, delta_t, delta_y;
  /* char **str_board; */
  FILE *file;
  board_t *board = (board_t *) calloc(sizeof(board_t), 1);

  file = fopen(file_name, "r");
  fscanf(file, "%lf %lf %d %d", &h, &w, &ny, &nx);
  fscanf(file, "%lf", &alpha);

  char *str_dt = getenv("DELTA_T");
  if (str_dt == NULL) {
    delta_t = (double) DEFAULT_DT;
  } else {
    sscanf(str_dt, "%lf", &delta_t);
  }
  delta_x = (double) w / ((double) nx-1);
  delta_y = (double) h / ((double) ny-1);

  if (delta_y != delta_x) {
    puts("delta_x != delta_y, terminando o programa");
    free(board);
    return NULL;
  }

  board->h = h;
  board->w = w;
  board->ny = ny;
  board->nx = nx;
  board->f0 = alpha * delta_t / (delta_x * delta_x);
  board->alpha = alpha;
  board->board = (double *) calloc(sizeof(double), (ny+2) * (nx+2));
  generate_board(board, file);

  fclose(file);

  if (board->f0 > 0.25) {
    printf("F0 grande demais (%lf), terminando o programa\n", board->f0);
    free_board(board);
    return NULL;
  }

  return board;
}

void free_board(board_t *board)
{
  free(board->board);
  free(board);
}

void generate_board(board_t *board, FILE *file)
{
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  double *b = board->board;
  for (int i=0; i<ymax; i++) {
    for (int j=0; j<xmax; j++) {
      char str[BUF];
      double temp;
      fscanf(file, "%s", str);
      temp = (double) ((str[0] == '#') ? ISOLATED_TEMP : atof(str));
      b[itomi(i, j, xmax)] = temp;
    }
  }
}

void print_board(board_t *board)
{
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  double *b = board->board;
  for (int i=0; i<ymax; i++) {
    for (int j=0; j<xmax; j++) {
      printf("%lf ", b[itomi(i, j, xmax)]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_array(double *arr, int len)
{
  for (int i=0; i<len; i++) {
    printf("%lf ", arr[i]);
  }
  printf("\n");
}

void save_to_pgm(board_t *board, const char *str)
{
  static size_t c = 0;
  char file_name[BUF];
  sprintf(file_name, "%s_out%ld.pgm", str, c);
  c++;
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  int len = xmax * ymax;
  double *b = board->board;
  double min_val = min(b, len);
  double max_val = max(b, len);
  int max_repr = (int) (max_val - min_val);
  double multiplier = MAX_PGM / max_repr;
  if (max_repr <= 0) return;

  FILE *file = fopen(file_name, "w");
  fprintf(file, "P2\n");
  fprintf(file, "%d %d\n", xmax, ymax);
  fprintf(file, "%d", MAX_PGM);

  for (int i=0; i<len; i++) {
    if (i % xmax == 0) fprintf(file, "\n");
    fprintf(file, "%d ", (b[i] == ISOLATED_TEMP) ? 0 : (int) ((b[i]-min_val)*multiplier));
  }

  fclose(file);
}

double max(double *arr, int len)
{
  double m = DBL_MIN;
  for (int i=0; i<len; i++) {
    if (arr[i] > m && arr[i] > ISOLATED_TEMP) m = arr[i];
  }
  return m;
}

double min(double *arr, int len)
{
  double m = DBL_MAX;
  for (int i=0; i<len; i++) {
    if (arr[i] < m && arr[i] > ISOLATED_TEMP) m = arr[i];
  }
  return m;
}

double parallel_avg(double *arr, int len)
{
  double sum = 0;
#pragma omp parallel for reduction(+:sum)
  for (int i=0; i<len; i++) {
    sum += arr[i];
  }
  return sum / len;
}

double avg(double *arr, int len)
{
  double sum = 0;
  for (int i=0; i<len; i++) {
    sum += arr[i];
  }
  return sum / len;
}

double sequencial_update(board_t *board, double *diffs)
{
  double curr_err;
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  int board_size = (xmax - 2) * (ymax - 2);
  double f0 = board->f0;
  double *new_board = (double *) calloc(xmax*ymax, sizeof(double));
  double *old_board = board->board;
  int idx = sequencial_calculate_borders(board, new_board, diffs);
  for (int i=2; i<ymax-2; i++) {
    for (int j=2; j<xmax-2; j++) {
      new_board[itomi(i, j, xmax)] = upd_tmp(old_board, i, j, xmax, f0);
      diffs[idx++] = fabs(old_board[itomi(i, j, xmax)] - new_board[itomi(i, j, xmax)]);
    }
  }

  board->board = new_board;
  free(old_board);
  curr_err = avg(diffs, board_size);
  return curr_err;
}


double update_temp(board_t *board, double *diffs)
{
  double curr_err;
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  int board_size = (xmax - 2) * (ymax - 2);
  double f0 = board->f0;
  double *new_board = (double *) calloc(xmax*ymax, sizeof(double));
  double *old_board = board->board;
  int idx = calculate_borders(board, new_board, diffs);
#pragma omp prallel for collapse(2) reduction(+:idx)
  for (int i=2; i<ymax-2; i++) {
    for (int j=2; j<xmax-2; j++) {
      new_board[itomi(i, j, xmax)] = upd_tmp(old_board, i, j, xmax, f0);
      diffs[idx++] = fabs(old_board[itomi(i, j, xmax)] - new_board[itomi(i, j, xmax)]);
    }
  }

  board->board = new_board;
  free(old_board);
  curr_err = parallel_avg(diffs, board_size);
  return curr_err;
}

int sequencial_calculate_borders(board_t *board, double *new_board, double *diffs)
{
  // never calculate corners
  int idx = 0;   // diffs array index
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  double *old_board = board->board;
  double f0 = board->f0;
  // keep outside values (expanded loops for cache optimization)
  for (int i=0; i<xmax; i++) {
    new_board[i] = old_board[i];
  }
  for (int i=0; i<xmax; i++) {
    new_board[itomi(ymax-1, i, xmax)] = old_board[itomi(ymax-1, i, xmax)];
  }
  for (int i=1; i<ymax-1; i++) {
    new_board[itomi(i, 0, xmax)] = old_board[itomi(i, 0, xmax)];
    new_board[itomi(i, xmax-1, xmax)] = old_board[itomi(i, xmax-1, xmax)];
  }
  // top and bottom
  for (int i=2; i<xmax-2; i++) {
    // top
    if (old_board[i] == ISOLATED_TEMP) {
      new_board[itomi(1, i, xmax)] = f0 * (2 * old_board[itomi(2, i, xmax)]
                                           + old_board[itomi(1, i+1, xmax)]
                                           + old_board[itomi(1, i-1, xmax)]
                                           - 4 * old_board[itomi(1, i, xmax)])
                                      + old_board[itomi(1, i, xmax)];
    } else {
      new_board[itomi(1, i, xmax)] = upd_tmp(old_board, 1, i, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(1, i, xmax)] - new_board[itomi(1, i, xmax)]);

    // bottom
    if (old_board[itomi(ymax-1, i, xmax)] == ISOLATED_TEMP) {
      new_board[itomi(ymax-2, i, xmax)] = f0 * (2 * old_board[itomi(ymax-3, i, xmax)]
                                                + old_board[itomi(ymax-2, i+1, xmax)]
                                                + old_board[itomi(ymax-2, i-1, xmax)]
                                                - 4 * old_board[itomi(ymax-2, i, xmax)])
                                          + old_board[itomi(ymax-2, i, xmax)];
    } else {
      new_board[itomi(ymax-2, i, xmax)] = upd_tmp(old_board, ymax-2, i, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(ymax-2, i, xmax)] -
                        new_board[itomi(ymax-2, i, xmax)]);
  }
  // left and right
  for (int i=2; i<ymax-2; i++) {
    // left
    if (old_board[itomi(i, 0, xmax)] == ISOLATED_TEMP) {
      new_board[itomi(i, 1, xmax)] = f0 * (2 * old_board[itomi(i, 2, xmax)]
                                           + old_board[itomi(i+1, 1, xmax)]
                                           + old_board[itomi(i-1, 1, xmax)]
                                           - 4 * old_board[itomi(i, 1, xmax)])
                                        + old_board[itomi(i, 1, xmax)];
    } else {
      new_board[itomi(i, 1, xmax)] = upd_tmp(old_board, i, 1, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(i, 1, xmax)] - new_board[itomi(i, 1, xmax)]);

    // right
    if (old_board[itomi(i, xmax-1, xmax)] == ISOLATED_TEMP) {
      new_board[itomi(i, xmax-2, xmax)] = f0 * (2 * old_board[itomi(i, xmax-3, xmax)]
                                           + old_board[itomi(i+1, xmax-2, xmax)]
                                           + old_board[itomi(i-1, xmax-2, xmax)]
                                           - 4 * old_board[itomi(i, xmax-2, xmax)])
                                        + old_board[itomi(i, xmax-2, xmax)];
    } else {
      new_board[itomi(i, xmax-2, xmax)] = upd_tmp(old_board, i, xmax-2, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(i, xmax-2, xmax)] -
                        new_board[itomi(i, xmax-2, xmax)]);
  }
  return idx;
}

int calculate_borders(board_t *board, double *new_board, double *diffs)
{
  // never calculate corners
  int idx = 0;   // diffs array index
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  double *old_board = board->board;
  double f0 = board->f0;
  // keep outside values (expanded loops for cache optimization)
  for (int i=0; i<xmax; i++) {
    new_board[i] = old_board[i];
  }
  for (int i=0; i<xmax; i++) {
    new_board[itomi(ymax-1, i, xmax)] = old_board[itomi(ymax-1, i, xmax)];
  }
  for (int i=1; i<ymax-1; i++) {
    new_board[itomi(i, 0, xmax)] = old_board[itomi(i, 0, xmax)];
    new_board[itomi(i, xmax-1, xmax)] = old_board[itomi(i, xmax-1, xmax)];
  }
  // top and bottom
#pragma omp parallel for reduction(+:idx)
  for (int i=2; i<xmax-2; i++) {
    // top
    if (old_board[i] == ISOLATED_TEMP) {
      new_board[itomi(1, i, xmax)] = f0 * (2 * old_board[itomi(2, i, xmax)]
                                           + old_board[itomi(1, i+1, xmax)]
                                           + old_board[itomi(1, i-1, xmax)]
                                           - 4 * old_board[itomi(1, i, xmax)])
                                      + old_board[itomi(1, i, xmax)];
    } else {
      new_board[itomi(1, i, xmax)] = upd_tmp(old_board, 1, i, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(1, i, xmax)] - new_board[itomi(1, i, xmax)]);

    // bottom
    if (old_board[itomi(ymax-1, i, xmax)] == ISOLATED_TEMP) {
      new_board[itomi(ymax-2, i, xmax)] = f0 * (2 * old_board[itomi(ymax-3, i, xmax)]
                                                + old_board[itomi(ymax-2, i+1, xmax)]
                                                + old_board[itomi(ymax-2, i-1, xmax)]
                                                - 4 * old_board[itomi(ymax-2, i, xmax)])
                                          + old_board[itomi(ymax-2, i, xmax)];
    } else {
      new_board[itomi(ymax-2, i, xmax)] = upd_tmp(old_board, ymax-2, i, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(ymax-2, i, xmax)] -
                        new_board[itomi(ymax-2, i, xmax)]);
  }
#pragma omp parallel for reduction(+:idx)
  // left and right
  for (int i=2; i<ymax-2; i++) {
    // left
    if (old_board[itomi(i, 0, xmax)] == ISOLATED_TEMP) {
      new_board[itomi(i, 1, xmax)] = f0 * (2 * old_board[itomi(i, 2, xmax)]
                                           + old_board[itomi(i+1, 1, xmax)]
                                           + old_board[itomi(i-1, 1, xmax)]
                                           - 4 * old_board[itomi(i, 1, xmax)])
                                        + old_board[itomi(i, 1, xmax)];
    } else {
      new_board[itomi(i, 1, xmax)] = upd_tmp(old_board, i, 1, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(i, 1, xmax)] - new_board[itomi(i, 1, xmax)]);

    // right
    if (old_board[itomi(i, xmax-1, xmax)] == ISOLATED_TEMP) {
      new_board[itomi(i, xmax-2, xmax)] = f0 * (2 * old_board[itomi(i, xmax-3, xmax)]
                                           + old_board[itomi(i+1, xmax-2, xmax)]
                                           + old_board[itomi(i-1, xmax-2, xmax)]
                                           - 4 * old_board[itomi(i, xmax-2, xmax)])
                                        + old_board[itomi(i, xmax-2, xmax)];
    } else {
      new_board[itomi(i, xmax-2, xmax)] = upd_tmp(old_board, i, xmax-2, xmax, f0);
    }
    diffs[idx++] = fabs(old_board[itomi(i, xmax-2, xmax)] -
                        new_board[itomi(i, xmax-2, xmax)]);
  }
  return idx;
}
