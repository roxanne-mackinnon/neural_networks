#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "idx_reader.h"




/* time to clean up and clarify code through variable name changes */
typedef struct neural_net_t {
  matrix_t * values;
  matrix_t * weights;
  matrix_t * biases;
  matrix_t * weight_gradient;
  matrix_t * bias_gradient;
  matrix_t * error;
  int * layer_sizes;
  int layer_count;
  float learning_rate;
} neural_net_t;


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
  return (float) 1 / (1 + expf(-1*t));
}


matrix_t * net_weight(neural_net_t * net, int layer) {
  assert(0 <= layer && layer < net->layer_count);
  return net->weights + layer;
}

matrix_t * net_node_layer(neural_net_t * net, int layer) {
  assert(0 <= layer && layer <= net->layer_count);
  return net->values + layer;
}

matrix_t * net_bias_layer(neural_net_t * net, int layer) {
  assert(0 < layer && layer < net->layer_count);
  return net->biases + layer;
}


/* Randomly initialized the weights of a neural net, such that the weights lie in [-1,1].
 * input: a pointer to a neural net. using a reference will cause problems with memory.
 */

void initialize_randomly(neural_net_t * net) {
  FILE *random_source = fopen("/dev/random","r");
  for (int weight_layer = 0; weight_layer < (net->layer_count - 1); weight_layer++) {
    for (int row = 0; row < *(net->layer_sizes + weight_layer + 1); row++) {
      set(net->biases + weight_layer, row, 0, random_zero_one(random_source));
      for (int column = 0; column < *(net->layer_sizes + weight_layer); column++) {
	set(net->weights + weight_layer, row, column, random_zero_one(random_source));
      }
    }
  }
}



neural_net_t neural_net(int * layer_sizes, int layer_count, float rate) {
  neural_net_t result;
  /* layer_sizes does not include the extra node that is always equal to -1, for biases.*/
  result.layer_sizes = layer_sizes;
  result.layer_count = layer_count;
  result.learning_rate = rate;
  result.values = (matrix_t *) malloc(layer_count * sizeof(matrix_t));
  result.weights = (matrix_t *) malloc((layer_count - 1) * sizeof(matrix_t));
  result.biases = (matrix_t *) malloc((layer_count - 1) * sizeof(matrix_t));
  result.weight_gradient = (matrix_t *) malloc((layer_count - 1) * sizeof(matrix_t));
  result.bias_gradient = (matrix_t *) malloc((layer_count - 1) * sizeof(matrix_t));
  result.error = (matrix_t *) malloc(1 * sizeof(matrix_t));
  for (int i = 0; i < layer_count - 1; i++) {
    *(result.values + i) = matrix(*(layer_sizes + i),1);
    *(result.weights + i) = matrix(*(layer_sizes + i + 1),*(layer_sizes + i));
    *(result.biases + i) = matrix(*(layer_sizes + i + 1), 1);
    *(result.weight_gradient + i) = matrix(*(layer_sizes + i + 1),*(layer_sizes + i));
    *(result.bias_gradient + i) = matrix(*(layer_sizes + i + 1), 1);
  }
  *(result.values + layer_count - 1) = matrix(*(layer_sizes + layer_count - 1), 1);
  initialize_randomly(&result);
  return result;
}

int max_index(matrix_t * mat) {
  int index = 0;
  for (int elem = 0; elem < mat->height; elem++) {
    if (get(mat, elem, 0) > get(mat, index, 0)) {
      index = elem;
    }
  }
  return index;
}


void node_values(neural_net_t * net, matrix_t * input) {
  assert((input->width == 1) && (input->height == *(net->layer_sizes + 0)));
  net->values = input;
  for (int layer = 0; layer < net->layer_count - 1; layer++) {
    *(net->values + layer + 1) = *(apply_func(msub(mmult(net->weights + layer, net->values + layer), net->biases + layer), sigmoid));
  }
}

matrix_t * calculate(neural_net_t * net, matrix_t * input) {
  assert((input->width == 1) && (input->height == *(net->layer_sizes + 0)));
  matrix_t * result = input;
  for (int i = 0; i < net->layer_count - 1; i++) {
    result = apply_func(msub(mmult(net->weights + i, result),net->biases + i),sigmoid);
  }
  return result;
}



float node_diff_weight(neural_net_t * net, int weight_layer, int weight_row, int weight_column, int node_layer, int node_row) {
  float coefficient = get(net->values + node_layer, node_row, 0) * (1 - get(net->values + node_layer, node_row, 0));
  /* if we've reached the final relevant layer */
  if (node_layer == weight_layer + 1) {
    /* if our node is connected to the weight in question */
    if (node_row == weight_row) {
      return coefficient * get(net->values + weight_layer, weight_column, 0);
    }
    /* otherwise, our node is not a function of the weight */
    else {
      return 0;
    }
  }
  /* for each node in the previous layer */
  float summation = 0;
  int pre_node;
  for (pre_node = 0; pre_node < *(net->layer_sizes + node_layer - 1); pre_node++) {
    /* add the differential of (weight between node and previous node)*(value of previous node), given that the weight isn't the one we care about */
    summation = summation + get(net->weights + node_layer - 1, node_row, pre_node) * node_diff_weight(net, weight_layer, weight_row, weight_column, node_layer - 1, pre_node);
  }
  return coefficient * summation;
}


float node_diff_bias(neural_net_t * net, int bias_layer, int bias_row, int node_layer, int node_row) {
  float coefficient = get(net->values + node_layer, node_row, 0) * (1 - get(net->values + node_layer, node_row, 0));
  if (node_layer == bias_layer + 1) {
    if (node_row == bias_row) {
      return -1 * coefficient;
    } else {
      return 0;
    }
  }

  float summation = 0;
  int pre_node;
  for (pre_node = 0; pre_node < *(net->layer_sizes + node_layer - 1); pre_node++) {
    summation = summation + get(net->weights + node_layer - 1, node_row, pre_node) * node_diff_bias(net, bias_layer, bias_row, node_layer - 1, pre_node);
  }
  return coefficient * summation;
}


float cost_diff_weight(neural_net_t * net, int weight_layer, int weight_row, int weight_column) {
  float sum = 0;
  int final_layer_length = *(net->layer_sizes + net->layer_count - 1);
  for (int p = 0; p < final_layer_length; p++) {
    sum += get(net->error, p, 0) * node_diff_weight(net, weight_layer, weight_row, weight_column, net->layer_count - 1, p);
  }
  return -1 * sum;
}


float cost_diff_bias(neural_net_t * net, int bias_layer, int bias_row) {
  float sum = 0;
  int final_layer_length = *(net->layer_sizes + net->layer_count - 1);
  for (int p = 0; p < final_layer_length; p++) {
    sum += get(net->error, p, 0) * node_diff_bias(net, bias_layer, bias_row, net->layer_count - 1, p);
  }
  return -1 * sum;
}


void weight_gradient(neural_net_t * net) {
  int layer, row, col;
  for (layer = 0; layer < net->layer_count - 1; layer++) {
    for (row = 0; row < *(net->layer_sizes + layer + 1); row++) {
      for (col = 0; col < *(net->layer_sizes + layer); col++) {
	set(net->weight_gradient + layer, row, col, net->learning_rate * cost_diff_weight(net, layer, row, col));
      }
    }
  }
}


void bias_gradient(neural_net_t * net) {
  int layer, row;
  for (layer = 0; layer < net->layer_count - 1; layer++) {
    for (row = 0; row < *(net->layer_sizes + layer + 1); row++) {
      set(net->bias_gradient + layer, row, 0, net->learning_rate * cost_diff_bias(net, layer, row));
    }
  }
}


void update_weights(neural_net_t * net, matrix_t * input, matrix_t * desired_output) {
  node_values(net, input);
  net->error = msub(desired_output, net->values + net->layer_count - 1);
  weight_gradient(net);
  bias_gradient(net);
  for (int layer = 0; layer < net->layer_count - 1; layer++) {
    *(net->weights + layer) = *(msub(net->weights + layer, net->weight_gradient + layer));
    *(net->biases + layer) = *(msub(net->biases + layer, net->bias_gradient + layer));
  }
}
