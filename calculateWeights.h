#ifndef __CALCULATE_WEIGHTS__
#define __CALCULATE_WEIGHTS__

#include "ece556.h"
#include <vector>

/* function calculates the weight of each edge used in the previous routing solution
   uses prior values stored in arrays
   takes special condition for first calculation
*/
void calculateWeights(routingInst *rst, int *overflow, int *history, int *weight);

#endif
