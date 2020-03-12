#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "neural_net.h"
#define IMAGE_SOURCE "/Users/roxannemackinnon/Documents/c_projects/machine_learning/neural_networks/digit_matrices.idx"
#define LABEL_SOURCE "/Users/roxannemackinnon/Documents/c_projects/machine_learning/neural_networks/digit_labels.idx"
#define TRAIN_START 0
#define TRAIN_END 50
#define TEST_START 0
#define TEST_END 50


/* /\* Updates the weights n times, giving information on cost and the like */
/*  * */
/*  *\/ */
/* void train_verbose(neural_net_t * net, int repeats) { */
/*   for (int epoch = 0; epoch < repeats; epoch++) { */
/*     printf("Training epoch %d...\n",epoch); */
/*     for (int image = TRAIN_START; image < TRAIN_END; image++) { */
/*       update_weights(net, *(matrices + image), *(output_vectors + image)); */
/*     } */
/*   } */
/* } */

/* float accuracy(neural_net_t * net) { */
/*   int correct = 0; */
/*   float result; */
/*   for (int image = TEST_START; image <= TEST_END; image++) { */
/*     if (max_index(calculate(*net, *(matrices + image))) == *(labels + image)) { */
/*       correct++; */
/*     } */
/*   } */
/*   result = (float) correct / (float) (TEST_END - TEST_START + 1); */
/*   return result; */
/* } */




int stoi(char * c) {
  int result = 0;
  for (int i = 0; *(c + i) != '\0'; i++) {
    result = 10*result + (*(c + i) - '0');
  }
  return result;
}


int main(int argc, char ** argv) {

  /* mat_file is an idx file that stores all 60000 784x784 pixel images in bytes */
  /* lab_file is an idx file that stores all 60000 labels (0-9) for the images, in bytes */

  matrix_t * train_matrices = parse_images(IMAGE_SOURCE, TRAIN_START, TRAIN_END);
  int * train_labels = parse_labels(LABEL_SOURCE, TRAIN_START, TRAIN_END);

  matrix_t * test_matrices = parse_images(IMAGE_SOURCE, TEST_START, TEST_END);
  int * test_labels = parse_labels(LABEL_SOURCE, TEST_START, TEST_END);

  /* the 'dimensions' of the neural net, i.e. how long each layer of nodes will be. we can derive the size of the weight matrices from there. */
  
  int * dims = malloc((argc-1) * sizeof(int));
  for (int i = 0; i < argc - 1; i++) {
    *(dims + i) = stoi(*(argv + i + 1));
  }
  
  neural_net_t * net = neural_net(dims, argc - 1, 0.1);

  node_values(net, train_matrices + 0);

  /* /\* calculate initial accuracy, without any training. *\/ */
  /* /\* accomplish this by retreiving the net's 'best guess' for each input and comparing it to the actual value *\/ */
  /* /\* best_guess simply returns the index of the largest element in a vector *\/ */

  return 0;
}
