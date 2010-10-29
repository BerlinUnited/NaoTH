/**
 * @file Cognition.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>
#include <PlatformInterface/Callable.h>


class Cognition : public naoth::Callable
{
public:
  Cognition();
  virtual ~Cognition();


  template<class PlatformType>
  void init(PlatformType& platformInterface)
  {
    std::cout << "Cognition register start" << std::endl;

    std::cout << "Cognition register end" << std::endl;
  }//end init


  virtual void call();

};

#endif	/* COGNITION_H */

