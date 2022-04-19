// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"
#include <sys/time.h>
#include <string.h>
#include "netDec.h"
#include "aStar.h"
#include "orderNets.h"
#include "calculateWeights.h"


// frees the old route from memory between iterations
void releaseRoute(routingInst *rst) {
  for(int i = 0; i < rst->numNets; ++i) {
    for(int j = 0; j < rst->nets[i].nroute.numSegs; ++j) {

      free(rst->nets[i].nroute.segments[j].edges);
    }
    free(rst->nets[i].nroute.segments);
  }
}


int main(int argc, char **argv)
{

 	if(argc!=5){
 		printf("Usage : ./ROUTE.exe -d=# -n=# <input_benchmark_name> <output_file_name> \n");
 		return 1;
 	}

	timeval startTime;
	timeval currentTime;
	int maxTime = 900; // 15 mins in sec
	gettimeofday(&startTime, NULL); // save start time
	

 	int status;
	char *decomp = argv[1];
	char *ordering = argv[2];
	char *inputFileName = argv[3];
 	char *outputFileName = argv[4];

	bool decompOn;
	bool orderingOn;

	if(strcmp(decomp, "-d=1") == 0) {
	  decompOn = true;
	} else {
	  decompOn = false;
	}

	if(strcmp(ordering, "-n=1") == 0) {
	  orderingOn = true;
	} else {
	  orderingOn = false;
	}

 	/// create a new routing instance
 	routingInst *rst = new routingInst;
	
 	/// read benchmark
 	status = readBenchmark(inputFileName, rst);
 	if(status==0){
 		printf("ERROR: reading input file \n");
 		return 1;
 	}

	if(decompOn) { // decompose nets if desired
	  netDec(rst);
	}
	
 	/// run actual routing
 	status = solveRouting(rst);
 	if(status==0){
 		printf("ERROR: running routing \n");
 		release(rst);
 		return 1;
 	}

	int overflow[rst->numEdges];
	int history[rst->numEdges];
	int weights[rst->numEdges];

	// initialize arrays with starting values
	for(int i = 0; i < rst->numEdges; ++i) {
	  overflow[i] = 0;
	  history[i] = 1;
	  weights[i] = 0;
	}

	int iteration = 0;
	int cost;
	int bestRoute = 0;

	printf("1\n");

	if(orderingOn) {
	  gettimeofday(&currentTime, NULL);
	  while((currentTime.tv_sec - startTime.tv_sec) < maxTime) {

	    cost = 0;
	    printf("2\n");
	    calculateWeights(rst, overflow, history, weights);

	    releaseRoute(rst);

	    printf("3\n");
	    orderNets(rst);

	    printf("4\n");
	    solveRoutingAstar(rst);

	    //calculate total cost of solution
	    for(int i = 0; i < rst->numNets; ++i) {
	      cost += rst->nets[i].cost;
	    }

	    printf("Iteration: %d, Cost: %d", iteration, cost);

	    if(iteration == 0) {
	      bestRoute = cost;
	    } else if (cost < bestRoute) {
	      bestRoute = cost;
	      writeOutput(outputFileName, rst);
	    }
	  }
	}


	if(!orderingOn) {
 	/// write the result
	  status = writeOutput(outputFileName, rst);
	  if(status==0){
 		printf("ERROR: writing the result \n");
 		release(rst);
 		return 1;
	  }
	}

 	release(rst);
 	printf("\nDONE!\n");	
 	return 0;
}
