#ifndef KINEMATICCHAINCALCULATOR_H
#define KINEMATICCHAINCALCULATOR_H

#include <ModuleFramework/Module.h>

#include <Representations/Modeling/KinematicChain.h>

BEGIN_DECLARE_MODULE(KinematicChainCalculator)
PROVIDE(KinematicChain)
END_DECLARE_MODULE(KinematicChainCalculator)

class KinematicChainCalculator : public KinematicChainCalculatorBase
{
public:
  KinematicChainCalculator();
  virtual void execute();
  virtual ~KinematicChainCalculator();
};

#endif // KINEMATICCHAINCALCULATOR_H
