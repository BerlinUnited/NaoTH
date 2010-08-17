/**
 * @file main.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief main file of SimSparkController
 *
 *  Usage: naoth-simspark [team_name [player_number [server_ip [server_port]]]]
 *  Example: naoth-simspark NaoTH 2 192.168.1.110
 */

#include "SimSparkOfficialController.h"

using namespace std;


void printUsage()
{
  cout << "NaoTH Simspark controller" << endl;
  cout << "-------------------------" << endl;
  cout << "www.naoteamhumboldt.de" << endl << endl;
  cout << "Usage: naoth-simspark [teamname [playernumber [server [port]]]]" << endl << endl;

}

int main(int argc, char** argv)
{
  string teamName = "NaoTH";
  unsigned int num = 0; // zero means get a number from server
  string server = "localhost";
  unsigned int port = 3100;
  
  if(argc == 2 && 
    (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 ))
  {
    printUsage();
    exit(0);
  }

  // read options
  if (argc > 1)
  {
    teamName = string(argv[1]);
  }
  if (argc > 2)
  {
    num = atoi(argv[2]);
  }
  if (argc > 3)
  {
    server = string(argv[3]);
  }
  if (argc > 4)
  {
    port = atoi(argv[4]);
  }

//    SimSparkController nao;
  SimSparkOfficialController nao;

  if (!nao.init(teamName, num, server, port))
  {
    cerr << "NaoTH SimSpark initialization failed!" << endl;
    return EXIT_FAILURE;
  }

  //nao.main();
  nao.multiThreadsMain();

  return 0;
}