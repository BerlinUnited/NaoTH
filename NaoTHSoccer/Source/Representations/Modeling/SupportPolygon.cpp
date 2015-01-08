/** 
* @file SupportPolygon.cpp
* Implementation of class SupportPolygon.
*
* @author <A href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</A>
*/

#include "SupportPolygon.h"

SupportPolygon::SupportPolygon():
mode(NONE)
{
}

SupportPolygon::~SupportPolygon()
{
}

bool SupportPolygon::isLeftFootSupportable() const
{
  return ( mode & (DOUBLE | DOUBLE_LEFT | DOUBLE_RIGHT | LEFT ) ) != 0;
}
  
bool SupportPolygon::isRightFootSupportable() const
{
  return ( mode & (DOUBLE | DOUBLE_LEFT | DOUBLE_RIGHT | RIGHT ) ) != 0;
}

std::ostream& operator <<(std::ostream& ost, const SupportPolygon& v)
{
    ost<<v.mode<<' ';
    
    const std::vector<Vector2<double> >& vp = v.vertex;
    ost<<vp.size()<<' ';
    for(std::vector<Vector2<double> >::const_iterator i=vp.begin();
            i != vp.end(); ++i){
        ost<<*i<<' ';
    }
    return ost;
}
