/*
 * @file VirtualVision.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief the vision information come from official SimSpark
 *
 */

#include "VirtualVision.h"

#include <string>

using namespace naorunner;

VirtualVision::VirtualVision()
{

}

VirtualVision::~VirtualVision()
{
  
}

void VirtualVision::clear()
{
  data.clear();
  lines.clear();
}

void VirtualVision::print(ostream& stream) const
{
  if ( data.empty() && lines.empty() )
  {
    stream<<"EMPTY";
    return;
  }

  for(map<string, Vector3<double> >::const_iterator iter=data.begin();
    iter!=data.end(); ++iter){
      stream<<iter->first<<" = ("<<iter->second<<")\n";
    }

  stream<<"\n"<<lines.size()<<" Lines:\n";
  for(list<Line>::const_iterator iter=lines.begin(); iter!=lines.end(); ++iter)
  {
    stream<<"("<<iter->p0<<") -- ("<<iter->p1<<")\n";
  }
}
