#ifndef VORONOICELL_H
#define VORONOICELL_H

#include <Tools/Math/Vector2.h>
#include <vector>
#include <list>
#include <map>


template<class T>
class VoronoiCellT: public T
{
public:

  VoronoiCellT<T>(){}

    Vector2d site;
    std::vector<VoronoiCellT<T>*> neighbors;


  /** Sets the position of this node
    * @param position The position
    */
    void setPosition(const Vector2d& position)
    { this->site = position;}

    /** Returns the position of this node
    * @return The position
    */
    Vector2d getPosition() const
    { return site;}


    /**********
     used for debug
    **********/

    //not anymore - just for debugcontrol, corners sorted by angle from smallest to biggest
    std::list<Vector2d> pointsOfCell;

    //compare function for cellGraph
    struct CompareSite : public std::binary_function<Vector2d,Vector2d,bool>
    {
      bool operator()(const Vector2d& l, const Vector2d& r) const {
    return (l.y < r.y) || ( (l.y == r.y) && (l.x < r.x));
      }
    };

  void addPointToPointsOfCell(const Vector2d& point)
  {

      // the angle if the site would be (0,0)
      double angle = (point-this->site).angle();

      std::list<Vector2d>::iterator it;

      // search the position in the list
      for(it=this->pointsOfCell.begin();it!=this->pointsOfCell.end();it++){
        // check if it is contained in the list
        if((*it)==point){
            break;
        }

        //check whether (*it) is before or behinde point in the traverse
        if(((*it)-this->site).angle() > angle){
            this->pointsOfCell.insert(it,point);
            break;
        }
    }

    //not contained
    if(it==this->pointsOfCell.end()){
        this->pointsOfCell.push_back(point);
    }
    return;
  }
};

#endif // VORONOICELL_H
