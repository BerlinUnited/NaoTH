#ifndef _VoronoiCellGraph_h_
#define _VoronoiCellGraph_h_

#include "VoronoiCell.h"
#include "Tools/Voronoi/Voronoi.h"
#include "VBSMVoronoiCell.h"

//#include "Tools/Debug/Stopwatch.h"

typedef std::map<Vector2d,VoronoiCellT<VBSMData>,VoronoiCellT<VBSMData>::CompareSite> CellGraph;

class VoronoiCellGraph
{
private:
  CellGraph voronoiCells;

  vor::Voronoi v; // voronoi diagram
    vor::Vertices ver; // Voronoi points
    vor::Edges edg;   // edges of the diagram

  double width;
  double height;

public:

  ~VoronoiCellGraph() {
    clear();
  }

  const vor::Vertices& getVertices() {
    return ver;
  }

  const vor::Edges& getEdges() {
    return edg;
  }

  CellGraph& getCellGraph() {
    return voronoiCells;
  }

  inline void addVertex(const Vector2d& v) {
    ver.push_back(new Vector2d(v));
  }

  inline void setPattern(Pattern& p){
    
    for(std::vector<Vector2d>::iterator it=p.pattern.begin();it!=p.pattern.end();it++)
    {
      addVertex(*it);
    }
  }


  void clear() {
    for(vor::Vertices::iterator  i = ver.begin(); i != ver.end(); ++i) delete (*i);
    for(vor::Edges::iterator i = edg.begin(); i != edg.end(); ++i) delete (*i);
    ver.clear();
    edg.clear();

    voronoiCells.clear();
  }

  void setSize(double w, double h) {
    width = w;
    height = h;
  }


  void calculate() {
    //STOPWATCH_START("voronoiFortune");
    v.calculate(&ver,&edg,width,height);
    //STOPWATCH_STOP("voronoiFortune");

    //STOPWATCH_START("buildVoronoiCellMap");
    buildVoronoiCellMap();
    //STOPWATCH_STOP("buildVoronoiCellMap");
  }


  void buildVoronoiCellMap()
  {
    vor::Edges::iterator it = edg.begin();
    // NOTE: some of the elements are deleted within the loop (!)
    while(it != edg.end())
    {
    VEdge* edge = *it;

    // remove the edge if it is completely outside of the field
    if(cutEdge(*edge) == -1) 
    {
      it = edg.erase(it);
      //TODO: we should get rid of pointer arithmetics
      delete edge;
      continue;
    }

    Vector2d start = (*it)->start;
    Vector2d end = (*it)->end;
 
    // site to the left and right of the edge
    const Vector2d& leftSite = *((*it)->left);
    const Vector2d& rightSite = *((*it)->right);

    // get the right and the left cells (create if necessary)
    // TODO: this is slow
    VoronoiCell& leftCell = voronoiCells[leftSite];
    VoronoiCell& rightCell = voronoiCells[rightSite];

    // TODO: make it better
    leftCell.site = leftSite;
    rightCell.site = rightSite;

    leftCell.neighbors.push_back(&rightCell);
    rightCell.neighbors.push_back(&leftCell);

    leftCell.addPointToPointsOfCell(start);
    leftCell.addPointToPointsOfCell(end);
    rightCell.addPointToPointsOfCell(start);
    rightCell.addPointToPointsOfCell(end);

    // iterator of the main loop
    ++it;
    }//end while

  }//end buildVoronoiCellMap


  int cutEdge(VEdge& edge) const
  {
    bool startOutside, endOutside;

    startOutside=   (fabs(edge.start.x)>width/2.0)
          ||(fabs(edge.start.y)>height/2.0);

    endOutside=   (fabs(edge.end.x)>width/2.0)
          ||(fabs(edge.end.y)>height/2.0);

    if(!startOutside && !endOutside) return 0;

    bool startLeft, startRight, startAbove, startBelow;
    bool endLeft, endRight, endAbove, endBelow;

    startLeft=edge.start.x<(-width/2.0);
    startRight=edge.start.x>(width/2.0);
    startAbove=edge.start.y>(height/2.0);
    startBelow=edge.start.y<(-height/2.0);

    endLeft=edge.end.x<(-width/2.0);
    endRight=edge.end.x>(width/2.0);
    endAbove=edge.end.y>(height/2.0);
    endBelow=edge.end.y<(-height/2.0);

    //outside of the field
    if((startLeft&&endLeft)||(startRight&&endRight)
        ||  (startAbove&&endAbove)||(startBelow&&endBelow)){
    return -1;
    }

    // move start to the border
    if(startOutside)
    {
    if(startLeft||startRight){
      edge.start.x=(startLeft)?-width/2.0:width/2.0;
      edge.start.y=edge.f*edge.start.x+edge.g;
    }

    startAbove=edge.start.y>(height/2.0);
    startBelow=edge.start.y<(-height/2.0);

    if(startAbove||startBelow){
      edge.start.y=(startAbove)?height/2.0:-height/2.0;
      if(edge.direction->x!=0){
        edge.start.x=(edge.start.y-edge.g)/edge.f;
      }
    }
    }

    // move end to the border
    if(endOutside)
    {
    if(endLeft||endRight){
      edge.end.x=(endLeft)?-width/2.0:width/2.0;
      edge.end.y=edge.f*edge.end.x+edge.g;
    }

    endAbove=edge.end.y>(height/2.0);
    endBelow=edge.end.y<(-height/2.0);

    if(endAbove||endBelow){
      edge.end.y=(endAbove)?height/2.0:-height/2.0;
      if(edge.direction->x!=0){
      edge.end.x=(edge.end.y-edge.g)/edge.f;
      }
    }
    }

    return 0;
  }//end cutEdge
};


#endif // _VoronoiCellGraph_h_