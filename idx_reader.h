#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

/* lets see how it interprets input as hex */
/* what have we learned? you can it will correctly identify each hex */
/* okay, even though we don't know how to read files in proper yet, lets try to write a program to decode idx files */

/*

magic number
size in dimension 0
size in dimension 1
nsize in dimension 2
.....
nnsize in dimension N
data

The magic number is an integer (MSB first). The first 2 bytes are always 0.

The third byte codes the type of the data:
0x08: unsigned byte
0x09: signed byte
0x0B: short (2 bytes)
0x0C: int (4 bytes)
0x0D: float (4 bytes)
0x0E: double (8 bytes)

The 4-th byte codes the number of dimensions of the vector/matrix: 1 for vectors, 2 for matrices....

The sizes in each dimension are 4-byte integers (MSB first, high endian, like in most non-Intel processors).

The data is stored like in a C array, i.e. the index in the last dimension changes the fastest.

*/

int stoi(char *s) {
  int result =0;
  for (int i=0; *(s+i) != '\0';i++) {
    result = 10*result + (*(s+i)-'0');
  }
  return result;
}

static int bytes_to_int(float b1, float b2, float b3, float b4) {
  int coeff = 16*16;
  return b4 + b3*coeff + b2*coeff*coeff + b1*coeff*coeff*coeff;
}

static float get_char(FILE *fp) {
  return (float) getc(fp);
}

static unsigned int getint(FILE *fp) {
  float b1,b2,b3,b4;
  b1=get_char(fp);
  b2=get_char(fp);
  b3=get_char(fp);
  b4=get_char(fp);
  return bytes_to_int(b2,b2,b3,b4);
}


void parse_images(FILE *fp, matrix_t * images) {

  assert(getc(fp) == '\x00');
  assert(getc(fp) == '\x00');

  int data_type = getc(fp);
  int dimcount = getc(fp);
  int * dims = malloc(dimcount * sizeof(int));
 
  *(dims) = getint(fp);
  printf("%d\n",*dims);
  *(dims + 1) = getint(fp);
  *(dims + 2) = getint(fp);

  assert(*(dims) == 60000);
  assert(*(dims+1) == 28);
  assert(*(dims+2) == 28);

  for (int i = 0; i < *dims; i++) {
    for (int j = 0; j < *(dims+1); j++) {
      for (int k = 0; k < *(dims+2); k++) {
	set(images + i, j * *(dims + 1) + k, 0, get_char(fp));
      }
    }
  }
}

void parse_labels(FILE *fp, int * l) {
  getint(fp);
  getint(fp);
  for (int i = 0; i < 60000; i++) {
    *(l+i) = getc(fp);
  }
}

float * get_vector(float *** arr, int n) {
  assert(0 <= n && n < 60000);
  return *(*(arr + n));
}

void print_matrix(matrix_t * mats, int n) {
  assert(0 <= n && n < 60000);
  for (int i=0; i<28; i++) {
    for (int j=0; j<28; j++) {
      if (get(mats + n, i*28 +j, 0) < 0.25) {
	putchar(' ');
      }
      else if (0.25 <= get(mats + n, i*28 +j, 0) && get(mats + n, i*28 +j, 0) <0.5) {
	putchar('.');
      }
      else if (0.5 <= get(mats + n, i*28 +j, 0) && get(mats + n, i*28 +j, 0) < 0.75) {
	putchar('~');
      }
      else {
	putchar('&');
      }
    }
    putchar('\n');
  }
}
