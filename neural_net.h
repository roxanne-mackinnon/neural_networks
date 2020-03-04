#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"

/* time to clean up and clarify code through variable name changes */
typedef struct neural_net_t {
  matrix_t * weights;
  matrix_t * biases;
  int * layer_sizes;
  int layer_count;
} neural_net_t;


/* common functions for calculations */

float learning_rate(float n) {
  return 0.1 * n;
}

/* Returns a random float in [-1, 1] by retreiving a random unsigned byte [0,255]
 * shifting that byte to [-128,127], and finally dividing by 128.
 * pre: file is a source of (pseudo) random bytes.
 */
float random_zero_one(FILE *file) {
  return ((signed int) getc(file) - 128) / ((float) 128.0);
}

/* Activation function for nodes.
 */
float sigmoid(float t) {
  return (float) 1 / (1 + expf(-1 * t));
}




/* Randomly initialized the weights of a neural net, such that the weights lie in [-1,1].
 * input: a pointer to a neural net. using a reference will cause problems with memory.
 */

void initialize_randomly(neural_net_t * net) {
  FILE *random_source = fopen("/dev/random","r");
  for (int weight_layer = 0; weight_layer < (net->layer_count - 1); weight_layer++) {
    for (int row = 0; row < (((net->weights) + weight_layer)->height); row++) {
      set(net->biases + weight_layer, row, 0, random_zero_one(random_source));
      for (int column = 0; column < (((net->weights) + weight_layer)->width); column++) {
	set(net->weights + weight_layer, row, column, random_zero_one(random_source));
      }
    }
  }
}


neural_net_t neural_net(int * layer_sizes, int layer_count) {
  neural_net_t result;
  /* layer_sizes does not include the extra node that is always equal to -1, for biases.*/
  result.layer_sizes = layer_sizes;
  result.layer_count = layer_count;
  result.weights = (matrix_t *) malloc((layer_count - 1) * sizeof(matrix_t));
  result.biases = (matrix_t *) malloc((layer_count - 1) * sizeof(matrix_t));
  for (int i = 0; i < layer_count - 1; i++) {
    *(result.weights + i) = matrix(*(layer_sizes + i + 1),*(layer_sizes + i));
    *(result.biases + i) = matrix(*(layer_sizes + i + 1), 1);
  }
  initialize_randomly(&result);
  return result;
}


matrix_t calculate(neural_net_t net, matrix_t input) {
  if (input.height != net.weights->width) {
    printf("ERROR: invalid input vector (%d,%d).\n",input.height,input.width);
  }
  for (int layer = 0; layer < (net.layer_count) - 1; layer++) {
    input = apply_func(msub(mmult(*(net.weights + layer), input),*(net.biases + layer)), sigmoid);
  }
  return input;
}


int max_index(matrix_t mat) {
  int index = 0;
  for (int elem = 0; elem < mat.height; elem++) {
    if (get(mat, elem, 0) > get(mat, index, 0)) {
      index = elem;
    }
  }
  return index;
}


matrix_t * node_values(neural_net_t * net, matrix_t input) {
  if (input.height != (net->weights)->width) {
    printf("ERROR: invalid input vector.\n");
  }
  matrix_t * nodes = (matrix_t *) malloc((net->layer_count) * sizeof(matrix_t));
  *(nodes) = input;
  for (int layer = 0; layer < (net->layer_count - 1); layer++) {
    *(nodes + layer + 1) = apply_func(msub(mmult(*(net->weights + layer), *(nodes + layer)),*(net->biases + layer)), sigmoid);
  }
  return nodes;
}


float node_diff_weight(neural_net_t * net, matrix_t * values, int weight_layer, int weight_row, int weight_column, int node_layer, int node_row) {
  if (weight_layer < 0 || weight_row < 0 || weight_column < 0 || node_layer < 0 || node_row < 0) {
    printf("ERROR: NEGATIVE INDEX VALUES.\n");
  }
  float coefficient = get(*(values + node_layer), node_row, 0) * (1 - get(*(values + node_layer), node_row, 0));
  /* if we've reached the final relevant layer */
  if (node_layer == weight_layer + 1) {
    /* if our node is connected to the weight in question */
    if (node_row == weight_row) {
      return coefficient * get(*(values + weight_layer), weight_column, 0);
    }
    /* otherwise, our node is not a function of the weight */
    else {
      return 0;
    }
  }
  /* for each node in the previous layer */
  float summation = 0;
  int pre_node;
  for (pre_node = 0; pre_node < (*(net->layer_sizes + node_layer - 1)); pre_node++) {
    /* add the differential of (weight between node and previous node)*(value of previous node), given that the weight isn't the one we care about */
    summation = summation + get(*(net->weights + node_layer - 1), node_row, pre_node) * node_diff_weight(net, values, weight_layer, weight_row, weight_column, node_layer - 1, pre_node);
  }
  return coefficient * summation;
}


float node_diff_bias(neural_net_t * net, matrix_t * values, int bias_layer, int bias_row, int node_layer, int node_row) {
  if (bias_layer < 0 || bias_row < 0 || node_layer < 0 || node_row < 0) {
    printf("ERROR: NEGATIVE INDEX VALUES.\n");
  }
  float coefficient = get(*(values + node_layer), node_row, 0) * (1 - get(*(values + node_layer), node_row, 0));
  if (node_layer == bias_layer + 1) {
    if (node_row == bias_row) {
      return -1 * coefficient;
    } else {
      return 0;
    }
  }

  float summation = 0;
  int pre_node;
  for (pre_node = 0; pre_node < (*(net->layer_sizes + node_layer - 1)); pre_node++) {
    summation = summation + get(*(net->weights + node_layer - 1), node_row, pre_node) * node_diff_bias(net, values, bias_layer, bias_row, node_layer - 1, pre_node);
  }
  return coefficient * summation;
}


float cost_diff_weight(neural_net_t * net, matrix_t *values, matrix_t error, int weight_layer, int weight_row, int weight_column) {
  float sum = 0;
  int final_layer_length = error.height;
  for (int p = 0; p < final_layer_length; p++) {
    sum += get(error, p, 0) * node_diff_weight(net, values, weight_layer, weight_row, weight_column, net->layer_count - 1, p);
  }
  return -1 * sum;
}


float cost_diff_bias(neural_net_t * net, matrix_t * values, matrix_t error, int bias_layer, int bias_row) {
  float sum = 0;
  int final_layer_length = error.height;
  for (int p = 0; p < final_layer_length; p++) {
    sum += get(error, p, 0) * node_diff_bias(net, values, bias_layer, bias_row, net->layer_count - 1, p);
  }
  return -1 * sum;
}


matrix_t * weight_gradient(neural_net_t * net, matrix_t * values, matrix_t desired_output) {
  matrix_t error = msub(desired_output, *(values + net->layer_count - 1));
  matrix_t * grad = (matrix_t *) malloc((net->layer_count - 1) * sizeof(matrix_t));
  int layer, row, col;
  for (layer = 0; layer < (net->layer_count - 1); layer++) {
    *(grad + layer) = matrix(*(net->layer_sizes + layer + 1), *(net->layer_sizes + layer));
    for (row = 0; row < *(net->layer_sizes + layer + 1); row++) {
      for (col = 0; col < *(net->layer_sizes + layer); col++) {
	set(grad + layer, row, col, cost_diff_weight(net, values, error, layer, row, col));
      }
    }
  }
  return grad;
}


matrix_t * bias_gradient(neural_net_t * net, matrix_t * values, matrix_t desired_output) {
  matrix_t error = msub(desired_output, *(values + net->layer_count - 1));
  matrix_t * grad = (matrix_t *) malloc((net->layer_count - 1) * sizeof(matrix_t));
  int layer, row;
  for (layer = 0; layer < (net->layer_count - 1); layer++) {
    *(grad + layer) = matrix(*(net->layer_sizes + layer + 1), 1);
    for (row = 0; row < *(net->layer_sizes + layer + 1); row++) {
      set(grad + layer, row, 0, cost_diff_bias(net, values, error, layer, row));
    }
  }
  return grad;
}


void update_weights(neural_net_t * net, matrix_t input, matrix_t desired_output) {
  matrix_t * weight_grad = weight_gradient(net, node_values(net, input), desired_output);
  matrix_t * bias_grad = bias_gradient(net, node_values(net, input), desired_output);
  for (int layer = 0; layer < (net->layer_count - 1); layer++) {
    *(net->weights + layer) = msub(*(net->weights + layer), apply_func(*(weight_grad + layer), learning_rate));
    *(net->biases + layer) = msub(*(net->biases + layer), apply_func(*(bias_grad + layer), learning_rate));
  }
}
