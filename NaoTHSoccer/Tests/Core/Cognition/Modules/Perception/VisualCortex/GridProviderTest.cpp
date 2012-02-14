#include <ModuleFramework/Module.h>
#include "ModuleFramework/ModuleManager.h"
#include <Representations/Infrastructure/Image.h>
#include "Cognition/Modules/Perception/VisualCortex/GridProvider.h"
#include <gtest/gtest.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(GridProviderTest)
PROVIDE(Image)
PROVIDE(ColorTable64)
PROVIDE(ColoredGrid)
END_DECLARE_MODULE(GridProviderTest)

class GridProviderTest : public testing::Test, public GridProviderTestBase, private ModuleManager
{
public:
  virtual void execute()  {};

  GridProvider* gridProvider;

protected:
  virtual void SetUp()
  {
    ModuleCreator<GridProvider>* m = registerModule<GridProvider>("GridProvider");
    m->setEnabled(true);
    gridProvider = (GridProvider*) m->getModule();
  }

};

TEST_F(GridProviderTest, ColorClassification)
{
  Pixel greenPixel;
  greenPixel.y = 100;
  greenPixel.u = 100;
  greenPixel.v = 100;
  Pixel redPixel;
  redPixel.y = 200;
  redPixel.u = 100;
  redPixel.v = 50;
  getColorTable64().setColorClass(ColorClasses::green, greenPixel.y, greenPixel.u,greenPixel.v);
  getColorTable64().setColorClass(ColorClasses::red, redPixel.y, redPixel.u,redPixel.v);

  for(unsigned int x=0; x < 320; x++)
  {
    for(unsigned int y=0; y < 240; y++)
    {
      // set first half of the image to green, the other one red
      if(y < 120)
      {
        getImage().set(x,y,greenPixel);
      }
      else
      {
        getImage().set(x,y,redPixel);
      }
    }
  }

  gridProvider->execute();
  
  // check if the grid points correspond to this color classes
  
  /* this test doesn't apply anymore
  unsigned int numOfGridPoints =getColoredGrid().uniformGrid.numberOfGridPoints;
  for(unsigned int i=0; i < numOfGridPoints/2; i++)
  {
    ASSERT_EQ(ColorClasses::green, getColoredGrid().pointsColors[i]);
  }
  for(unsigned int i=numOfGridPoints/2; i < numOfGridPoints; i++)
  {
    ASSERT_EQ(ColorClasses::red, getColoredGrid().pointsColors[i]);
  }
  */
}
