#include <stdio.h>
#include <stdlib.h>
#define ELEM_TYPE float


typedef struct matrix_t {
  int height;
  int width;
  ELEM_TYPE * values;
} matrix_t;

matrix_t matrix(int height, int width) {
  matrix_t result;
  result.height = height;
  result.width = width;
  result.values = (ELEM_TYPE *) malloc(height * width * sizeof(ELEM_TYPE));
  return result;
}

void set(matrix_t * mat, int i, int j, ELEM_TYPE elem) {
  if (i * (mat->width) + j >= (mat->width) * (mat->height)) {
  }
  *(mat->values + i * (mat->width) + j) = elem; /* what should be a dereferenced pointer to the correct location in memory */
}

ELEM_TYPE get(matrix_t mat, int i, int j) {
  if (i * mat.width + j >= mat.width * mat.height) {

  }
  return *(mat.values + i * (mat.width) + j);
}

void printm(matrix_t mat) {
  for (int i = 0; i < mat.height; i++) {
    putchar('<');
    putchar(' ');
    for (int j = 0; j < mat.width; j++) {
      printf("%.8f ", get(mat, i, j));
    }
    putchar('>');
    putchar('\n');
  }
}

matrix_t madd(matrix_t m1, matrix_t m2) {
  if (m1.height != m2.height || m1.width != m2.width) {
    printf("ERROR: INCONSISTENT MATRIX SIZES FOR ADDITION\n");
    return matrix(0, 0);
  }
  matrix_t result = matrix(m1.height, m1.width);
  int i, j;
  for (i = 0; i < m1.height; i++) {
    for (j = 0; j < m1.width; j++) {
      set(&result, i, j, get(m1, i, j) + get(m2, i, j));
    }
  }
  return result;
}

matrix_t msub(matrix_t m1, matrix_t m2) {
  if (m1.height != m2.height || m1.width != m2.width) {
    printf("ERROR: INCONSISTENT MATRIX SIZES (%d,%d) x (%d,%d) FOR SUBTRACTION\n", m1.height, m1.width, m2.height, m2.width);
    return matrix(0, 0);
  }
  matrix_t result = matrix(m1.height, m2.width);
  int i, j;
  for (i =0; i < m1.height; i++) {
    for (j = 0; j < m1.width; j++) {
      set(&result, i, j, get(m1, i, j) - get(m2, i, j));
    }
  }
  return result;
}

matrix_t mmult(matrix_t m1, matrix_t m2) {
  matrix_t result = matrix(m1.height, m2.width);
  int i, j, k;
  ELEM_TYPE sum;
  for (i = 0; i < result.height; i++) {
    for (j = 0; j < result.width; j++) {
      sum = 0;
      for (k = 0; k < m1.width; k++) {
	sum = sum + get(m1, i, k) * get(m2, k, j);
      }
      set(&result, i, j, sum);
    }
  }
  return result;
}

matrix_t apply_func(matrix_t mat, float (*foo)(float)) {
  int i, j;
  for (i = 0; i < mat.height; i++) {
    for (j = 0; j < mat.width;  j++) {
      set(&mat, i, j, (*foo)(get(mat, i, j)));
    }
  }
  return mat;
}
