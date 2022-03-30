// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"
#include <iostream>
#include <fstream>
#include <string>

int readBenchmark(const char *fileName, routingInst *rst){

    if(fileName == NULL || rst == NULL) {
        return 0;
    }
    FILE* inputFile;
    inputFile = fopen(fileName, "r");

    fscanf(inputFile, "%*s %d %d ", &(rst->gx), &(rst->gy));

    fscanf(inputFile, "%*s %d", &(rst->cap));
    
    fscanf(inputFile, "%*s %*s %d", &(rst->numNets));

    rst->nets = (net*)malloc(rst->numNets * sizeof(net));

    for (int i = 0; i < rst->numNets; ++i) { // create array of nets

        rst->nets[i].id = i;
        
        fscanf(inputFile, "%*s %d", &(rst->nets[i].numPins));

        rst->nets[i].pins = (point*)malloc(rst->nets[i].numPins * sizeof(point));

        for (int j = 0; j < rst->nets[i].numPins; ++j) { // create array of points
            fscanf(inputFile, "%d %d", &(rst->nets[i].pins[j].x), &(rst->nets[i].pins[j].y));
        }
    }

    int blocks;
    fscanf(inputFile, "%d", &blocks);

    int numEdges = rst->gx*(rst->gy - 1) + rst->gy*(rst->gx - 1);

    // EdgeID formulae
    // Horizontal: min(xlocal) + ylocal * xgrid
    // Vertical: min(ylocal) * (xgrid) + xlocal + ygrid * (xgrid - 1)


    rst->edgeCaps = (int*)malloc(numEdges * sizeof(int));
    rst->edgeUtils = (int*)malloc(numEdges * sizeof(int));

    for(int i = 0; i < numEdges; ++i) {
        rst->edgeCaps[i] = rst->cap;
        rst->edgeUtils[i] = 0;
    }

    for (int i = 0; i < blocks; ++i) { // find IDs of blockages and update cap

        int bx1;
        int bx2;
        int by1;
        int by2;
        int cap;
        int edg;

        //inputFile >> bx1 >> by1 >> bx2 >> by2 >> cap;
        fscanf(inputFile, "%d %d %d %d %d", &bx1, &by1, &bx2, &by2, &cap);
        printf("block \n");
        if (bx1 != bx2) {
            edg = (bx1 + bx2)/2 + by1 * (rst->gx - 1);
        } else {
            edg = ((by1 + by2)/2) * rst->gx + bx1 + rst->gy * (rst->gx - 1);
        }
        printf("edge %d \n", edg);

        rst->edgeCaps[edg] = cap;
    }

    fclose(inputFile);

    return 1;
}

int solveRouting(routingInst *rst){
    int x1, y1;

    for(int i = 0; i < rst->numNets; i++)
    {
        rst->nets[i].nroute.numSegs = rst->nets[i].numPins - 1;
        rst->nets[i].nroute.segments = (segment*)malloc(rst->nets[i].nroute.numSegs*sizeof(segment));
        if(rst->nets[i].nroute.segments == NULL)
        {
            fprintf(stderr, "Memory not allocated for route segments \n");
            return 0;
        }


        for(int j = 0; j < rst->nets[i].nroute.numSegs; j++)
        {

            rst->nets[i].nroute.segments[j].p1 = rst->nets[i].pins[j];
            rst->nets[i].nroute.segments[j].p2 = rst->nets[i].pins[j+1];


            x1 = rst->nets[i].pins[j+1].x - rst->nets[i].pins[j].x;
            y1 = rst->nets[i].pins[j+1].y - rst->nets[i].pins[j].y;

            rst->nets[i].nroute.segments[j].numEdges =
                    abs(x1) + abs(y1);

            rst->nets[i].nroute.segments[j].edges =
                    (int*)malloc(rst->nets[i].nroute.segments[j].numEdges*sizeof(int));

            if( rst->nets[i].nroute.segments[j].edges == NULL)
            {
                fprintf(stderr, "Memory not allocated for edge array.\n");
                return 0;
            }

        }
    }

    return 1;
}

/* Function to retrive edge endpoints for writeOutput */
void edgeEnds(point *p1, point *p2, routingInst *rst, int iD ){

    /* Variable to determine if edge is vertical or horizontal */
    int edgeID_VorH = iD - ((rst -> gy) * ((rst -> gx) - 1));

    /* If edge is horizontal */
    if(edgeID_VorH <= 0){

        /* Assign values to p1,p2 y values */
        p1->y = ((iD - 1) / ((rst->gx) - 1));
        p2->y = (p1->y);

        /* Assign values to p1,p2 x values, variables switched for compilation order */
        p2->x = iD - (((rst->gx) - 1)*(p2->y));
        p1->x = (p2->x) - 1;
    }
        /* If edge is vertical */
    else{

        /* Assign values to p1,p2 x values */
        p1->x = ((edgeID_VorH - 1) / ((rst->gy) - 1));
        p2->x = p1->x;

        /* Assign values to p1,p2 y values */
        p2->y = edgeID_VorH - ((p2->x) * (rst->gy - 1));
        p1->y = (p2->y) - 1;
    }
}

int writeOutput(const char *outRouteFile, routingInst *rst){

    /* check for null parameters */
    if( outRouteFile == NULL || rst == NULL )
    {
        printf("Argument file or routingInst is null");
        return 0;
    }

    FILE* outFile;

    /* Open output file to begin writing */
    outFile = fopen(outRouteFile, "w");

    /* Instantiate temp points */
    point p1;
    point p2;

    /* Instantiate temp ex points and set to initial values */
    point p1Ex;
    point p2Ex;
    /* set x values */
    p1Ex.x = 0;
    p2Ex.x = 0;
    /* set y values */
    p1Ex.y = 0;
    p2Ex.y = 0;

    /* Instantiate temp point values */
    int xDir = 0; /* used to determine direction */
    int yDir = 0; /* used to determine direction */
    int xEx = 0; /* used to hold previous value */
    int yEx = 0; /* used to hold previous value */

    /* Parent loop for number of nets in routingInst*/
    for (int i = 0; i < rst->numNets; i++){

        /* Print net number */
        fprintf(outFile,"n%d \n",i);

        for (int m = 0; m < rst->nets[i].nroute.numSegs; m++){

            for (int n = 0; n < rst->nets[i].nroute.segments[m].numEdges; n++) {

                edgeEnds(&p1, &p2, rst, rst->nets[i].nroute.segments[m].edges[n]);

                /* variable to hold number of edges */
                int edgeAmount = rst->nets[i].nroute.segments[m].numEdges;

                /* Print case for if only one edge is used in route */
                if (edgeAmount == 1) {
                    fprintf(outFile, "(%d,%d)-(%d,%d)", p1.x, p1.y, p2.x, p2.y);
                }

                /* If statement to determine if this is the first segment */
                if (n == 0) {
                    p2Ex = p2;
                    p1Ex = p1;
                    xEx = abs(p1.x - p2.x);
                    yEx = abs(p1.y - p2.y);
                }
                    /* if this is not the first segment */
                else {
                    xDir = abs(p1.x - p2.x); /* if this has a non zero value then there is movment in the x direction */
                    yDir = abs(p1.y - p2.y); /* if this has a non zero value then there is movment in the y direction */

                    /* Start evaluating if path is horizontal in another horizontal */
                    if ((xDir > 0) && (xEx > 0)) {
                        if (p2.x == p1Ex.x) {
                            p1Ex = p1;
                        } else if (p1.x == p1Ex.x) {
                            p1Ex = p2;
                        } else if (p2.x == p2Ex.x) {
                            p2Ex = p1;
                        } else if (p1.x == p2Ex.x) {
                            p2Ex = p2;
                        }
                        /* if we have reached the last edge */
                        if ((edgeAmount - 1) == n) {
                            /* print to outFile */
                            fprintf(outFile, "(%d,%d)-(%d,%d)\n", p1Ex.x, p1Ex.y, p2Ex.x, p2Ex.y);
                        }
                    }

                        /* Start evaluating if path is vertical */
                    else if ((yDir > 0) && (xEx > 0)) {

                        /* print to outFile  */
                        fprintf(outFile, "(%d,%d)-(%d,%d)\n", p1Ex.x, p1Ex.y, p2Ex.x, p2Ex.y);

                        /* set values */
                        xEx = abs(p2.x - p1.x);
                        yEx = abs(p2.y - p1.y);
                        p1Ex = p1;
                        p2Ex = p2;

                        /* if we have reached the last edge */
                        if ((edgeAmount - 1) == n) {
                            /* print to outFile */
                            fprintf(outFile, "(%d,%d)-(%d,%d)\n", p1Ex.x, p1Ex.y, p2Ex.x, p2Ex.y);
                        }
                    } else if ((yDir > 0) && (yEx > 0)) {

                        if (p2.y == p1Ex.y) {
                            p1 = p1Ex;
                        } else if (p1.y == p1Ex.y) {
                            p1Ex = p2;
                        } else if (p2.y == p2Ex.y) {
                            p2Ex = p1;
                        } else if (p1.y == p2Ex.y) {
                            p2Ex = p2;
                        }
                        /* if we have reached the last edge */
                        if ((edgeAmount - 1) == n) {
                            fprintf(outFile, "(%d,%d)-(%d,%d)\n", p1Ex.x, p1Ex.y, p2Ex.x, p2Ex.y);
                        }
                    }
                        /* Start evaluating if path is vertical into horizontal */
                    else if ((xDir > 0) && (yEx > 0)) {

                        /* print to outFile  */
                        fprintf(outFile, "(%d,%d)-(%d,%d)\n", p1Ex.x, p1Ex.y, p2Ex.x, p2Ex.y);

                        /* set values */
                        xEx = abs(p2.x - p1.x);
                        yEx = abs(p2.y - p1.y);
                        p1Ex = p1;
                        p2Ex = p2;

                        /* if last edge in segment, print */
                        if ((edgeAmount - 1) == n) {
                            fprintf(outFile, "(%d,%d)-(%d,%d)\n", p1Ex.x, p1Ex.y, p2Ex.x, p2Ex.y);
                        }
                    }
                }
            }
        }
        fprintf(outFile, "!\n");
    }
    return 1;
}


int release(routingInst *rst){
    // start at bottom work back to top
    for (int i = 0; i < rst->numNets; ++i) { // parse through each net
        for (int j = 0; j < rst->nets[i].nroute.numSegs; ++j) { // each segment of route
            free(rst->nets[i].nroute.segments[j].edges);
        }
        free(rst->nets[i].nroute.segments);

        free(rst->nets[i].pins);
    }

    free(rst->nets);

    free(rst->edgeCaps);

    free(rst->edgeUtils);

    free(rst);

    return 1;
}
