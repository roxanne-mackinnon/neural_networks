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

int max_index(matrix_t * mat) {
  int index = 0;
  for (int i = 0; i < mat->height; i++) {
    if (get(mat, i, 0) > get(mat, index, 0)) {
      index = i;
    }
  }
  return index;
}

float accuracy(neural_net_t * net, matrix_t * tests, int * labs) {
  int correct = 0;
  float result;
  for (int image = TEST_START; image <= TEST_END; image++) {
    if (max_index(calculate(net, tests + image)) == *(labs + image)) {
      correct++;
    }
  }
  result = (float) correct / (float) (TEST_END - TEST_START + 1);
  return result;
}

matrix_t * output_for(int num) {
  matrix_t * result = matrix(10, 1);
  for (int i = 0; i < 10; i++) {
    set(result, i, 0, 0);
  }
  set(result, num, 0, 1);
  return result;
}

void train_verbose(neural_net_t * net, matrix_t * train_mats, int * train_labs, matrix_t * test_mats, int * test_labs, int repeats) {
  for (int epoch = 0; epoch < repeats; epoch++) {
    printf("%d   %.3f\n", epoch, accuracy(net, test_mats, test_labs));
    for (int image = TRAIN_START; image < TRAIN_END; image++) {
      update_weights(net, train_mats + image, output_for(*(train_labs + image)));
    }
  }
}


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
  
  neural_net_t * net = neural_net(dims, argc - 1, 0.5);

  train_verbose(net, train_matrices, train_labels, test_matrices, test_labels, 100);

  /* /\* calculate initial accuracy, without any training. *\/ */
  /* /\* accomplish this by retreiving the net's 'best guess' for each input and comparing it to the actual value *\/ */
  /* /\* best_guess simply returns the index of the largest element in a vector *\/ */

  return 0;
}
