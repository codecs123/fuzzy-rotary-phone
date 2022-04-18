#include "calculateWeights.h"

void calculateWeights(routingInst *rst, int *overflow, int *history, int *weight) {

   bool used[rst->numEdges];
   int edge;

   // array identifying which edges have already been processed this iteration
   for(int i = 0; i < rst->numEdges; ++i) {
      used[i] = false;
   }

   for (int i = 0; i < rst->numNets; ++i) {
      for (int j = 0; j < rst->nets[i].nroute.numSegs; ++j) {
         rst->nets[i].cost = 0; // set cost of net to 0
         for (int k = 0; k < rst->nets[i].nroute.segments[j].numEdges; ++k) {
	 // iterate through all edges used
	   edge = rst->nets[i].nroute.segments[j].edges[k];
	   if(!used[edge]) { // check if calculations have already been made
              overflow[edge] = rst->edgeUtils[edge] - rst->edgeCaps[edge];
	      if(overflow[edge] < 0) { // calculate overflow
                overflow[edge] = 0;
	      }
              if(overflow[edge] > 0) { // calculate history adjustment
		history[edge] += 1;
              }
	      weight[edge] = overflow[edge] * history[edge]; // calculate weight
           }
	   rst->nets[i].cost += weight[edge]; // add weight to cost of net
	 }
      }
   }

}
