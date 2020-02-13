#include <stdio.h>
#include <stdlib.h>
#include "idx_reader.h"
#include "neural_net.h"
#define START 0
#define END 5


struct matrix output(char num) {
  struct matrix result = makematrix(10,1);
  for (int i=0; i<10; i++) {
    assign(&result,i,0,0);
  }
  assign(&result,num,0,1);
  return result;
}

int main(int argc, char ** argv) {

  /* these are the two files that are needed for the net. the first are matrices of images of drawn digits, the second is the labels for those images. */
  FILE *mat_file = fopen("/Users/roxannemackinnon/Documents/c_projects/neural_networks/digit_matrices.idx","r");
  FILE *lab_file = fopen("/Users/roxannemackinnon/Documents/c_projects/neural_networks/digit_labels.idx","r");

  /* arr is meant to store all the matrices. each element will point to an array of pointers pointing to matrix rows. labs stores the labels */
  float *** arr = parse_images(mat_file);
  int * labs = parse_labels(lab_file);

  struct matrix * matrices = (struct matrix *) malloc(60000*sizeof(struct matrix));
  struct matrix * output_vectors = (struct matrix *) malloc(60000*sizeof(struct matrix));
  
  /* we turn out pointer pointer pointers into actual matrix structs, defined under multiplication, etc. */
  for (int i=0;i<60000; i++) {
    *(matrices+i) = makematrix(784,1);
    (*(matrices+i)).vrep = get_vector(arr,i);
    *(output_vectors+i) = output(*(labs+i));
  }
  
  /* map_to_output turns a label into a 10 x 1 vector  representing the 'ideal output' for a given matrix input */

  /* the 'dimensions' of the neural net, i.e. how long each layer of nodes will be. we can derive the size of the weight matrices from there. */
  int * dims = malloc(4*sizeof(int));
  *dims = 784;
  *(dims+1) = 16;
  *(dims+2) = 16;
  *(dims+3) = 10;
  
  struct net n = makenet(dims,4); /* 4 layers */

  /* calculate initial accuracy, without any training. */
  /* accomplish this by retreiving the net's 'best guess' for each input and comparing it to the actual value */
  /* best_guess simply returns the index of the largest element in a vector */
  printf("first one:\n");
  printm(calculate(n,*matrices));
  printf("second one:\n");
  printm(calculate(n,*(matrices+1)));
  /* now, let's go through and update the weight matrix for each thing */
  for (int i=START; i<END; i++) {
    update_weights(&n,*(matrices),*(output_vectors));
    update_weights(&n,*(matrices+1),*(output_vectors+1));
    printf("calculating %d...\n",i);
  }
  printf("first one:\n");
  printm(calculate(n,*matrices));
  printf("second one:\n");
  printm(calculate(n,*(matrices+1)));
  return 0;
}