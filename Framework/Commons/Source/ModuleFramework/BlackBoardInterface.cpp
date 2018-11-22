/**
* @file BlackBoardInterface.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BlackBoardInterface
* Implements an interface to access the blackboard
*
*/


#include "BlackBoard.h"
#include "BlackBoardInterface.h"
//#include "Tools/Debug/NaoTHAssert.h"
//#include <iostream>

BlackBoardInterface::BlackBoardInterface(BlackBoard* theBlackBoard) 
  : theBlackBoard(theBlackBoard)
{
  blackBoardOwner = false;
  //std::cout << "BlackBoard inherited" << std::endl;
}

BlackBoardInterface::BlackBoardInterface()
{
  theBlackBoard = new BlackBoard();
  blackBoardOwner = true;
}

BlackBoardInterface::~BlackBoardInterface() {
  if(blackBoardOwner) {
    delete theBlackBoard;
  }
}

BlackBoard& BlackBoardInterface::getBlackBoard() {
  assert(theBlackBoard != NULL);
  return *theBlackBoard;
}

const BlackBoard& BlackBoardInterface:: getBlackBoard() const {
  assert(theBlackBoard != NULL);
  return *theBlackBoard;
}
