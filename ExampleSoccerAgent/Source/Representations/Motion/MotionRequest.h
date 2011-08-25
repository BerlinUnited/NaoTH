/**
 * @file MotionRequest.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @breief this is a very simplemotion request
 */

#ifndef MotionRequest_H
#define	MotionRequest_H

class MotionRequest
{
public:

  MotionRequest()
    :
    id(stand)
  {
  }

  enum RequestID
  {
    stand,
    walk_forward,
    turn_left,
    stand_up_from_front,
    stand_up_from_back,
    numOfRequestID
  };

  RequestID id;
};

#endif	/* MotionRequest_H */

