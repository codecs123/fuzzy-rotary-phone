#ifndef ASTAR_H
#define ASTAR_H

#include "ece556.h"
#include <type_traits>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <math.h>

/* parts of namespace used */
using std::vector;
using std::pair;
using std::unordered_map;
using std::priority_queue;




const double MAX_HEUR_COST = 20.0;
const double K_OVF_MULT = 1.0;
const int K_EDGE_CAPS = 1;
const int K_EDGE_UTIL = 1;

/* constants */
const double unitDist = 0.99;

/* used to adjust the estimate for cost to the target node so that it
 * is closer to the actual cost, expanding fewer nodes
 */
static double prevAvgCost = 0;



namespace{

struct Node
{
  point loc;     
  Node* parent;  
  int edgeID;   
  int distFromS; 
  int distToT;   
  int edgeUtil;  
  int edgeCap;  

  double F;    
  double G;   


  //STRUCT FUNCTIONS
  Node() : loc(-1, -1), parent(NULL), edgeID(0), distFromS(-1),
            distToT(-1), edgeUtil(0), edgeCap(0), F(-1), G(-1)
  {
 
  }

  Node( const Node& other ): loc(other.loc), parent(other.parent), edgeID(other.edgeID),
                      distFromS(other.distFromS), distToT(other.distToT),
                      edgeUtil(other.edgeUtil), edgeCap(other.edgeCap),
                      F(other.F), G(other.G) {}

  bool operator==(const Node& n) const
  {
    return (loc == n.loc);
  }
  bool operator!=(const Node&n) const
  {
    return !(loc == n.loc);
  }
};


/* used to compare elements in the open set */
class CompareNodeCost
{
  public:
    bool operator()( const Node* n1, const Node* n2 ) const
    {
      //returns true if e2 has less cost than e1
      return n1->F > n2->F;
    }
};      


class PointHash
{
  public:
    std::size_t operator()(const point& pt) const
    {
      return pt.y + gy * pt.x;
    }

  //same as rst->gy, used to calc unique index into 2d array
    static int gy;
};

int PointHash::gy = 0;

} //close namespace


void calcF( Node& n ) ;
void calcG( Node& n ) ;

void getNeighbors(routingInst *rst, vector<Node>& neighbors,
                    Node* current, Node T);

int retrace(routingInst *rst, Node* nS, Node* current, int netInd, int segInd) ;

int solveRoutingAstar(routingInst *rst);

int routeNetAstar(routingInst *rst,int ,int ,int);

//returns the manhattan distance between 2 nodes
int m_dist( const Node& n1, const Node& n2);

void deleteMap( unordered_map<point, Node*, PointHash>& group );
#endif
