/**
* @file Container.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration and definition of abstract class Container. 
* It discribes an interface for an container being able to collect objects of a given type.
* 
*/
#ifndef __Container_h_
#define __Container_h_

template<typename T>
class Container
{
public:
  virtual void add(const T& object) = 0;
};

#endif /* __Container_h_ */