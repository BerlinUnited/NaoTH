/*
 * @file VirtualVision.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief the vision information come from official SimSpark
 *
 */

#ifndef _VIRTUALVISION_H
#define _VIRTUALVISION_H

#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Printable.h"

#include <list>
#include <map>

namespace naoth
{
  class VirtualVision : public Printable
  {
  public:
    typedef std::map<std::string, Vector3d> PointMap;
    PointMap data;

    struct Line
    {
      Vector3d p0, p1;
    };
    std::list<Line> lines;

    void clear();

    virtual void print(std::ostream& stream) const;
  };

  class VirtualVisionTop: public VirtualVision {};
}

#endif  /* _VIRTUALVISION_H */

