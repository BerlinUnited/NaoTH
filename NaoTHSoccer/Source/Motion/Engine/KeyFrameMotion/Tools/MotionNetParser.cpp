/**
* @file MotionNetParser.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MotionNetParser
*/

#include "MotionNetParser.h"

using namespace naoth;

void MotionNetParser::eat()
{
  scanner.getNextToken();
}//end eat


bool MotionNetParser::isToken(KeyWordID tokenId)
{
  return scanner.isToken(getKeyWord(tokenId));
}//end isToken

void MotionNetParser::isTokenAndEat(KeyWordID tokenId)
{
  if(isToken(tokenId))
    eat();
  else
    throw_exception(
      string("Syntaxerror: found '")+string(scanner.buffer)+
      string("' but ")+ getKeyWord(tokenId)+string("' expected."));
}//end isTokenAndEat


MotionNet MotionNetParser::parse()
{
  // create new Motion Net
  MotionNet motionNet;

  eat();
  if(!scanner.isToken("nao"))
    throw_exception(string("not a 'nao' motion net."));

  eat(); // "nao"
  parseMotionNet(motionNet);
  //isTokenAndEat(EOF);

  return motionNet;
}//end parse


void MotionNetParser::parseMotionNet(MotionNet& motionNet)
{
  parseJointIndex(motionNet);

  while( isToken(keyFrame) )
  {
    parseKeyFrame(motionNet);
    //motionNet->keyFrameArray[motionNet->numberOfKeyFrames-1].print();
  }//end while

  while( isToken(transition) )
  {
    parseTransition(motionNet);
    //motionNet->transitionArray[motionNet->numberOfTransitions-1].print();
  }//end while

}//end parseMotionNet



void MotionNetParser::parseJointIndex(MotionNet& motionNet)
{
  while(!isToken(transition) && !isToken(keyFrame))
  {
    JointData::JointID id = JointData::jointIDFromName(std::string(scanner.buffer));
    eat(); // eat the joint name
    isTokenAndEat(separator); // ";"

    if(id == JointData::numOfJoint)
    {
      cout << JointData::getJointName(id) << ": " << std::string(scanner.buffer) << endl;
    }
    else
    {
      if(motionNet.getNumOfJoints() < JointData::numOfJoint)
      {
        motionNet.add(id);
      }
      else
      {
        throw_exception(string("To many joint indices."));
      }
    }
  }//end while
}//end parseJointIndex


void MotionNetParser::parseTransition(MotionNet& motionNet)
{
  MotionNet::Transition result;

  // transition;id_from;id_to;duration;condition

  isTokenAndEat(transition); // keyword "transition"
  isTokenAndEat(separator); // ";"
  result.fromKeyFrame = parseInt(); // id from
  isTokenAndEat(separator); // ";"
  result.toKeyFrame = parseInt(); // id to
  isTokenAndEat(separator); // ";"
  result.duration = parseFloat(); // duration

  isTokenAndEat(separator); // ";"

  // get the condition and eat
  result.condition = std::string(scanner.buffer);
  eat();

  motionNet.add(result);
}//end parseTransition


void MotionNetParser::parseKeyFrame(MotionNet& motionNet)
{
  MotionNet::KeyFrame result;
  
  isTokenAndEat(keyFrame); // keyword "keyframe"
  isTokenAndEat(separator); // ";"
  result.id = parseInt(); // id

  // skip the coordinates
  isTokenAndEat(separator); // ";"
  parseInt(); // x
  isTokenAndEat(separator); // ";"
  parseInt(); // y

  // parse joint values
  for(int i = 0; i < motionNet.getNumOfJoints(); i++)
  {
    isTokenAndEat(separator); // ";"
    result.jointData[i] = Math::fromDegrees(parseFloat());
  }//end for
  
  motionNet.add(result);
}//end parseKeyFrame


// pars a integer (something like "- 20" is not allowed, correct would be "-20")
int MotionNetParser::parseInt()
{
  int result = atoi(scanner.buffer);
  eat();

  return result;
}//end parseInt


// pars a float
float MotionNetParser::parseFloat()
{
  float result = (float)atof(scanner.buffer);
  eat();

  return result;
}//end parseFloat


std::string MotionNetParser::getKeyWord(KeyWordID keyWordID)
{
  switch(keyWordID)
  {
    case endOfLine: return std::string("\n");
    case separator: return std::string(";");
    case keyFrame: return std::string("keyframe");
    case transition: return std::string("transition");
    case comment: return std::string("#");
    default: return std::string("unknown");
  }//end switch
}//end getKeyWord

void MotionNetParser::throw_exception(std::string message)
{
  std::stringstream out;
  out << "[MotionNetParser](" << scanner.getLineNumber() << ") ERROR : " << message;
  throw out.str();
}//end throw_exception

