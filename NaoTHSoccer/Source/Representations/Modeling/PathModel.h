/**
 * @file PathModel.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef _PathModel_H_
#define _PathModel_H_

class PathModel
{
public:
   PathModel() {}
  ~PathModel() {}

  // Go to Ball with right or left foot (depending on XABSL decision)
  bool goto_ball_right;
  bool goto_ball_left;
  // distance and yOffset parameters (set by XABSL)
  double goto_distance;
  double goto_yOffset;
};

#endif /* _PathModel_H_ */
