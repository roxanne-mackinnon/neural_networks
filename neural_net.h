#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"


/* forget about types for now. we will clean that up later. */

struct net {
  struct matrix * weights;
  int * layer_sizes;
  int layer_count;
};

float learning_rate(float n) {
  return 0.1*n;
}

float rand_p(FILE *rf) {
  return (float)((signed int) getc(rf) - 128)/((float)128.0);
}

float sigmoid(float t) {
  return (float) 1/(1+expf(-1*t));
}

void initialize_randomly(struct net * n) {
  FILE *fp = fopen("/dev/random","r");
  for (int w=0; w<(n->layer_count-1);w++) {
    for (int i=0; i<(((n->weights)+w)->height); i++) {
      for (int j=0;j<(((n->weights)+w)->width); j++) {
	assign(n->weights+w,i,j,rand_p(fp));
      }
    }
  }
}

void initialize_zero(struct net * n) {
  int w,i,j;
  for (w=0;w<(n->layer_count-1);w++) {
    for (i=0;i<(((n->weights)+w)->height); i++) {
      for (j=0; j<(((n->weights)+w)->width);j++) {
	assign(n->weights+w,i,j,0);
      }
    }
  }
}

struct net makenet(int * layer_sizes, int layer_count) {
  struct net result;
  result.layer_sizes = layer_sizes;
  result.layer_count = layer_count;
  result.weights = (struct matrix *) malloc((layer_count-1)*sizeof(struct matrix));
  for (int i=0; i<layer_count-1; i++) {
    *(result.weights+i) = makematrix(*(layer_sizes+i+1),*(layer_sizes+i));
  }
  initialize_randomly(&result);
  return result;
}

struct matrix calculate(struct net n, struct matrix input) {
  if (input.height != n.weights->width) {
    printf("ERROR: invalid input vector.\n");
  }

  for (int i=0;i<(n.layer_count)-1; i++) {
    input = apply_func(mmult(*(n.weights+i),input),sigmoid);
  }
  return input;
}

int max_index(struct matrix mat) {
  int ind = 0;
  for (int i=0;i<mat.height; i++) {
    if (retreive(mat,i,0) > retreive(mat,ind,0)) {
      ind = i;
    }
  }
  return ind;
}

struct matrix * node_values(struct net *n, struct matrix input) {
  if (input.height != (n->weights)->width) {
    printf("ERROR: invalid input vector.\n");
    
  }
  struct matrix *nodes = (struct matrix *) malloc((n->layer_count)*sizeof(struct matrix));
  *(nodes) = input;
  for (int i=0;i<(n->layer_count-1); i++) {
    *(nodes+i+1) = apply_func(mmult(*(n->weights+i),*(nodes+i)),sigmoid);
  }
  return nodes;
}

float diff_node(struct net * n, struct matrix * values, int weight_layer, int weight_row, int weight_column, int node_layer, int node_row) {
  if (weight_layer < 0 || weight_row < 0 || weight_column < 0 || node_layer < 0 || node_row < 0) {
    printf("ERROR: NEGATIVE INDEX VALUES.\n");
  }
  float coefficient = retreive(*(values+node_layer),node_row,0) * (1-retreive(*(values+node_layer),node_row,0));
  /* if we've reached the final relevant layer */
  if (node_layer == weight_layer+1) {
    /* if our node is connected to the weight in question */
    if (node_row == weight_row) {
      return coefficient * retreive(*(values+weight_layer),weight_column,0);
    }
    /* otherwise, our node is not a function of the weight */
    else {
      return 0;
    }
  }
  /* for each node in the previous layer */
  float summation = 0;
  int pre_node;

  for (pre_node = 0; pre_node<(*(n->layer_sizes+node_layer-1)); pre_node++) {
    /* add the differential of (weight between node and previous node)*(value of previous node), given that the weight isn't the one we care about */
    summation = summation + retreive(*(n->weights + node_layer - 1),node_row,pre_node) * diff_node(n,values,weight_layer,weight_row,weight_column,node_layer-1,pre_node);
  }
  return coefficient * summation;
}

float cost_diff(struct net *n, struct matrix *values,struct matrix error,int weight_layer, int weight_row, int weight_column) {
  float sum =0;
  int final_layer_length = error.height;
  for (int p=0; p<final_layer_length; p++) {
    sum += retreive(error,p,0) * diff_node(n,values,weight_layer,weight_row,weight_column,n->layer_count-1,p);
  }
  return -1*sum;
}

struct matrix * gradient(struct net *n, struct matrix * values, struct matrix desired_output) {
  struct matrix error = msub(desired_output,*(values+n->layer_count-1));
  struct matrix * grad = (struct matrix *) malloc((n->layer_count-1)*sizeof(struct matrix));
  int layer, row, col;
  for (layer = 0; layer < (n->layer_count-1); layer++) {
    *(grad+layer) = makematrix(*(n->layer_sizes+layer+1),*(n->layer_sizes+layer));
    for (row = 0; row < *(n->layer_sizes+layer+1);row++) {
      for (col=0; col< *(n->layer_sizes+layer);col++) {
	assign(grad+layer,row,col,cost_diff(n,values,error,layer,row,col));
      }
    }
  }
  return grad;
}

void update_weights(struct net *n, struct matrix input, struct matrix desired_output) {
  struct matrix * grad = gradient(n,node_values(n,input),desired_output);
  int layer;
  for (layer = 0; layer < (n->layer_count-1); layer++) {
    *(n->weights+layer) = msub(*(n->weights+layer),apply_func(*(grad+layer),learning_rate));
  }
}
