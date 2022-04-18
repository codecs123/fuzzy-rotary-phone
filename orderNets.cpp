/* used to order the nets during RRR */
#include "orderNets.h"


int compareCost(net one, net two) {
   if(one.cost >= two.cost) {
      return 1;
   } else {
      return 0;
   }
}


void orderNets(routingInst *rst) {
   // uses built in sort function and cost comparing algorithm
   sort(rst->nets, rst->nets + rst->numNets, compareCost);
}
