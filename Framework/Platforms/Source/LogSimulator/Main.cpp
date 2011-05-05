/* 
 * File:   Main.cpp
 * Author: thomas
 *
 * Created on 27. November 2008, 17:20
 */

#include <glib.h>
#include <glib-object.h>


#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>

#include "Cognition.h"
#include "Motion.h"

#include <cstring>
#include "Simulator.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
  g_type_init();
  
  bool compatibleMode = false;
  bool backendMode = false;

  char* logpath = getenv("NAOTH_LOGFILE");

	if(logpath == NULL)
  {
		if(argc > 1)
		{
			logpath = argv[argc-1];

			if(argc > 2)
			{
				for (int i = 1; i < argc - 1; i++)
				{
					// check if compatible mode
					if (strcmp(argv[i], "-c") == 0)
					{
						compatibleMode = true;
					} else if (strcmp(argv[i], "-b") == 0)
					{
						backendMode = true;
					}
				}//end for
			}//end if > 2
		}//end if > 1
  }
  else
  {
		compatibleMode = getenv("NAOTH_LOGCOMPAT") != NULL;
  }


	if(logpath == NULL)
	{
		cerr << "You need to give the path to the logfile as argument" << endl;
		cerr << "arguments: (-b)? (-c)? <logfile>" << endl;
		cerr << "\"-b\" enable the backend mode which is only used by LogfilePlayer of RobotControl" << endl;
		cerr << "when using \"-c\" you are forcing a compatible mode" << endl;
		return (EXIT_FAILURE);
	}
  
  Cognition theCognition;
  Motion theMotion;
  
  Simulator sim(logpath, compatibleMode, backendMode);
  
  Platform::getInstance().init(&sim);
  
  sim.registerCallbacks(&theMotion, &theCognition);
  
  sim.main();
  
  Stopwatch::getInstance().dump();

  return (EXIT_SUCCESS);
}//end main

