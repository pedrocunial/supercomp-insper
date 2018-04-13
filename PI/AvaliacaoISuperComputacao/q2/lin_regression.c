#include <stdlib.h>

double *transpose(double *__X, int rows, int cols) {
  double (*T)[rows] = malloc(sizeof(double) * rows * cols);
  double (*X)[cols] = (double(*)[cols]) __X;

  // caso bem simples de paralelizacao, apresentando ganhos já mesmo em casos com outras
  // paralelizacoes (ganhando cerca de 10% de desempenho no q3 com epochs=2000 e já com
  // o #compute_XTX paralelizado)
#pragma omp parallel for collapse(2)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      T[j][i] = X[i][j];
    }
  }
  return (double *) T;
}

void compute_XTX(double *__XT, int rows, int cols, double *__XTX) {
  double (*XT)[rows] = (double(*)[rows]) __XT;
  double (*XTX)[cols] = (double(*)[cols]) __XTX;


  // utilizei apenas um collapse(2) pois a operação do 3o loop é crítica
  // só com este pragma já consegui um ganho de 30% no exemplo 2
#pragma omp parallel for collapse(2) shared(cols, rows, XT, XTX)
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < cols; j++) {
      XTX[i][j] = 0.0;
      // Este pragma não me entregou nenhum ganho muito substancial (apenas .1 segundo)
      // em média no exemplo 2, mas não atrapalhou o desempenho em outros casos, então
      // o mantive
#pragma omp parallel for reduction(+: XTX[i][j])
      for (int k = 0; k < rows; k++) {
        XTX[i][j] += XT[i][k] * XT[j][k];
      }
    }
  }
}

void matriz_vector_multiply(double *__m1, double *v1, int rows, int cols, double *res) {
  double (*m1)[cols] = (double(*)[cols]) __m1;

  // a otimizacao da parte externa deste loop é bem trivial, uma vez que o loop nao faz
  // nada além de zerar o valor de res[i] (que só é acessado em uma iteracao do loop) e
  // por tanto nao exige nenhuma forma de "protecao" (como critical ou atomic)
#pragma omp parallel for shared(cols, res)
  for (int i = 0; i < rows; i++) {
    res[i] = 0;
    // o pragma no loop interno nao pareceu util em meus testes, apresentando uma perda
    // consideravel de desempenho  nos exemplos menores e um ganho quase disprezivel
    // nos maiores
/* #pragma omp parallel for shared(res, m1, v1, i) */
    for (int j = 0; j < cols; j++) {
      res[i] += m1[i][j] * v1[j];
    }
  }
}

/* v2 -= v1 */

void vector_inplace_sub(double *v1, double *v2, int n) {
  // como a subtração ocorre a cada iteração do loop em cada membro individual do vetor,
  // nao é necessario o uso de uma flag de reduction (apenas a parelização do for já
  // deveria ser o suficiente).
  //
  // no caso do exemplo 2, esta paralelizacao resultou em uma perda de desempenho de cerca
  // de 30%, ou seja, nao vale a pena! até mesmo em casos maiores (exmeplo3) ele
  // apresentou uma perda razoável de desempenho, provando que realmente nao valeria a pena
/* #pragma omp parallel for */
  for (int i = 0; i < n; i++) {
    v2[i] -= v1[i];
  }
}

void vector_inplace_square(double *v1, int n) {
  // mesmo caso da anterior
/* #pragma omp parallel for */
  for (int i = 0; i < n; i++) {
    v1[i] *= v1[i];
  }
}

double vector_sum(double *v, int n) {
  double sum = 0;
  // apesar da perda de desempenho para os exemplos 1 e 2, esta reduction apresentou um
  // já no caso do exemplo 3, de forma que parece ser mais útil na maioria dos casos e,
  // portanto, uma adicao valida
#pragma omp parallel for reduction(+: sum)
  for (int i = 0; i < n; i++) {
    sum += v[i];
  }
  return sum;
}

void vector_inplace_multiply_scalar(double *v, int n, double s) {
  // mesmo caso do #vector_inplace_sub
/* #pragma omp parallel for */
  for (int i = 0; i < n; i++) {
    v[i] *= s;
  }
}


double cost_function(double *X, double *y, double *w, int rows, int cols) {
  static double *temp1 = NULL;
  if (temp1 == NULL) {
    temp1 = malloc(sizeof(double) * rows);
  }
  matriz_vector_multiply(X, w, rows, cols, temp1);
  vector_inplace_sub(y, temp1, rows);
  vector_inplace_square(temp1, rows);
  return vector_sum(temp1, rows);
}

void update_weights(double *X, double *XT, double *y, double *w, double learning_rate, int rows, int cols) {
  static double *temp1 = NULL, *temp2 = NULL, *temp3 = NULL;
  if (temp1 == NULL) {
    temp1 = malloc(sizeof(double) * cols * cols); /* X^T X */
    temp2 = malloc(sizeof(double) * cols); /* X^T X w */
    temp3 = malloc(sizeof(double) * cols); /* X^T y */
  }
  compute_XTX(XT, rows, cols, temp1);

  // como as operacoes de multiplicacao de vetor-matriz sao independentes entre si,
  // podemos realizar cada uma delas em uma task separada, resultando em um ganho de cerca
  // de 5% para exemplos médio-grandes (exemplo3 com epochs=2500)
#pragma omp parallel
  {
#pragma omp single
    {
#pragma omp task
      matriz_vector_multiply(temp1, w, cols, cols, temp2);
#pragma omp task
      matriz_vector_multiply(XT, y, cols, rows, temp3);
    }
  }
  vector_inplace_sub(temp3, temp2 ,cols);
  vector_inplace_multiply_scalar(temp2, cols, 2.0*learning_rate/rows);
  vector_inplace_sub(temp2, w, cols);
}
