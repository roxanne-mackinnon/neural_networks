#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#undef RAND_MAX 
#define RAND_MAX 100
#define ELEM_TYPE float

typedef struct matrix_t {
  int height;
  int width;
  ELEM_TYPE * values;
} matrix_t;

matrix_t * matrix(int h, int w) {
  assert(h > 0);
  assert(w > 0);
  matrix_t * result = (matrix_t *) malloc(sizeof(matrix_t));
  result->height = h;
  result->width = w;
  result->values = (ELEM_TYPE *) malloc(h * w * sizeof(ELEM_TYPE));
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

char is_equal(matrix_t * m1, matrix_t * m2) {
  if (m1->height != m2->height || m1->width != m2->width) {
    return 0;
  }
  int row, column;
  for (row = 0; row < m1->height; row++) {
    for (column = 0; column < m1->width; column++) {
      if (get(m1, row, column) != get(m2, row, column)) {
	return 0;
      }
    }
  }
  return 1;
}


void printm(matrix_t * mat) {
  matrix_t * check = mat;
  printf("------\n");
  for (int i = 0; i < mat->height; i++) {
    putchar('<');
    putchar(' ');
    for (int j = 0; j < mat->width; j++) {
      printf("%.8f ", get(mat, i, j));
    }
    putchar('>');
    putchar('\n');
  }
  printf("------\n");
  assert(is_equal(check,mat));
}

matrix_t * madd(matrix_t * m1, matrix_t * m2) {
  assert(m1->height == m2->height);
  assert(m1->width == m2->width);
  matrix_t * check1 = m1;
  matrix_t * check2 = m2;
  matrix_t * result = matrix(m1->height, m1->width);
  int i, j;
  for (i = 0; i < m1->height; i++) {
    for (j = 0; j < m1->width; j++) {
      set(result, i, j, get(m1, i, j) + get(m2, i, j));
    }
  }
  assert(is_equal(check1, m1));
  assert(is_equal(check2, m2));
  return result;
}

matrix_t * msub(matrix_t * m1, matrix_t * m2) {
  assert(m1->height == m2->height);
  assert(m1->width == m2->width);
    matrix_t * check1 = m1;
  matrix_t * check2 = m2;

  matrix_t * result = matrix(m1->height, m1->width);
  int i, j;
  for (i = 0; i < m1->height; i++) {
    for (j = 0; j < m1->width; j++) {
      set(result, i, j, get(m1, i, j) - get(m2, i, j));
    }
  }
  assert(is_equal(check1, m1));
  assert(is_equal(check2, m2));
  return result;
}

matrix_t * mmult(matrix_t * m1, matrix_t * m2) {
  assert(m1->width == m2->height);
    matrix_t * check1 = m1;
  matrix_t * check2 = m2;

  matrix_t * result = matrix(m1->height, m2->width);
  int i, j, k;
  ELEM_TYPE sum;
  for (i = 0; i < result->height; i++) {
    for (j = 0; j < result->width; j++) {
      sum = 0;
      for (k = 0; k < m1->width; k++) {
  	sum += get(m1, i, k) * get(m2, k, j);
      }
      set(result, i, j, sum);
    }
  }
  assert(is_equal(m1, check1));
  assert(is_equal(m2, check2));
  return result;
}

matrix_t * apply_func(matrix_t * m1, float (*func)(float)) {
  matrix_t * check1 = m1;
  matrix_t * result = matrix(m1->height, m1->width);
  for (int i = 0; i < m1->height; i++) {
    for (int j = 0; j < m1->width; j++) {
      set(result, i, j, (*func)(get(m1, i, j)));
    }
  }
  assert(is_equal(check1, m1));
  return result;
}

matrix_t * copy(matrix_t * mat) {
  matrix_t * check = mat;
  matrix_t * result = matrix(mat->height, mat->width);
  for (int i = 0; i < mat->height; i++) {
    for (int j = 0; j < mat->width; j++) {
      set(result, i, j, get(mat, i, j));
    }
  }
  assert(is_equal(check, mat));
  return result;
}

void random_values(matrix_t * mat) {
  for (int i = 0; i < mat->height; i++) {
    for (int j = 0; j < mat->width; j++) {
      set(mat, i, j, rand());
    }
  }
}

matrix_t * id(int n) {
  matrix_t * result = matrix(n,n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == j) {
	set(result, i, j, 1);
      } else {
	set(result, i, j, 0);
      }
    }
  }
  return result;
}
