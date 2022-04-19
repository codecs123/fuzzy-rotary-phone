#include "netDec.h"

void netDec(routingInst *rst){

    /* for loop to cycle through nets */
    int totalNets = rst->numNets;

    /* array to hold points close to points found in order */
    point *closePoint = NULL;

    /* point variable to hold closest determined point */
    point nearPoint;

    for (int i=0; i<totalNets; i++){

        /* assign starting point values */
        point startIn = rst -> nets[i].pins[0];
        point startIn2 = rst -> nets[i].pins[1];

        /* find the shortest x distance from point 1 to point 2 */
        int xShort = abs(rst->nets[i].pins[1].x - rst->nets[i].pins[0].x);

        /* find the shortest y distance from point 1 to point 2 */
        int yShort = abs(rst->nets[i].pins[1].y - rst->nets[i].pins[0].y);

        /* sum shortest distances from xShort and yShort to find total shortest distance */
        int shortestDis = xShort + yShort;

        /* loop through other pins, if distance from start is < shortestDis, update shortestDis */

        /* varibale to hold total pins */
        int totalPins = rst -> nets[i].numPins;

        for (int m=0; m<totalPins; m++){
            for (int n=0; n<totalPins; n++){

                /* shortest x distance from next point */
                int tempXdist = abs(rst->nets[i].pins[m].x - rst->nets[i].pins[n].x);

                /* shortest x distance from next point */
                int tempYdist = abs(rst->nets[i].pins[m].y - rst->nets[i].pins[n].y);

                /* shortest total distance from next point */
                int tempTotal = tempXdist + tempYdist;

                /* compare tempTotal to initial shortest distance and update if shorter */
                if((tempTotal < shortestDis) && (tempTotal != 0)){

                    /* assign first starting point */
                    startIn = rst -> nets[i].pins[m];

                    /* assign next point */
                    startIn2 = rst -> nets[i].pins[n];

                    /* replace shortestDis with tempTotal */
                    shortestDis = tempTotal;
                }

            }
        }

        /* insert points startIn & startIn2 into point array closePoint for use in future compairison */

        /* Adjust size of closePoint to fit values of pins */
        int newSize = sizeof(point) * rst->nets[i].numPins;
        closePoint = (point*)realloc(closePoint, newSize);

        /* place startIn into closePoint */
        closePoint[0] = startIn;

        /* place startIn2 into closePoint */
        closePoint[1] = startIn2;

        /* varibale to hold total pins for next loop*/
	int totalPins2 = rst -> nets[i].numPins - 2;
	
        for(int p = 0; p < totalPins2; p++){

            /* create a very distant point for compairison to all other points */
            point farPoint;
            farPoint.x = 2000000000;
            farPoint.y = 2000000000;

            /* set starting distance to large value for use in compairisons */
            int farDist = 2000000000;

            /* place starting values into compairison points from closePoint array */
            point comp1 = closePoint[p];
            point comp2 = closePoint[p+1];

            /* sort through all other pins in the current net to see if any are closer, if so set farDist value equal to new shortest value */
            for(int q = 0; q < totalPins; q++){

                /* temp point to hold values of point in current loop */
                point curPoint = rst->nets[i].pins[q];

                /* Determine if point is already in closePoint array */
                bool doesHave = false;
                int p2 = p+2;

                /* for loop to iterate through closePoint array */
                for(int r = 0; r < p2; r++){

                    /* if points match, set doesHave equal to "true" */
                    if((curPoint.y == closePoint[r].y) && (curPoint.x == closePoint[r].x)){

                        /* set does have equal to true */
                        doesHave = true;
                    }

                }

                /* if the current point is not in the array then assign x,y values based on comp1 & comp2 and curPoint */
                if(doesHave == false){

                    /* set y values for use in determining distance */
                    int yCur = curPoint.y;
                    int yComp1 = comp1.y;
                    int yComp2 = comp2.y;

                    /* set x values for use in determining distance */
                    int xCur = curPoint.x;
                    int xComp1 = comp1.x;
                    int xComp2 = comp2.x;

                    /*-----------------------------------------------------------------*/
                    /* determine if current point is between comp1 and comp2 x values */
                    bool betweenX = false;

                    if((xComp1 <= xCur) && (xCur <= xComp2)){
                        betweenX = true;
                    }
                    if((xComp2 <= xCur) && (xCur <= xComp1)){
                        betweenX = true;
                    }

                    /* determine if current point is between comp1 and comp2 y values */
                    bool betweenY = false;

                    if((yComp1 <= yCur) && (yCur <= yComp2)){
                        betweenY = true;
                    }
                    if((yComp2 <= yCur) && (yCur <= yComp1)){
                        betweenY = true;
                    }

                    /*-----------------------------------------------------------------*/
                    /* DETERMINE WHERE CURRENT X LIES IN RELATION TO OTHER X POINTS */
                    /* determine where x lies if not between xComp1 and xComp2 */

                    /* if xCur < xComp1 < xComp2 */
                    bool x_lt_x1x2 = false;
		    bool x_lt_x2x1 = false;

                    if((xComp1 <= xComp2) && (xCur < xComp1)){
                        x_lt_x1x2 = true;
                    }

                    /* if xCur < xComp2 < xComp1 */
                    // bool x_lt_x2x1 = false;

                    else if((xComp2 <= xComp1) && (xCur < xComp2)){
                        x_lt_x2x1 = true;
                    }

                    /* if xComp2 < xComp1 < xCur */
                    bool x_gt_x1x2 = false;
		    bool x_gt_x2x1 = false;

                    if((xComp1 >= xComp2) && (xCur > xComp1)){
                        x_gt_x1x2 = true;
                    }

                    /* if xComp1 < xComp2 < xCur */
                    // bool x_gt_x2x1 = false;

                    else if((xComp2 >= xComp1) && (xCur > xComp2)){
                        x_gt_x2x1 = true;
                    }
                    /*-----------------------------------------------------------------*/

                    /*-----------------------------------------------------------------*/
                    /* DETERMINE WHERE CURRENT Y LIES IN RELATION TO OTHER Y POINTS */
                    /* determine where y lies if not between yComp1 and yComp2 */

                    /* if yCur < yComp1 < yComp2 */
                    bool y_lt_y1y2 = false;
		    bool y_lt_y2y1 = false;

                    if((yComp1 <= yComp2) && (yCur < yComp1)){
                        y_lt_y1y2 = true;
                    }

                    /* if yCur < yComp2 < yComp1 */
                    // bool y_lt_y2y1 = false;

                    else if((yComp2 <= yComp1) && (yCur < yComp2)){
                        y_lt_y2y1 = true;
                    }

                    /* if yComp2 < yComp1 < yCur */
                    bool y_gt_y1y2 = false;
		    bool y_gt_y2y1 = false;

                    if((yComp1 >= yComp2) && (yCur > yComp1)){
                        y_gt_y1y2 = true;
                    }

                    /* if yComp1 < yComp2 < yCur */
                    // bool y_gt_y2y1 = false;

                    else if((yComp2 >= yComp1) && (yCur > yComp2)){
                        y_gt_y2y1 = true;
                    }
                    /*-----------------------------------------------------------------*/

                    /* int var to hold shortest distance */
                    int tempTotal2;

                    /* int values to determine distinces between points */
                    if(betweenX){

                        /* absolute distance between yCur abd yComp1 */
                        int c_to_c1y = abs(yComp1 - yCur);
                        /* absolute distance between yCur abd yComp2 */
                        int c_to_c2y = abs(yComp2 - yCur);

                        /* assign new shortest distance value */
                        /* if current y value is closer to yComp2 then set tempTotal2 equal to distance between yCur and yComp2 */
                        if (c_to_c1y > c_to_c2y){
                            tempTotal2 = c_to_c2y;
                        }
                        if (c_to_c1y <= c_to_c2y){
                            tempTotal2 = c_to_c1y;
                        }
                        if(tempTotal2 < farDist){
                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }

                    }
                    else if(betweenY){

                        /* absolute distance between xCur abd xComp1 */
                        int c_to_c1x = abs(xComp1 - xCur);
                        /* absolute distance between xCur abd xComp2 */
                        int c_to_c2x = abs(xComp2 - xCur);

                        /* assign new shortest distance value */
                        /* if current x value is closer to xComp2 then set tempTotal2 equal to distance between xCur and xComp2 */
                        if (c_to_c1x > c_to_c2x){
                            tempTotal2 = c_to_c2x;
                        }
                        if (c_to_c1x <= c_to_c2x){
                            tempTotal2 = c_to_c1x;
                        }
                        if(tempTotal2 < farDist){
                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 1-----------------------------------------------*/
                    else if(x_lt_x1x2 && y_gt_y1y2){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp1-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 1-----------------------------------------------*/

                    /*-----------------sit 2-----------------------------------------------*/
                    else if(x_gt_x1x2 && y_gt_y1y2){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp1-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 2-----------------------------------------------*/

                    /*-----------------sit 3-----------------------------------------------*/
                    else if(x_lt_x1x2 && y_lt_y1y2){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp1-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 3-----------------------------------------------*/

                    /*-----------------sit 4-----------------------------------------------*/
                    else if(x_gt_x1x2 && y_lt_y1y2){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp1-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 4-----------------------------------------------*/

                    /*-----------------sit 5-----------------------------------------------*/
                    else if(x_gt_x2x1 && y_gt_y2y1){

                        int xShort2 = abs(xComp2-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 5-----------------------------------------------*/

                    /*-----------------sit 6-----------------------------------------------*/
                    else if(x_lt_x2x1 && y_gt_y2y1){

                        int xShort2 = abs(xComp2-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 6-----------------------------------------------*/

                    /*-----------------sit 7-----------------------------------------------*/
                    else if(x_lt_x2x1 && y_lt_y2y1){

                        int xShort2 = abs(xComp2-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 7-----------------------------------------------*/

                    /*-----------------sit 8-----------------------------------------------*/
                    else if(x_gt_x2x1 && y_lt_y2y1){

                        int xShort2 = abs(xComp2-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 8-----------------------------------------------*/

                    /*-----------------sit 9-----------------------------------------------*/
                    else if(x_gt_x1x2 && y_gt_y2y1){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 9-----------------------------------------------*/

                    /*-----------------sit 10-----------------------------------------------*/
                    else if(x_lt_x1x2 && y_gt_y2y1){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 10-----------------------------------------------*/

                    /*-----------------sit 11-----------------------------------------------*/
                    else if(x_lt_x1x2 && y_lt_y2y1){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 11-----------------------------------------------*/

                    /*-----------------sit 12-----------------------------------------------*/
                    else if(x_gt_x1x2 && y_lt_y2y1){

                        int xShort2 = abs(xComp1-xCur);
                        int yShort2 = abs(yComp2-yCur);

                        tempTotal2 = xShort2 + yShort2;

                        if(tempTotal2 < farDist){

                            farDist = tempTotal2;
                            nearPoint = curPoint;
                        }
                    }
                    /*-----------------sit 12-----------------------------------------------*/

		    /*-----------------sit 13-----------------------------------------------*/
		    else if(x_lt_x2x1 && y_lt_y1y2) {

		      int xShort2 = abs(xComp2-xCur);
		      int yShort2 = abs(yComp1-xCur);

		      tempTotal2 = xShort2 + yShort2;

		      if(tempTotal2 < farDist) {

			farDist = tempTotal2;
			nearPoint = curPoint;
		      }
		    }
		    /*-----------------sit 13-----------------------------------------------*/

		    /*-----------------sit 14-----------------------------------------------*/
		    else if(x_lt_x2x1 && y_gt_y1y2) {

		      int xShort2 = abs(xComp2-xCur);
		      int yShort2 = abs(yComp1-xCur);

		      tempTotal2 = xShort2 + yShort2;

		      if(tempTotal2 < farDist) {

			farDist = tempTotal2;
			nearPoint = curPoint;
		      }
		    }
		    /*-----------------sit 14-----------------------------------------------*/

		    /*-----------------sit 15-----------------------------------------------*/
		    else if(x_gt_x2x1 && y_lt_y1y2) {

		      int xShort2 = abs(xComp2-xCur);
		      int yShort2 = abs(yComp1-xCur);

		      tempTotal2 = xShort2 + yShort2;

		      if(tempTotal2 < farDist) {

			farDist = tempTotal2;
			nearPoint = curPoint;
		      }
		    }

		    /*-----------------sit 16-----------------------------------------------*/
		    else if(x_gt_x2x1 && y_gt_y1y2) {

		      int xShort2 = abs(xComp2-xCur);
		      int yShort2 = abs(yComp1-xCur);

		      tempTotal2 = xShort2 + yShort2;

		      if(tempTotal2 < farDist) {

			farDist = tempTotal2;
			nearPoint = curPoint;
		      }
		    }

                /* if (doeshave) */
                }
            /* pins loop */
            }

            /* move closest point into list */
            int spot  = p+2;
            closePoint[spot] = nearPoint;

        /* point loop */
        }

        /* loop to add newfound closest values into rst */
        for(int j = 0; j < totalPins; j++){

            /* add new y vaules into rst */
            rst->nets[i].pins[j].y = closePoint[j].y;

            /* add new x vaules into rst */
            rst->nets[i].pins[j].x = closePoint[j].x;
        }


    /* initial loop */
    }

/* reset closePoint list */
free(closePoint);
}
