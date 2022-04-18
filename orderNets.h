#ifndef __ORDER_NETS__
#define __ORDER_NETS__

#include ece556.h
#include <algorithm> // needed for sort() function
#include <vector>

/* helper function used in the orderNets algorithm to determine
   which net comes first (higher cost) 
*/
int compareCost(net one, net two);


/* function iterates through the nets stored in rst and orders them
   based on cost value (high to low)
*/
void orderNets(routingInst *rst);

#endif
