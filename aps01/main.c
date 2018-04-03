#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>
#include "board.h"

#define EPS           "EPS"
#define MAX_ITER      "MAX_ITER"
#define VIZ_ITER      "VIZ_ITER"
#define DEFAULT_FNAME "exemplo1"

const char *SEQ = "SEQUENCIAL";
const char *PAR = "PARALLEL";

int run(char *fname, double eps, size_t max_iter, size_t viz_iter)
{
  // sequencial
  board_t *board = init_board(fname);
  if (board == NULL) {
    return 1;
  }

  double err = eps + 1;
  double board_size = board->nx * board->ny;
  double *diffs = (double *) calloc(sizeof(double), board_size);
  /* print_board(board); */
  double beg_time, end_time;
  // parallel
  beg_time = omp_get_wtime();
  for (size_t i=0, j=0; i<max_iter && err > eps; i++, j++) {
    err = update_temp(board, diffs);
    /* print_board(board); */
    if (j == viz_iter) {
      // THIS IS REALLY SLOW, I HAVE NO IDEA HOW
      // ELSE I SHOULD DO IT
      /* save_to_pgm(board, PAR); */
      j = 0;
    }
  }
  end_time = omp_get_wtime();
  printf("Parallel calculation took %lf seconds\n", end_time - beg_time);
  free_board(board);

  board = init_board(fname);
  if (board == NULL) {
    return 1;
  }
  err = eps + 1;

  beg_time = omp_get_wtime();
  for (size_t i=0, j=0; i<max_iter && err > eps; i++, j++) {
    err = sequencial_update(board, diffs);
    /* print_board(board); */
    if (j == viz_iter) {
      // THIS IS REALLY SLOW, I HAVE NO IDEA HOW
      // ELSE I SHOULD DO IT
      /* save_to_pgm(board, SEQ); */
      j = 0;
    }
  }
  end_time = omp_get_wtime();
  printf("Sequencial calculation took %lf seconds\n", end_time - beg_time);
  free_board(board);

  return 0;
}

int main(int argc, char *argv[])
{
  double eps;
  size_t max_iter, viz_iter;
  char *str_eps, *str_max_iter, *fname, *str_viz_iter;

  if (argc == 2) {
    fname = argv[1];
  } else if (argc == 1) {
    fname = DEFAULT_FNAME;
  } else {
    puts("Too many arguments, terminating");
    return 2;
  }

  str_eps = getenv(EPS);
  if (str_eps == NULL) {
    eps = DEFAULT_EPS;
  } else {
    sscanf(str_eps, "%lf", &eps);
  }

  str_max_iter = getenv(MAX_ITER);
  if (str_max_iter == NULL) {
    max_iter = DEFAULT_MAX_ITER;
  } else {
    sscanf(str_max_iter, "%ld", &max_iter);
  }

  str_viz_iter = getenv(VIZ_ITER);
  if (str_viz_iter == NULL) {
    viz_iter = DEFAULT_VIZ_ITER;
  } else {
    sscanf(str_viz_iter, "%ld", &viz_iter);
  }

  return run(fname, eps, max_iter, viz_iter);;
}
