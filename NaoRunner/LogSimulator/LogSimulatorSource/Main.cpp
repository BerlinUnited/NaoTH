/* 
 * File:   Main.cpp
 * Author: thomas
 *
 * Created on 27. November 2008, 17:20
 */

#include <cstring>
#include "Simulator.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{

  bool compatibleMode = false;
  bool backendMode = false;
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
    }
  }
  else if(argc < 2)
  {
    cerr << "You need to give the path to the logfile as argument" << endl;
    cerr << "arguments: (-b)? (-c)? <logfile>" << endl;
    cerr << "\"-b\" enable the backend mode which is only used by LogfilePlayer of RobotControl" << endl;
    cerr << "when using \"-c\" you are forcing a compatible mode" << endl;
    return (EXIT_FAILURE);
  }

  Simulator sim(argv[argc-1], compatibleMode, backendMode);
  sim.main();
  return (EXIT_SUCCESS);
}//end main

