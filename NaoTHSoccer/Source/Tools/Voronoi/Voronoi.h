#ifndef Voronoi_h
#define Voronoi_h

#include <list>
#include <queue>
#include <set>

#include "VEdge.h"
#include "VParabola.h"
#include "VEvent.h"
#include <functional>
#include <Tools/Math/Vector2.h>


namespace vor
{
  /*
  Useful data containers for Vertices (places) and Edges of Voronoi diagram
  */

  typedef std::list<Vector2d*> Vertices;
  typedef std::list<VEdge*> Edges;

  /*
  Class for generating the Voronoi diagram
  */

  class Voronoi
  {
  public:

    /*
    Constructor - without any parameters
    */

    Voronoi();

    /*
    The only public function for generating a diagram

    input:
    v    : Vertices - places for drawing a diagram
    w    : width  of the result (top left corner is (0, 0))
    h    : height of the result
    output:
    pointer to list of edges

    All the data structures are managed by this class
    */

    void calculate(Vertices* v, Edges* edg,  double w,  double h);

  private:

    /*
    places  : container of places with which we work
    edges   : container of edges which will be the result
    width   : width of the diagram
    height  : height of the diagram
    root    : the root of the tree, that represents a beachline sequence
    ly      : current "y" position of the line (see Fortune's algorithm)
    */

    Vertices*   places;
    Edges*      edges;
    double      width, height;
    VParabola*  root;
    double      ly;

    /*
    deleted  : set  of deleted (false) Events (since we can not delete from PriorityQueue
    points   : list of all new points that were created during the algorithm
    queue    : priority queue with events to process
    */

    std::set<VEvent*> deleted;
    std::list<Vector2d*> points;
    std::priority_queue<VEvent*, std::vector<VEvent*>, VEvent::CompareEvent> queue;

    /*
    InsertParabola  : processing the place event
    RemoveParabola  : processing the circle event
    FinishEdge      : recursively finishes all infinite edges in the tree
    GetXOfEdge      : returns the current x position of an intersection point of left and right parabolas
    GetParabolaByX  : returns the Parabola that is under this "x" position in the current beachline
    CheckCircle     : checks the circle event (disappearing) of this parabola
    GetEdgeInterse
    */

    void InsertParabola(Vector2d* p);
    void RemoveParabola(VEvent* e);
    void FinishEdge(VParabola* n);
    double GetXOfEdge(VParabola* par, double y) const;
    VParabola* GetParabolaByX(double xx) const;
    double GetY(const Vector2d& p, double x) const;
    void CheckCircle(VParabola* b, bool mustHaveCircleEvent);
    Vector2d* GetEdgeIntersection(VEdge* a, VEdge* b, bool mustIntersect) const;


    struct CompareSites : public std::binary_function<Vector2d, Vector2d, bool>
    {
      bool operator()(const Vector2d& l, const Vector2d& r) const 
      {
        if (l.y < r.y){
          return true;
        }

        if ( (l.y == r.y) && (l.x < r.x)){
          return true;
        }

        return false; 
      }
    };
  };
}

#endif
