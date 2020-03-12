#include <stdio.h>
#include <stdlib.h>
#include "vector.h"


/*
char 0
char 0
char type
char dim
int size in dimension 0
int size in dimension 1
int size in dimension 2
.....
int size in dimension N
(type) data

The magic number is an integer (MSB first). The first 2 bytes are always 0.

The third byte codes the type of the data:
0x08: unsigned byte
0x09: signed byte
0x0B: short (2 bytes)
0x0C: int (4 bytes)
0x0D: float (4 bytes)
0x0E: double (8 bytes)

The sizes in each dimension are 4-byte integers (MSB first, high endian, like in most non-Intel processors).
The data is stored like in a C array, i.e. the index in the last dimension changes the fastest.

*/

unsigned int getint(FILE *fp) {
  int result = (getc(fp)<<24) | (getc(fp)<<16) | (getc(fp)<<8) | (getc(fp)<<0);
  return result;
}


/* sets elements of matrices to unsigned chars, not floats between 0 and 1 */
matrix_t * parse_images(char * path, int start, int end) {

  FILE * fp = fopen(path, "r");
  fseek(fp, 0, SEEK_SET);
  
  /* first 2 bytes should always be zero */
  assert(fgetc(fp) == '\x00');
  assert(fgetc(fp) == '\x00');

  /* we do not do anything with data types yet */
  int data_type = fgetc(fp);
  int dimcount = fgetc(fp);
  
  int * dims = malloc(dimcount * sizeof(int));

  /* we cannot deal with vectors in arbitrary dims */
  assert(dimcount == 3);
 
  *(dims) = getint(fp);
  *(dims + 1) = getint(fp);
  *(dims + 2) = getint(fp);

  assert(start >= 0);
  assert(start < *(dims));

  matrix_t * result = (matrix_t *) malloc((end - start) * sizeof(matrix_t));

  fseek(fp, 17 + start * (*(dims + 1)) * (*(dims + 2)), SEEK_SET);
  int count = end - start;
  for (int element = 0; element < count && element < *(dims); element++) {
    *(result + element) = *(matrix(*(dims+1) * *(dims+2), 1));
    for (int row = 0; row < *(dims + 1); row++) {
      for (int column = 0; column < *(dims + 2); column++) {
	set(result + element, row * (*(dims + 1)) + column, 0, (float) fgetc(fp) / (float) 255.0);
      }
    }
  }
  fclose(fp);
  return result;
}

int * parse_labels(char * path, int start, int end) {

  FILE * fp = fopen(path, "r");
  fseek(fp, 0, SEEK_SET);
  
  /* first 2 bytes should always be zero */
  assert(fgetc(fp) == '\x00');
  assert(fgetc(fp) == '\x00');

  /* type (cant do anything with it yet) */
  fgetc(fp);

  /* dims (should be 1) */
  fgetc(fp);
  
  int filesize = getint(fp);
  int count = (end - start);

  assert(start >= 0);
  assert(end < filesize);
  
  fseek(fp, 8 + start, SEEK_SET);
  int * result = malloc(count * sizeof(int));
  for (int i = 0; i < count; i++) {
    *(result+i) = fgetc(fp);
  }
  fclose(fp);
  return result;
}


