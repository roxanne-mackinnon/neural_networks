#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define ELEM_TYPE float


typedef struct matrix_t {
  int height;
  int width;
  ELEM_TYPE * values;
} matrix_t;

matrix_t matrix(int h, int w) {
  matrix_t result;
  result.height = h;
  result.width = w;
  result.values = (ELEM_TYPE *) malloc(h * w * sizeof(ELEM_TYPE));
  return result;
}

void set(matrix_t * mat, int i, int j, ELEM_TYPE elem) {
  assert(0 <= i && i < mat->height);
  assert(0 <= j && j < mat->width);
  *(mat->values + i * (mat->width) + j) = elem; /* what should be a dereferenced pointer to the correct location in memory */
}


ELEM_TYPE get(matrix_t * mat, int i, int j) {
  assert(0 <= i && i < mat->height);
  assert(0 <= j && j < mat->width);
  return *(mat->values + i * (mat->width) + j);
}

void printm(matrix_t * mat) {
  for (int i = 0; i < mat->height; i++) {
    putchar('<');
    putchar(' ');
    for (int j = 0; j < mat->width; j++) {
      printf("%.8f ", get(mat, i, j));
    }
    putchar('>');
    putchar('\n');
  }
}

matrix_t * madd(matrix_t * m1, matrix_t * m2) {
  assert(m1->height == m2->height);
  assert(m1->width == m2->width);
  matrix_t * result = (matrix_t *) malloc(1 * sizeof(matrix_t));
  *result = matrix(m1->height, m1->width);
  int i, j;
  for (i = 0; i < m1->height; i++) {
    for (j = 0; j < m1->width; j++) {
      set(result, i, j, get(m1, i, j) + get(m2, i, j));
    }
  }
  return result;
}

matrix_t * msub(matrix_t * m1, matrix_t * m2) {
  assert(m1->height == m2->height);
  assert(m1->width == m2->width);
  matrix_t * result = (matrix_t *) malloc(1 * sizeof(matrix_t));
  *result = matrix(m1->height, m1->width);
  int i, j;
  for (i = 0; i < m1->height; i++) {
    for (j = 0; j < m1->width; j++) {
      set(result, i, j, get(m1, i, j) - get(m2, i, j));
    }
  }
  return result;
}

matrix_t * mmult(matrix_t * m1, matrix_t * m2) {
  assert(m1->width == m2->height);
  matrix_t * result = (matrix_t *) malloc(1 * sizeof(matrix_t));
  *result = matrix(m1->height, m2->width);
  int i, j, k;
  ELEM_TYPE sum;
  for (i = 0; i < result->height; i++) {
    for (j = 0; j < result->width; j++) {
      sum = 0;
      for (k = 0; k < m1->width; k++) {
  	sum = sum + get(m1, i, k) * get(m2, k, j);
      }
      set(result, i, j, sum);
    }
  }
  return result;
}


matrix_t * apply_func(matrix_t * m1, float (*func)(float)) {
  matrix_t * result = (matrix_t *) malloc(1 * sizeof(matrix_t));
  *result = matrix(m1->height, m1->width);
  for (int i = 0; i < m1->height; i++) {
    for (int j = 0; j < m1->width; j++) {
      assert(0 <= i && i < m1->height);
      assert(0 <= j && j < m1->width);
      set(result, i, j, (*func)(get(m1, i, j)));
    }
  }
  return result;
}

