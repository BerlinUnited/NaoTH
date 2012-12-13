/**
* @file BlackBoardInterface.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BlackBoardInterface
* Implements an interface to access the blackboard
*
*/

#ifndef _BlackBoardInterface_h_
#define _BlackBoardInterface_h_

#include "BlackBoard.h"
#include "Tools/Debug/NaoTHAssert.h"
#include <iostream>

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
    assert(theBlackBoard != NULL);
    return *theBlackBoard;
  }

  const BlackBoard& getBlackBoard() const
  {
    assert(theBlackBoard != NULL);
    return *theBlackBoard;
  }
};

#endif //_BlackBoardInterface_h_
