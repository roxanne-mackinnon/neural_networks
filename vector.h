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

ELEM_TYPE get(matrix_t * mat, int i, int j) {
  if (i < 0 || i >= mat->height || j < 0 || j >= mat->width) {
    fprintf(stderr,"ERROR: index (%d,%d) out of bounds for matrix (%d,%d)\n",i+1,j+1,mat->height,mat->width);
  }
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
  if (m1->height != m2->height || m1->width != m2->width) {
    fprintf(stderr,"ERROR: INCONSISTENT MATRIX SIZES FOR ADDITION\n");
  }
  matrix_t result = matrix(m1->height, m1->width);
  matrix_t * ptr = &result;
  int i, j;
  for (i = 0; i < m1->height; i++) {
    for (j = 0; j < m1->width; j++) {
      set(ptr, i, j, get(m1, i, j) + get(m2, i, j));
    }
  }
  return ptr;
}

matrix_t * msub(matrix_t * m1, matrix_t * m2) {
  if (m1->height != m2->height || m1->width != m2->width) {
    fprintf(stderr, "ERROR: INCONSISTENT MATRIX SIZES (%d,%d) - (%d,%d) FOR SUBTRACTION\n", m1->height, m1->width, m2->height, m2->width);
  }
  matrix_t result = matrix(m1->height, m1->width);
  matrix_t * ptr = &result;
   int i, j;
  for (i = 0; i < m1->height; i++) {
    for (j = 0; j < m1->width; j++) {
      set(ptr, i, j, get(m1, i, j) - get(m2, i, j));
    }
  }
  return ptr;
}

matrix_t * mmult(matrix_t * m1, matrix_t * m2) {
  if (m1->width != m2->height) {
    fprintf(stderr,"ERROR: matrix (%d,%d) incompatible for multiplication with matrix (%d,%d)\n",m1->height,m1->width,m2->height,m2->width);
  }
  
  matrix_t * ptr;
  ptr->height = m1->height;
  ptr->width = m2->width;
  ptr->values = (ELEM_TYPE *) malloc((ptr->width) * (ptr->height) * sizeof(ELEM_TYPE));
  int i, j, k;
  ELEM_TYPE sum;
  for (i = 0; i < ptr->height; i++) {
    for (j = 0; j < ptr->width; j++) {
      sum = 0;
      for (k = 0; k < m1->width; k++) {
  	sum = sum + get(m1, i, k) * get(m2, k, j);
      }
      set(ptr, i, j, sum);
    }
  }
  return ptr;
}

matrix_t * apply_func(matrix_t * mat, float (*func)(float)) {
  int i, j;
  matrix_t result = matrix(mat->height,mat->width);
  matrix_t * ptr = &result;
  float buff;
  for (i = 0; i < mat->height; i++) {
    for (j = 0; j < mat->width;  j++) {
      buff = get(mat, i, j);
      set(ptr, i, j, func(buff));
    }
  }
  return ptr;
}
