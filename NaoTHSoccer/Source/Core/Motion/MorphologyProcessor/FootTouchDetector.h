/**
* @file FootTouchDetector.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* detect if the foot touch the ground
*/

#ifndef _FOOTTOUCHDETECTOR_H
#define	_FOOTTOUCHDETECTOR_H

#include "Tools/LinearClassifier.h"
#include <Representations/Infrastructure/FSRData.h>

class FootTouchDetector 
{
public:
  FootTouchDetector();
  ~FootTouchDetector();

  /** set the FSR data pointer, and try to load parameters from string */
  void init(const double* fsr, const std::string& cfg);

  /** save the parameters to string */
  void save(std::string& cfg) const;

  /** detect if the foot is touch the ground */
  bool isTouch() const;

  /** calibrate the detector by giving the current state: touch or not */
  void calibrate(bool isTouch);

private:
  const double* fsrData; // pointer to current FSR data
  LinearClassifier<4> theLinearClassifier;

  // training set
  std::vector<Vector_n<double,4> > touchTrainSet;
  std::vector<Vector_n<double,4> > unTouchTrainSet;
};

#endif	/* _FOOTTOUCHDETECTOR_H */

