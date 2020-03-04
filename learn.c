#include <stdio.h>
#include <stdlib.h>
#include "idx_reader.h"
#include "neural_net.h"
#define TRAIN_START 0
#define TRAIN_END 50
#define TEST_START 100
#define TEST_END 200

FILE *mat_file;
FILE *lab_file;

matrix_t * matrices;
matrix_t * output_vectors;

float *** images;
int * labels;


matrix_t output(char num) {
  matrix_t result = matrix(10,1);
  for (int i=0; i<10; i++) {
    set(&result,i,0,0);
  }
  set(&result,num,0,1);
  return result;
}

/* Updates the weights n times, giving information on cost and the like
 *
 */
void train_verbose(neural_net_t * net, int repeats) {
  for (int epoch = 0; epoch < repeats; epoch++) {
    printf("Training epoch %d...\n",epoch);
    for (int image = TRAIN_START; image < TRAIN_END; image++) {
      update_weights(net, *(matrices + image), *(output_vectors + image));
    }
  }
}

float accuracy(neural_net_t * net) {
  int correct = 0;
  float result;
  for (int image = TEST_START; image <= TEST_END; image++) {
    if (max_index(calculate(*net, *(matrices + image))) == *(labels + image)) {
      correct++;
    }
  }
  result = (float) correct / (float) (TEST_END - TEST_START + 1);
  return result;
}







int main(int argc, char ** argv) {

  /* mat_file is an idx file that stores all 60000 784x784 pixel images in bytes */
  /* lab_file is an idx file that stores all 60000 labels (0-9) for the images, in bytes */
  mat_file = fopen("/Users/roxannemackinnon/Documents/c_projects/machine_learning/neural_networks/digit_matrices.idx","r");
  lab_file = fopen("/Users/roxannemackinnon/Documents/c_projects/machine_learning/neural_networks/digit_labels.idx","r");
  
  matrices = (matrix_t *) malloc(60000*sizeof(matrix_t));
  output_vectors = (matrix_t *) malloc(60000*sizeof(matrix_t));


  /* images is a 3D float pointer */
  /* labels is a 1D int pointer */
  images = parse_images(mat_file);
  labels = parse_labels(lab_file);

  /* we turn out pointer pointer pointers into actual matrix structs, defined under multiplication, etc. */
  for (int i = TRAIN_START; i <= TRAIN_END; i++) {
    *(matrices+i) = matrix(784,1);
    (matrices+i)->values = get_vector(images,i);
    *(output_vectors+i) = output(*(labels+i));
  }

  for (int i = TEST_START; i <= TEST_END; i++) {
    *(matrices+i) = matrix(784,1);
    (matrices+i)->values = get_vector(images,i);
    *(output_vectors+i) = output(*(labels+i));
  }


  /* map_to_output turns a label into a 10 x 1 vector  representing the 'ideal output' for a given matrix input */

  /* the 'dimensions' of the neural net, i.e. how long each layer of nodes will be. we can derive the size of the weight matrices from there. */
  int * dims = malloc(3*sizeof(int));
  *dims = 784;
  *(dims+1) = 16;
  *(dims+2) = 10;
  
  neural_net_t net = neural_net(dims,3); /* 3 layers */

  /* calculate initial accuracy, without any training. */
  /* accomplish this by retreiving the net's 'best guess' for each input and comparing it to the actual value */
  /* best_guess simply returns the index of the largest element in a vector */


  printf("initial accuracy: %f\n",accuracy(&net));
  train_verbose(&net, 10);
  printf("posterior accuracy: %f\n",accuracy(&net));
  

  return 0;
}
