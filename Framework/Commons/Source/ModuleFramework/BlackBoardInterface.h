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

//#include "BlackBoard.h"
class BlackBoard;

class BlackBoardInterface
{
private:
  BlackBoard* theBlackBoard;

  /** indicates whether this object has its own blackboard*/
  bool blackBoardOwner;

protected:
  /** inherits the blackboard */
  BlackBoardInterface(BlackBoard* theBlackBoard);

  /** create its own blackboard */
  BlackBoardInterface();

  virtual ~BlackBoardInterface();

  /** getters */
  BlackBoard& getBlackBoard();
  const BlackBoard& getBlackBoard() const;
};

#endif //_BlackBoardInterface_h_
