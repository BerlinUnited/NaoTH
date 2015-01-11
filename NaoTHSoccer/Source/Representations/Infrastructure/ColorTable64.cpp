/** 
* @file ColorTable64.cpp
* Implementation of class ColorTable64.
*
* @author <A href="mailto:juengel@informatik.hu-berlin.de">Matthias Juengel</A>
*/

#include "ColorTable64.h"


using namespace std;

bool ColorTable64::loadFromFile(const std::string& fileName)
{
  ifstream inputFileStream ( fileName.c_str() , ifstream::in );
  
  // could not open color table
  if(inputFileStream.fail())
  {
    return false;
  }

  inputFileStream.read((char*)&(colorClasses), colorTableLength);
  inputFileStream.close();

  // check the color-table
  for(int channel1 = 0; channel1 < 64; channel1++)
  {
    for(int channel2 = 0; channel2 < 64; channel2++)
    {
      for(int channel3 = 0; channel3 < 64; channel3++)
      {
        ColorClasses::Color color = getColorClass(
          static_cast<unsigned char>(channel1 << 2), 
          static_cast<unsigned char>(channel2 << 2), 
          static_cast<unsigned char>(channel3 << 2));

        if(color >= ColorClasses::numOfColors)
        {
          std::cout << std::endl << "Colortable broken!!! ["
            << channel1 << "," << channel2 << "," << channel3 << "] = "
            << (int) color
            << std::endl << std::endl;
          // clear the colortable
          reset();
          return false;
        }
      }
    }
  }

  return true;
}//end loadFromFile

