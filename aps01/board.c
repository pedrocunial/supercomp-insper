#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "board.h"


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
  delta_x = (double) w / ((double) nx);
  delta_y = (double) h / ((double) ny);

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

  if (board->f0 > 0.30) {
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

double update_temp(board_t *board)
{
  double diff, curr_err;
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  double f0 = board->f0;
  double *old_board = board->board;
  double *new_board = (double *) calloc(sizeof(double), xmax * ymax);
  curr_err = calculate_borders(board, new_board);

  for (int i=2; i<ymax-2; i++) {
    for (int j=2; j<xmax-2; j++) {
      new_board[itomi(i, j, xmax)] = upd_tmp(old_board, i, j, xmax, f0);
      diff = fabs(old_board[itomi(i, j, xmax)] - new_board[itomi(i, j, xmax)]);
      if (diff < curr_err && diff != 0) curr_err = diff;
    }
  }

  free(old_board);
  board->board = new_board;
  return curr_err;
}

double calculate_borders(board_t *board, double *new_board)
{
  // never calculate corners
  double diff;
  int xmax = board->nx + 2;
  int ymax = board->ny + 2;
  double *old_board = board->board;
  double f0 = board->f0;
  double curr_err = DBL_MAX;
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
  for (int i=1; i<xmax-1; i++) {
    if (old_board[i] != ISOLATED_TEMP) {
      new_board[itomi(1, i, xmax)] = f0 * (2 * old_board[itomi(2, i, xmax)]
                                           + old_board[itomi(1, i+1, xmax)]
                                           + old_board[itomi(1, i-1, xmax)]
                                           - 4 * old_board[itomi(1, i, xmax)])
                                      + old_board[itomi(1, i, xmax)];
      diff = fabs(old_board[itomi(1, i, xmax)] - new_board[itomi(1, i, xmax)]);
      if (diff < curr_err) curr_err = diff;
    } else {
      new_board[itomi(1, i, xmax)] = old_board[itomi(1, i, xmax)];
    }
    if (old_board[itomi(ymax-1, i, xmax)] != ISOLATED_TEMP) {
      new_board[itomi(ymax-2, i, xmax)] = f0 * (2 * old_board[itomi(ymax-3, i, xmax)]
                                                + old_board[itomi(ymax-2, i+1, xmax)]
                                                + old_board[itomi(ymax-2, i-1, xmax)]
                                                - 4 * old_board[itomi(ymax-2, i, xmax)])
                                          + old_board[itomi(ymax-2, i, xmax)];
      diff = fabs(old_board[itomi(ymax-2, i, xmax)] - new_board[itomi(ymax-2, i, xmax)]);
      if (diff < curr_err) curr_err = diff;
    } else {
      new_board[itomi(ymax-1, i, xmax)] = old_board[itomi(ymax-1, i, xmax)];
    }
  }
  // left and right
  for (int i=1; i<ymax-1; i++) {
    if (old_board[itomi(i, 0, xmax)] != ISOLATED_TEMP) {
      new_board[itomi(i, 0, xmax)] = f0 * (2 * old_board[itomi(i, 1, xmax)]
                                           + old_board[itomi(i+1, 0, xmax)]
                                           + old_board[itomi(i-1, 0, xmax)]
                                           - 4 * old_board[itomi(i, 0, xmax)])
                                        + old_board[itomi(i, 0, xmax)];
      diff = fabs(old_board[itomi(i, 0, xmax)] - new_board[itomi(i, 0, xmax)]);
      if (diff < curr_err && diff != 0) curr_err = diff;
    } else {
      new_board[itomi(i, 0, xmax)] = old_board[itomi(i, 0, xmax)];
    }
    if (old_board[itomi(i, xmax-1, xmax)] != ISOLATED_TEMP) {
      new_board[itomi(i, xmax-1, xmax)] = f0 * (2 * old_board[itomi(i, xmax-2, xmax)]
                                           + old_board[itomi(i+1, xmax-1, xmax)]
                                           + old_board[itomi(i-1, xmax-1, xmax)]
                                           - 4 * old_board[itomi(i, xmax-1, xmax)])
                                        + old_board[itomi(i, xmax-1, xmax)];
      diff = fabs(old_board[itomi(i, xmax-1, xmax)] - new_board[itomi(i, xmax-1, xmax)]);
      if (diff < curr_err && diff != 0) curr_err = diff;
    } else {
      new_board[itomi(i, xmax-1, xmax)] = old_board[itomi(i, xmax-1, xmax)];
    }
  }
  return diff;
}
