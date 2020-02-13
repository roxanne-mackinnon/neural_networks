#include <stdio.h>
#include <stdlib.h>
#define ELEM_TYPE float


struct matrix {
  int height;
  int width;
  ELEM_TYPE * vrep;
};

struct matrix makematrix(int h, int w) {
  struct matrix result;
  result.height=h;
  result.width=w;
  result.vrep = (ELEM_TYPE *) malloc(h*w*sizeof(ELEM_TYPE));
  return result;
}

void assign(struct matrix * mat, int i, int j, ELEM_TYPE elem) {
  if (i*(mat->width) + j >= (mat->width)*(mat->height)) {
  }
  *(mat->vrep + i*(mat->width) + j) = elem; /* what should be a dereferenced pointer to the correct location in memory */
}

ELEM_TYPE retreive(struct matrix mat, int i, int j) {
  if (i*mat.width + j >= mat.width*mat.height) {

  }
  return *(mat.vrep + i*(mat.width) + j);
}

void printm(struct matrix mat) {
  for (int i=0; i<(mat.height); i++) {
    putchar('<');
    putchar(' ');
    for (int j=0;j<(mat.width); j++) {
      printf("%.8f ",retreive(mat,i,j));
    }
    putchar('>');
    putchar('\n');
  }
}

struct matrix madd(struct matrix m1, struct matrix m2) {
  if (m1.height != m2.height || m1.width != m2.width) {
    printf("ERROR: INCONSISTENT MATRIX SIZES FOR ADDITION\n");
    return makematrix(0,0);
  }
  struct matrix result = makematrix(m1.height,m1.width);
  int i,j;
  for (i=0; i<m1.height; i++) {
    for (j=0; j<m1.width; j++) {
      assign(&result,i,j,retreive(m1,i,j)+retreive(m2,i,j));
    }
  }
  return result;
}

struct matrix msub(struct matrix m1, struct matrix m2) {
  if (m1.height != m2.height || m1.width != m2.width) {
    printf("ERROR: INCONSISTENT MATRIX SIZES (%d,%d) x (%d,%d) FOR SUBTRACTION\n",m1.height,m1.width,m2.height,m2.width);
    return makematrix(0,0);
  }
  struct matrix result = makematrix(m1.height,m2.width);
  int i,j;
  for (i=0;i<m1.height;i++) {
    for (j=0;j<m1.width;j++) {
      assign(&result,i,j,retreive(m1,i,j)-retreive(m2,i,j));
    }
  }
  return result;
}

struct matrix mmult(struct matrix m1, struct matrix m2) {
  struct matrix result = makematrix(m1.height,m2.width);
  int i,j,k;
  ELEM_TYPE sum;
  for (i=0; i<result.height; i++) {
    for (j=0;j<result.width; j++) {
      sum = 0;
      for (k=0;k<m1.width;k++) {
	sum = sum + retreive(m1,i,k) * retreive(m2,k,j);
      }
      assign(&result,i,j,sum);
    }
  }
  return result;
}

struct matrix apply_func(struct matrix mat, float (*foo)(float)) {
  int i,j;
  for (i=0; i<mat.height; i++) {
    for (j=0; j<mat.width;  j++) {
      assign(&mat,i,j,(*foo)(retreive(mat,i,j)));
    }
  }
  return mat;
}
