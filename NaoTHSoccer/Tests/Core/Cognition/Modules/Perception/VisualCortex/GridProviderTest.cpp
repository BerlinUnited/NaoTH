#include <ModuleFramework/Module.h>
#include "ModuleFramework/ModuleManager.h"
#include <Representations/Infrastructure/Image.h>
#include "Core/Cognition/Modules/Perception/VisualCortex/GridProvider.h"
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

  unsigned int numOfGridPoints =getColoredGrid().uniformGrid.numberOfGridPoints;
  for(unsigned int i=0; i < numOfGridPoints/2; i++)
  {
    Vector2<int> v = getColoredGrid().uniformGrid.getPoint(i);
    getImage().set(v.x,v.y,greenPixel);
  }
  for(unsigned int i=numOfGridPoints/2; i < numOfGridPoints; i++)
  {
    Vector2<int> v = getColoredGrid().uniformGrid.getPoint(i);
    getImage().set(v.x,v.y,redPixel);
  }

  gridProvider->execute();

  for(unsigned int i=0; i < numOfGridPoints/2; i++)
  {
    ASSERT_EQ(ColorClasses::green, getColoredGrid().pointsColors[i]);
  }
  for(unsigned int i=numOfGridPoints/2; i < numOfGridPoints; i++)
  {
    ASSERT_EQ(ColorClasses::red, getColoredGrid().pointsColors[i]);
  }
}
