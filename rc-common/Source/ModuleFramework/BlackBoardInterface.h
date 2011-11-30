/**
* @file BlackBoardInterface.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BlackBoardInterface
* Implements an interface to access the blackboard
*
*/

#ifndef __BlackBoardInterface_h_
#define __BlackBoardInterface_h_

#include "BlackBoard.h"
#include "Tools/Debug/Assert.h"
#include <iostream>

namespace crf
{

class BlackBoardInterface
{
private:
  /** */
  BlackBoard* theBlackBoard;

  /** indicates whether this object has its own blackboard*/
  bool blackBoardOwner;

protected:
  /** inherits the blackboard */
  BlackBoardInterface(BlackBoard* theBlackBoard) : theBlackBoard(theBlackBoard)
  {
    blackBoardOwner = false;
    //std::cout << "BlackBoard inherited" << std::endl;
  }

  /** create its own blackboard */
  BlackBoardInterface()
  {
    theBlackBoard = new BlackBoard();
    blackBoardOwner = true;
  }

  virtual ~BlackBoardInterface()
  {
    if(blackBoardOwner)
    {
      delete theBlackBoard;
    }
  }

  BlackBoard& getBlackBoard()
  {
    ASSERT(theBlackBoard != NULL);
    return *theBlackBoard;
  }

  const BlackBoard& getBlackBoard() const
  {
    ASSERT(theBlackBoard != NULL);
    return *theBlackBoard;
  }
};

}//crf

#endif //__BlackBoardInterface_h_
