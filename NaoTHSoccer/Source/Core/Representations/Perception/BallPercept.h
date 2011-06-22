/** 
 * @file BallPercept.h
 *
 * Declaration of class BallPercept
 */

#ifndef __BallPercept_h_
#define __BallPercept_h_

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>
#include <Tools/ColorClasses.h>
#include <Tools/DataStructures/Printable.h>

#include <Representations/Infrastructure/FrameInfo.h>


class BallPercept : public Printable
{ 
public:
  BallPercept(): 
    ballWasSeen(false),
    radiusInImage(0.0),
    ballColor(ColorClasses::orange)
  {}

  /* indicates wether a ball was seen, i.e., the percept is valid */
  bool ballWasSeen;

  /* center of ball in image coordinates */
  Vector2<double> centerInImage;

  /* radius of ball in image */
  double radiusInImage;

  ColorClasses::Color ballColor;

  /* relative ball position to the robot based on the camera matrix */
  Vector2<double> bearingBasedOffsetOnField; 

  /* relative ball position to the robot based on the seen size of the ball */
  Vector3<double> sizeBasedRelativePosition; 
  
  FrameInfo frameInfoWhenBallWasSeen;

  /* reset percept */
  void reset()
  {
    ballWasSeen = false;
  }//end reset
/*
  virtual void toDataStream(ostream& stream) const
  {
    naothmessages::BallPercept p;

    p.set_ballwasseen(ballWasSeen);
    
    p.mutable_centerinimage()->set_x(centerInImage.x);
    p.mutable_centerinimage()->set_y(centerInImage.y);

    p.set_radiusinimage(radiusInImage);
    
    p.set_ballcolor((naothmessages::Color) ballColor);

    p.mutable_bearingbasedoffsetonfield()->set_x(bearingBasedOffsetOnField.x);
    p.mutable_bearingbasedoffsetonfield()->set_y(bearingBasedOffsetOnField.y);

    p.mutable_frameinfowhenballwasseen()->set_framenumber(frameInfoWhenBallWasSeen.frameNumber);
    p.mutable_frameinfowhenballwasseen()->set_time(frameInfoWhenBallWasSeen.time);

    google::protobuf::io::OstreamOutputStreamLite buf(&stream);
    p.SerializeToZeroCopyStream(&buf);
  }

  virtual void fromDataStream(istream& stream)
  {
    naothmessages::BallPercept p;
    google::protobuf::io::IstreamInputStreamLite buf(&stream);
    p.ParseFromZeroCopyStream(&buf);

    if(p.has_ballwasseen())
    {
      ballWasSeen = p.ballwasseen();
    }
    if(p.has_centerinimage())
    {
      centerInImage.x = p.centerinimage().x();
      centerInImage.y = p.centerinimage().y();
    }
    if(p.has_radiusinimage())
    {
      radiusInImage = p.radiusinimage();
    }
    if(p.has_ballcolor())
    {
      ballColor = (ColorClasses::Color) p.ballcolor();
    }
    if(p.has_bearingbasedoffsetonfield())
    {
      bearingBasedOffsetOnField.x = p.bearingbasedoffsetonfield().x();
      bearingBasedOffsetOnField.y = p.bearingbasedoffsetonfield().y();
    }
    if(p.has_frameinfowhenballwasseen())
    {
      frameInfoWhenBallWasSeen.frameNumber = p.frameinfowhenballwasseen().framenumber();
      frameInfoWhenBallWasSeen.time = p.frameinfowhenballwasseen().time();
    }
  }
*/
  virtual void print(ostream& stream) const
  {
    stream << "ballWasSeen = " << ballWasSeen << endl;
    stream << "centerInImage = " << centerInImage << endl;
    stream << "radiusInImage = " << radiusInImage << endl;
    stream << "bearingBasedOffsetOnField = " << bearingBasedOffsetOnField << endl;
    //stream << "frameInfoWhenBallWasSeen:" << frameInfoWhenBallWasSeen << endl;
    stream << "ball color:\n" << ColorClasses::getColorName(ballColor) << endl;
  }//end print
};


#endif //__BallPercept_h_


