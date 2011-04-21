/**
 * @file Cognition.h
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

#include <ModuleFramework/ModuleManager.h>
#include <Core/Tools/Modules/ModuleLoader.h>

class Cognition : public naoth::Callable, public ModuleManager
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void call();

  void init(naoth::PlatformDataInterface& platformInterface);

private:
  ModuleLoader moduleLoader;
};

#endif	/* COGNITION_H */

