/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Krause, Thomas</a>
 */

#include "PlatformInterface/PlatformInterface.h"

using namespace naoth;
using namespace std;

PlatformBase::~PlatformBase()
{
  std::cout << "destruct PlatformBase" << std::endl;
}

