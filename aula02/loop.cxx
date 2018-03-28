#define SIZE 12

long sum(int v[SIZE]) throw() {
  long s = 0;
  for (unsigned i = 0; i < SIZE; i++)
    s += v[i];
  return s;
}

void div82(float vec[8]) {
  for (unsigned i=0; i<8; i++)
    vec[i] /= 2;
}
