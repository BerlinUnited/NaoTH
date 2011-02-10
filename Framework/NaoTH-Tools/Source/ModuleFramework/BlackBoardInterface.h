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
#include "Tools/Debug/NaoTHAssert.h"
#include <iostream>

class BlackBoardInterface
{
private:
  // TODO: make it reference
  BlackBoard* theBlackBoard;
  bool blackBoardOwner;

protected:
  BlackBoardInterface(BlackBoard* theBlackBoard) : theBlackBoard(theBlackBoard)
  {
    blackBoardOwner = false;
    std::cout << "BlackBoard inherited" << std::endl;
  }

  BlackBoardInterface()
  {
    theBlackBoard = new BlackBoard();
    blackBoardOwner = true;
    std::cout << "BlackBoard created" << std::endl;
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
};

#endif //__BlackBoardInterface_h_
