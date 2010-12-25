/**
 * @file BlackBoard.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @breief implementation of a very basic static black board
 *
 */

#ifndef BlackBoard_H
#define	BlackBoard_H

#include "BallPercept.h"
#include "MotionRequest.h"

class BlackBoard
{

public:
  /** gives access to the instance of the blackboard */
  static BlackBoard& getInstance()
  {
    static BlackBoard instance;
    return instance;
  }

  /** Representations */
  BallPercept theBallPercept;
  MotionRequest theMotionRequest;


private:
  /** this constructor cann only be called from inside */
  BlackBoard(){}

  /** the ONLY instance of the blackboard */
  static BlackBoard instance;

};

#endif	/* BlackBoard_H */