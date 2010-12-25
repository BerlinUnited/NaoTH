/**
 * @file BallPercept.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief this is a very simple ball percept
 */

#ifndef BALLPERCEPT_H
#define	BALLPERCEPT_H

class BallPercept
{
public:

  BallPercept() 
    :
    wasSeen(false),
    distance(0)
  {
  }

  /** */
  bool wasSeen;

  /** */
  double distance;
};

#endif	/* BALLPERCEPT_H */

