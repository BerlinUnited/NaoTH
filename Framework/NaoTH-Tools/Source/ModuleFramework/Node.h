/* 
 * File:   Node.h
 * Author: thomas
 *
 * Created on 8. Oktober 2010, 12:14
 */

#ifndef NODE_H
#define	NODE_H

template<class FROM_TYPE, class TO_TYPE>
class Node
{
public:
  std::list<FROM_TYPE*> from;
  std::list<TO_TYPE*> to;

  void registerFrom(FROM_TYPE& node)
  {
    from.push_back(&node);
  }//end registerFrom

  void registerTo(TO_TYPE& node)
  {
    to.push_back(&node);
  }//end registerTo

  void unregisterFrom(FROM_TYPE& node)
  {
    from.remove(&node);
  }//end unregisterFrom

  void unregisterTo(TO_TYPE& node)
  {
    to.remove(&node);
  }//end unregisterFrom
};



#endif	/* NODE_H */

