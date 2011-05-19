/**
* @file Area.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration and definition of abstract class Area. It discribes an interface for 
* an geometrical 2D area.
* 
*/
#ifndef __Area_h_
#define __Area_h_

template<class T>
class Area
{
public:
  virtual ~Area(){}
  
  virtual bool isInside(const Vector2<T>& point) const = 0;
};//end class Area

#endif /* __Area_h_ */
