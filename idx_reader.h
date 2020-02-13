#include <stdio.h>
#include <stdlib.h>

/* lets see how it interprets input as hex */
/* what have we learned? you can it will correctly identify each hex */
/* okay, even though we don't know how to read files in proper yet, lets try to write a program to decode idx files */

/*

magic number
size in dimension 0
size in dimension 1
size in dimension 2
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

/*
void allocate_ptr(void * (*ptr), int * dims, int dimcount) {
  if (dimcount == 2) {
    ptr = (char **) malloc(*dims * sizeof(char*));
    for (int i=0; i<*dims; i++) {
      *(ptr+i) = malloc(*(dims+1) * sizeof(char));
    }
    return;
  }
  ptr = (void **) malloc(*dims * sizeof(void *));
  for (int i=0; i < *dims; i++) {
    allocate_ptr(*(ptr+i),dims+1,dimcount-1);
  }
}
*/

int stoi(char *s) {
  int result =0;
  for (int i=0; *(s+i) != '\0';i++) {
    result = 10*result + (*(s+i)-'0');
  }
  return result;
}

int bytes_to_int(float b1, float b2, float b3, float b4) {
  int coeff = 16*16;
  return b4 + b3*coeff + b2*coeff*coeff + b1*coeff*coeff*coeff;
}

float get_char(FILE *fp) {
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


float *** parse_images(FILE *fp) {

  if (getc(fp) != '\x00' || getc(fp) != '\x00') {
    printf("ERROR: invalid startbits for idx file.\n");
  }

  int data_type = getc(fp);
  int dimcount = getc(fp);
  int * dims = malloc(dimcount * sizeof(int));
  /*  for (int i=0; i<dimcount; i++) {
    *(dims+i) = getint(fp);
    } */
  getint(fp);
  getint(fp);
  getint(fp);
  *dims = 60000;
  *(dims+1) = 28;
  *(dims+2) = 28;

  float *** mats = (float ***) malloc(*dims * sizeof(float **));
  /*  allocate_ptr(mats,dims,dimcount); */
  for (int i = 0; i< *dims; i++) {
    *(mats+i) = (float **) malloc(*(dims+1) * sizeof(float *));
    for (int j=0; j< *(dims+1); j++) {
      *(*(mats+i)+j) = (float *) malloc(*(dims+2) * sizeof(float));
      for (int k=0; k< *(dims+2); k++) {
	*(*(*(mats+i)+j)+k) = getc(fp);
      }
    }
  }
  return mats;
}

int * parse_labels(FILE *fp) {
  getint(fp);
  int count = getint(fp);

  int * labels = (int *) malloc(count*sizeof(int));
  for (int i=0; i<count; i++) {
    *(labels+i) = getc(fp);
  }
  return labels;
}

float * get_vector(float *** arr, int n) {
  if (0>n || n>=60000) {
    printf("ERROR: chosnen matrix number must be between 0 and 59999\n");
    return 0;
  }
  float * result = (float *) malloc(28*28*sizeof(float));
  for (int i=0; i<28; i++) {
    for (int j=0; j<28; j++) {
      *(result + 28*i + j) = *(*(*(arr+n)+i)+j);
    }
  }
  return result;
}

float ** get_matrix(float *** arr, int n) {
  if (0>n || n>=60000) {
    printf("ERROR: chosen matrix number must be between 0 and 59999\n");
    return 0;
  }
  return *(arr+n);
}

void print_matrix(float *** arr, int n) {
  if (0>n || n>=60000) {
    printf("ERROR: invalid matrix number. must be between 0-59999\n");
    return;
  }
  for (int i=0; i<28; i++) {
    for (int j=0; j<28; j++) {
      if (*(*(*(arr+n)+i)+j) < 50) {
	putchar(' ');
      }
      else if (50 <= *(*(*(arr+n)+i)+j) && *(*(*(arr+n)+i)+j) <100) {
	putchar('.');
      }
      else if (100 <= *(*(*(arr+n)+i)+j) && *(*(*(arr+n)+i)+j) < 200) {
	putchar('~');
      }
      else {
	putchar('&');
      }
    }
    putchar('\n');
  }
}
