/**
* @file MotionNetParser.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class MotionNetParser
*/

#ifndef __MotionNetParser_h_
#define __MotionNetParser_h_

#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <string.h>
#include <vector>

#include "Tools/Math/Common.h"
#include "MotionNet.h"
#include "Scanner.h"
#include "Representations/Infrastructure/JointData.h"


class MotionNetParser
{
public:

  MotionNetParser(Scanner& scanner) 
    : scanner(scanner)
  {}

  ~MotionNetParser(){}

  MotionNet parse();

private:

  enum KeyWordID
  {
    endOfLine,
    separator,
    keyFrame,
    transition,
    comment,
    numberOfKeyWordID
  };

  Scanner& scanner;

  void parseMotionNet(MotionNet& motionNet);

  void parseJointIndex(MotionNet& motionNet);
  void parseKeyFrame(MotionNet& motionNet);
  void parseTransition(MotionNet& motionNet);

  float parseFloat();
  int parseInt();

  std::string getKeyWord(KeyWordID keyWordID);
  bool isToken(KeyWordID tokenId);
  void eat();
  void isTokenAndEat(KeyWordID tokenId);

  void throw_exception(std::string message);
};

#endif //__MotionNetParser_h_
