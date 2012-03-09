/* 
 * File:   FieldColorClassifierPreProcessor.h
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef FIELDCOLORCLASSIFIERPREPROCESSOR_H
#define FIELDCOLORCLASSIFIERPREPROCESSOR_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/FieldColorPreProcessingPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"

//Perception
#include "Tools/ImageProcessing/Histogram.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifierPreProcessor)
  REQUIRE(Histogram)
  REQUIRE(FieldColorPercept)
  REQUIRE(FrameInfo)

  PROVIDE(FieldColorPreProcessingPercept)
END_DECLARE_MODULE(FieldColorClassifierPreProcessor)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class FieldColorClassifierPreProcessor : public  FieldColorClassifierPreProcessorBase
{
public:
  FieldColorClassifierPreProcessor();
  virtual ~FieldColorClassifierPreProcessor(){}

  /** executes the module */
  void execute();

};

#endif  /* FIELDCOLORCLASSIFIER_H */

