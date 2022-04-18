#include "aStar.h"


int solveRoutingAstar(routingInst *rst)
{

  PointHash::gy = rst->gy;

  //go through all nets and route the pairs of pins
  for( int i = 0; i < rst->numNets; i++ )
  {

    rst->nets[i].nroute.numSegs = rst->nets[i].numPins - 1;

    rst->nets[i].nroute.segments = 
      (segment*)malloc( (rst->nets[i].numPins -1)* sizeof(segment)) ;

    for( int j = 0; j < rst->nets[i].numPins - 1; j++)
    {
      int ret = routeNetAstar(rst, i, j, j+1 );
      if( ret == EXIT_FAILURE )
      {
        fprintf( stderr, "failed to route net.\n");
        return EXIT_FAILURE;
      }
    }
  }
  
  return EXIT_SUCCESS;
}


int routeNetAstar(routingInst *rst,int netInd ,int SpinInd,int TpinInd)
{

  unordered_map<point, Node*, PointHash> openSet;
  priority_queue<Node*, vector<Node*>, CompareNodeCost> openSet_pq;
  unordered_map<point, Node*, PointHash> closedSet;

  //create target node
  Node nT;
  nT.loc = rst->nets[netInd].pins[TpinInd];

  
  Node* nS = new Node;
  nS->loc = rst->nets[netInd].pins[SpinInd]; 
  nS->parent = NULL;
  nS->distFromS = 0;

  int dx = abs(nS->loc.x - nT.loc.x);
  int dy = abs(nS->loc.y - nT.loc.y);

  nS->distToT = dx + dy;

  int approxElems = dx * dy;
  openSet.reserve( approxElems );
  closedSet.reserve( approxElems );


  openSet.insert( std::make_pair(nS->loc, nS) );
  openSet_pq.push( nS );



  while( !openSet_pq.empty() )
  {

    auto current = openSet_pq.top();
    openSet_pq.pop();


    auto it = openSet.find( current->loc );
    if( it == openSet.end() )
    {

      continue;
    }

    //if we are at the target, retrace our path
    if( current->loc == nT.loc )
    {
      int ret = retrace(rst, nS, current, netInd, SpinInd);


      deleteMap( closedSet );
      deleteMap( openSet );

      return ret;
    }


    closedSet.insert( std::make_pair(current->loc, current) );
    openSet.erase(it);
    
    vector<Node> neighbors;
    
    getNeighbors(rst, neighbors, current, nT );

    for( auto neighbor = neighbors.begin(); neighbor != neighbors.end();
              neighbor++ )
    {

      auto elem = closedSet.find( neighbor->loc );
      if( elem != closedSet.end() )
      {
        //already in closed, don't add
        continue;
      }

      //check if it's in the openSet.  if the score is better, update the 
      //existing node in the openSet, and add it to the priority queue again?
      elem = openSet.find( neighbor->loc );
      if( elem != openSet.end() )
      {
        //need to update element in openSet, and add to pq again (shouldn't cause problems?)
        //the parent, distFromS, G, edgeCap, edgeUtil, 
        //and F fields in the node have to be updated
        elem->second->parent = current;
        elem->second->distFromS = current->distFromS + 1;
        elem->second->edgeID = getEdgeID(rst, current->loc, neighbor->loc);
        elem->second->edgeCap = rst->edgeCaps[elem->second->edgeID-1];
        elem->second->edgeUtil = rst->edgeUtils[elem->second->edgeID-1];
        calcG( *elem->second );
        calcF( *elem->second );

        openSet_pq.push( elem->second );
      } else
      {
        //elem == openSet.end(), so it is not currently in the openSet
        //we need to make a new Node here so that we can push its address
        //into the openSet because the vector one will go away
        Node *tmpN = new Node(*neighbor);
        openSet_pq.push( tmpN );
        openSet.insert( std::make_pair(tmpN->loc, tmpN));
      }
    }

  }

  //if we reach this point, we didn't find the Target node
  printf( "couldn't find route for netInd: %d\tSpinInd: %d\tTpinInd: %d\n",
      netInd, SpinInd, TpinInd );
  return EXIT_FAILURE;
}

int retrace(routingInst *rst, Node* nS, Node* current, int netInd, int segInd) 
{
  Node* tmp = current;

  /* number of edges is simply the length from the starting node */
  rst->nets[netInd].nroute.segments[segInd].p2 = current->loc;
  rst->nets[netInd].nroute.segments[segInd].p1 = nS->loc;

  //printf("\nstart loc: (%d, %d)\tend loc: (%d, %d)\n",
   //         nS->loc.x, nS->loc.y, current->loc.x, current->loc.y );


  
  rst->nets[netInd].nroute.segments[segInd].numEdges = current->distFromS;
  rst->nets[netInd].nroute.segments[segInd].edges = 
    (int*)malloc(sizeof(int) * current->distFromS);

  if(rst->nets[netInd].nroute.segments[segInd].edges == NULL)
  {
    fprintf(stderr, "Couldn't malloc for segments[segInd].edges pointer.\n");
    return EXIT_FAILURE;
  }

  int i  = 0;
  double costSum = 0;
  while( tmp->parent != NULL )
  {


    int edgeID = getEdgeID( rst, tmp->loc.x, tmp->loc.y, 
        (*tmp->parent).loc.x, (*tmp->parent).loc.y );

    rst->nets[netInd].nroute.segments[segInd].edges[i] = edgeID;

    costSum += tmp->G - 1;


    rst->edgeUtils[edgeID-1]++;

    tmp =  tmp->parent;
    i++;
  }
  prevAvgCost = (prevAvgCost + costSum / (double)i ) / 2.0;

  return EXIT_SUCCESS;
}

void calcG( Node& n ) 
{

  int diff = n.edgeUtil - n.edgeCap;
  double h_cost = MAX_HEUR_COST / (1.0 + exp(-K_OVF_MULT* (double)diff )); 
  n.G = n.distFromS * unitDist + h_cost;
}

void calcF( Node& n ) 
{
  n.F = n.G + n.distToT * (1.0 + prevAvgCost);
}

int m_dist( const Node& n1, const Node& n2)
{
  return abs(n1.loc.x - n2.loc.x) + abs(n1.loc.y - n2.loc.y);
}

void getNeighbors(routingInst *rst, vector<Node>& neighbors,
                    Node* current, Node T)
{
  Node tmp;
  int leftX, rightX, topY, botY;


  leftX = current->loc.x - 1;
  rightX = current->loc.x + 1;
  topY = current->loc.y + 1;
  botY = current->loc.y - 1;

  tmp.distFromS = current->distFromS +1;
  tmp.parent = current;
  

  if( leftX >= 0 )
  {
    tmp.loc = current->loc;
    tmp.loc.x = leftX;
    tmp.distToT = m_dist(tmp, T);
    tmp.edgeID = getEdgeID(rst, tmp.loc.x, tmp.loc.y, tmp.loc.x + 1, tmp.loc.y);
    /* use edgeID to find specific edgeCap and edgeUtil */
    tmp.edgeCap = rst->edgeCaps[tmp.edgeID-1];
    tmp.edgeUtil = rst->edgeUtils[tmp.edgeID-1];
    calcG(tmp);
    calcF(tmp);
    neighbors.push_back(tmp);
  } 

  if( rightX < rst->gx )
  {
    tmp.loc = current->loc;
    tmp.loc.x = rightX;
    tmp.distToT = m_dist(tmp, T);
    tmp.edgeID = getEdgeID(rst, tmp.loc.x, tmp.loc.y, tmp.loc.x -1, tmp.loc.y);
    tmp.edgeCap = rst->edgeCaps[tmp.edgeID-1];
    tmp.edgeUtil = rst->edgeUtils[tmp.edgeID-1];
    calcG(tmp);
    calcF(tmp);
    neighbors.push_back(tmp);
  }

  if( topY < rst->gy )
  {
    tmp.loc = current->loc;
    tmp.loc.y = topY;
    tmp.distToT = m_dist(tmp, T);
    tmp.edgeID = getEdgeID(rst, tmp.loc.x, tmp.loc.y, tmp.loc.x, tmp.loc.y - 1);
    tmp.edgeCap = rst->edgeCaps[tmp.edgeID-1];
    tmp.edgeUtil = rst->edgeUtils[tmp.edgeID-1];
    calcG(tmp);
    calcF(tmp);
    neighbors.push_back(tmp);
  }

  if( botY >= 0 )
  {
    tmp.loc = current->loc;
    tmp.loc.y = botY;
    tmp.distToT = m_dist(tmp, T);
    tmp.edgeID = getEdgeID(rst, tmp.loc.x, tmp.loc.y, tmp.loc.x, tmp.loc.y + 1);
    tmp.edgeCap = rst->edgeCaps[tmp.edgeID-1];
    tmp.edgeUtil = rst->edgeUtils[tmp.edgeID-1];
    calcG(tmp);
    calcF(tmp);
    neighbors.push_back(tmp);
  }
}
//free memory from a map
void deleteMap( unordered_map<point, Node*, PointHash>& group )
{
  for( auto it = group.begin(); it != group.end(); it++ )
  {
    //free element
    delete (it->second);
  }
}
