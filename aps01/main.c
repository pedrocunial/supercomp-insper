#include <stdio.h>
#include <stdlib.h>
#include "board.h"

#define EPS           "EPS"
#define MAX_ITER      "MAX_ITER"
#define DEFAULT_FNAME "exemplo1"

int main(int argc, char *argv[])
{
  double eps;
  size_t max_iter;
  char *str_eps, *str_max_iter, *fname;

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

  board_t *board = init_board(fname);
  if (board == NULL) {
    return 1;
  }

  double err = eps + 1;
  puts("calculating...");
  print_board(board);
  for (size_t i=0; i<max_iter && err > eps; i++) {
    err = update_temp(board);
    print_board(board);
  }
  return 0;
}
