#include <stdio.h>
#include <stdlib.h>
#include "neural_net.h"




int main() {

  FILE *rf = fopen("/dev/random","r");

  struct matrix up = makematrix(4,1);
  assign(&up,0,0,1.0);
  assign(&up,1,0,0.9);
  assign(&up,2,0,0.2);
  assign(&up,3,0,0.1);

  struct matrix up_output = makematrix(2,1);
  assign(&up_output,0,0,1.0);
  assign(&up_output,1,0,0.0);

  struct matrix down = makematrix(4,1);
  assign(&down,0,0,0.0);
  assign(&down,0,1,0.5);
  assign(&down,0,2,0.7);
  assign(&down,0,3,1.0);

  struct matrix down_output = makematrix(2,1);
  assign(&down_output,0,0,0.0);
  assign(&down_output,1,0,1.0);

  int * dims = malloc(4*sizeof(int));
  *(dims) = 4;
  *(dims+1) = 3;
  *(dims+2) = 3;
  *(dims+3) = 2;

  
  struct net n = makenet(dims,4);

  struct matrix * values = node_values(&n,up);
  struct matrix error = msub(up_output,*(values+n.layer_count-1));
  struct matrix * g = gradient(&n,values,up_output);


  float correct = 0;
  float total = 0;
  for (int i=0; i<50000; i++) {
    if (max(calculate(n,up)) == 0) {
      correct++;
    }
    total++;
    if (max(calculate(n,down)) == 1) {
      correct++;
    }
    total++;
  }

  printf("accuracy: %f\n",(float)correct/(float)total);
  for (int i=0; i<10000; i++) {
    update_weights(&n,down,down_output);
    update_weights(&n,up,up_output);
  }

  correct=0;
  total=0;
  for (int i=0; i<50000; i++) {
    if (max(calculate(n,up)) == 0) {
      correct++;
    }
    total++;
    if (max(calculate(n,down)) == 1) {
      correct++;
    }
    total++;
  }

  printf("accuracy: %f\n",(float)correct/(float)total);
  
  return 0;
}
