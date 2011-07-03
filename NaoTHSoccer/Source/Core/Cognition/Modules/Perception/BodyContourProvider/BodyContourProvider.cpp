/*
*@file BodyContourProvider.h
* Module implementation for providing the robot's contour in the image. 
* The Body Contour is used to exclude robot's limbs from image, so that they
* couldn't be confused with other objects
*
*@author Kirill Yasinovskiy
*/

#include "BodyContourProvider.h"
#include "Tools/Math/Matrix.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/Stopwatch.h"
#include "PlatformInterface/Platform.h"
#include "Tools/Math/Geometry.h"
#include "Tools/CameraGeometry.h"
#include "Tools/Math/ConvexHull.h"
#include <vector>




BodyContourProvider::BodyContourProvider()
{
  getBodyContour().stepSize = 20;
  getBodyContour().yDensity = getImage().cameraInfo.resolutionHeight/getBodyContour().stepSize;
  getBodyContour().xDensity = getImage().cameraInfo.resolutionWidth/getBodyContour().stepSize;
 
  getBodyContour().grid.resize(getBodyContour().xDensity);
  for (int i = 0; i < getBodyContour().xDensity; i++)
  {
    getBodyContour().grid[i].resize(getBodyContour().yDensity);
  }
  // load contour parameters
  // torso
  bodyparts.torso.push_back(Vector3<double>(-60, 17, 80));
  bodyparts.torso.push_back(Vector3<double>(-53, 45, 80));
  bodyparts.torso.push_back(Vector3<double>( -35, 58, 100));
  bodyparts.torso.push_back(Vector3<double>( -32, 75, 115));
  bodyparts.torso.push_back(Vector3<double>(32, 75, 115));
  bodyparts.torso.push_back(Vector3<double>(35, 58, 100));
  bodyparts.torso.push_back(Vector3<double>(51, 45, 80));
  bodyparts.torso.push_back(Vector3<double>(59, 17, 80));
  bodyparts.torso.push_back(Vector3<double>(59, -17, 80));
  bodyparts.torso.push_back(Vector3<double>(51, -45, 80));
  bodyparts.torso.push_back(Vector3<double>(35, -58, 100));
  bodyparts.torso.push_back(Vector3<double>(32, -75, 115));
  bodyparts.torso.push_back(Vector3<double>(-32, -75, 115));
  bodyparts.torso.push_back(Vector3<double>(-35, -58, 100));
  bodyparts.torso.push_back(Vector3<double>(-53, -45, 80));
  bodyparts.torso.push_back(Vector3<double>(-60, -17, 80));   
  // upper arm
  bodyparts.upperArm.push_back(Vector3<double>(15, -5, -60));
  bodyparts.upperArm.push_back(Vector3<double>(10, 5, -60));
  bodyparts.upperArm.push_back(Vector3<double>(-25, 5, -40));
  bodyparts.upperArm.push_back(Vector3<double>(-30, 25, -30));
  bodyparts.upperArm.push_back(Vector3<double>(-40, 25, -10));
  bodyparts.upperArm.push_back(Vector3<double>(-45, 20, 5));
  bodyparts.upperArm.push_back(Vector3<double>(-35, 10, 30));
  bodyparts.upperArm.push_back(Vector3<double>(-10, -5, 42));
  bodyparts.upperArm.push_back(Vector3<double>(0, -22, 47));
  bodyparts.upperArm.push_back(Vector3<double>(120, -22, 47));
  // lower arm
  bodyparts.lowerArm.push_back(Vector3<double>(-20, 30, 25));
  bodyparts.lowerArm.push_back(Vector3<double>(120, 30, 45));
  bodyparts.lowerArm.push_back(Vector3<double>(120, -40, 40));
  // upper leg
  bodyparts.upperLeg.push_back(Vector3<double>(50, 30, 0));
  bodyparts.upperLeg.push_back(Vector3<double>(50, 50, -115));
  bodyparts.upperLeg.push_back(Vector3<double>(-60, 45, -160));
  bodyparts.upperLeg.push_back(Vector3<double>(-60, -38, -160));
  bodyparts.upperLeg.push_back(Vector3<double>(50, -42, -115));
  bodyparts.upperLeg.push_back(Vector3<double>(50, -30, 0));
  // lower leg
  bodyparts.lowerLeg.push_back(Vector3<double>(50, 50, -115));
  bodyparts.lowerLeg.push_back(Vector3<double>(50, -42, -115));
  // foot
  bodyparts.foot.push_back(Vector3<double>(-20, 50, -35));
  bodyparts.foot.push_back(Vector3<double>(65, 50, -35));
  bodyparts.foot.push_back(Vector3<double>(105, 20, -35));
  bodyparts.foot.push_back(Vector3<double>(105, -20, -35));
  bodyparts.foot.push_back(Vector3<double>(80, -40, -35));
  bodyparts.foot.push_back(Vector3<double>(-20, -40, -35));
  
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d_body_contour_feet", "draws 3D body contour of the feet", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d_body_contour_legs", "draws 3D body contour of the legs", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d_body_contour_torso", "draws 3D body contour of the torso", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d_body_contour_shoulders", "draws 3D body contour of the shoulders", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d_body_contour_forearms", "draws 3D body contour of the forearms", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_body_contour_lines", "draws body contour on the image based on lines", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_activated_cells", "draws activated cells", false);
}

void BodyContourProvider::execute()
{
  getBodyContour().lines.clear();
  imagePoints.clear();
  eraseGrid();
  getBodyContour().grid.resize(getBodyContour().xDensity);
  for (int i = 0; i < getBodyContour().xDensity; i++)
  {
    getBodyContour().grid[i].resize(getBodyContour().yDensity);
  }
  
  lineNumber = 0; 
  
  // add body contours
  add(getKinematicChain().theLinks[KinematicChain::Torso].M, bodyparts.torso, 1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::Torso);
  add(getKinematicChain().theLinks[KinematicChain::LThigh].M, bodyparts.upperLeg, 1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::LegLeft);
  add(getKinematicChain().theLinks[KinematicChain::RThigh].M, bodyparts.upperLeg, -1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::LegRight);
  add(getKinematicChain().theLinks[KinematicChain::LThigh].M, bodyparts.lowerLeg, 1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::LegLeft);
  add(getKinematicChain().theLinks[KinematicChain::RThigh].M, bodyparts.lowerLeg, -1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::LegRight);
  add(getKinematicChain().theLinks[KinematicChain::LFoot].M, bodyparts.foot, 1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::FootLeft);
  add(getKinematicChain().theLinks[KinematicChain::RFoot].M, bodyparts.foot, -1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::FootRight);
  add(getKinematicChain().theLinks[KinematicChain::LBicep].M, bodyparts.upperArm, 1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::UpperArmLeft);
  add(getKinematicChain().theLinks[KinematicChain::RBicep].M, bodyparts.upperArm, -1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::UpperArmRight);
  add(getKinematicChain().theLinks[KinematicChain::LForeArm].M, bodyparts.lowerArm, 1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::LowerArmLeft);
  add(getKinematicChain().theLinks[KinematicChain::RForeArm].M, bodyparts.lowerArm, -1, getImage().cameraInfo, getCameraMatrix(), getBodyContour(), BodyContour::LowerArmRight);
    
  DEBUG_REQUEST("BodyContourProvider:draw_body_contour_lines",
    if (getBodyContour().lines.size() >= 2)
    {
      for(vector<BodyContour::Line>::const_iterator iter=getBodyContour().lines.begin(); iter!=getBodyContour().lines.end(); iter++)
      {
        LINE_PX(ColorClasses::skyblue,iter->p1.x, iter->p1.y, iter->p2.x, iter->p2.y);
      }// end for
    }// end if
    );
  

  DEBUG_REQUEST("BodyContourProvider:draw_3d_body_contour_feet",
    Vector3<double> lf1 = getKinematicChain().theLinks[KinematicChain::LFoot].M * bodyparts.foot[0];
    for (unsigned int i = 1; i < bodyparts.foot.size(); i++)
    {
      Vector3<double> lf2 = getKinematicChain().theLinks[KinematicChain::LFoot].M * bodyparts.foot[i];
      LINE_3D(ColorClasses::red, lf1, lf2);
      lf1 = lf2;
    }// end for
    Vector3<double> rf1 = getKinematicChain().theLinks[KinematicChain::RFoot].M * Vector3<double>(bodyparts.foot[0].x, bodyparts.foot[0].y * (-1), bodyparts.foot[0].z);
    for (unsigned int i = 1; i < bodyparts.foot.size(); i++)
    {
      Vector3<double> rf2 = getKinematicChain().theLinks[KinematicChain::RFoot].M * Vector3<double>(bodyparts.foot[i].x, bodyparts.foot[i].y * (-1), bodyparts.foot[i].z);
      LINE_3D(ColorClasses::red, rf1, rf2);
      rf1 = rf2;
    }// end for
    );
  DEBUG_REQUEST("BodyContourProvider:draw_3d_body_contour_legs",
      Vector3<double> rl1 = getKinematicChain().theLinks[KinematicChain::RThigh].M * Vector3<double>(bodyparts.upperLeg[0].x, bodyparts.upperLeg[0].y * (-1), bodyparts.upperLeg[0].z);
      for (unsigned int i = 1; i < bodyparts.upperLeg.size(); i++)
      {
        Vector3<double> rl2 = getKinematicChain().theLinks[KinematicChain::RThigh].M * Vector3<double>(bodyparts.upperLeg[i].x, bodyparts.upperLeg[i].y * (-1), bodyparts.upperLeg[i].z);
        LINE_3D(ColorClasses::blue, rl1, rl2);
        rl1 = rl2;
      }// end for
      Vector3<double> ll1 = getKinematicChain().theLinks[KinematicChain::LThigh].M * bodyparts.upperLeg[0];
      for (unsigned int i = 1; i < bodyparts.upperLeg.size(); i++)
      {
        Vector3<double> ll2 = getKinematicChain().theLinks[KinematicChain::LThigh].M * bodyparts.upperLeg[i];
        LINE_3D(ColorClasses::blue, ll1, ll2);
        ll1 = ll2;
      }// end for
      Vector3<double> lknee1 = getKinematicChain().theLinks[KinematicChain::LThigh].M * bodyparts.lowerLeg[0];
      Vector3<double> lknee2 = getKinematicChain().theLinks[KinematicChain::LThigh].M * bodyparts.lowerLeg[1];
      Vector3<double> rknee1 = getKinematicChain().theLinks[KinematicChain::RThigh].M * Vector3<double>(bodyparts.lowerLeg[0].x, bodyparts.lowerLeg[0].y * (-1), bodyparts.lowerLeg[0].z);
      Vector3<double> rknee2 = getKinematicChain().theLinks[KinematicChain::RThigh].M * Vector3<double>(bodyparts.lowerLeg[1].x, bodyparts.lowerLeg[1].y * (-1), bodyparts.lowerLeg[1].z);
      LINE_3D(ColorClasses::blue, lknee1, lknee2);
      LINE_3D(ColorClasses::blue, rknee1, rknee2);
      );

   DEBUG_REQUEST("BodyContourProvider:draw_3d_body_contour_torso",
     Vector3<double> t1 = getKinematicChain().theLinks[KinematicChain::Torso].M * bodyparts.torso[0];
     for (unsigned int i = 1; i < bodyparts.torso.size(); i++)
     {
       Vector3<double> t2 = getKinematicChain().theLinks[KinematicChain::Torso].M * bodyparts.torso[i];
       LINE_3D(ColorClasses::green, t1, t2);
       t1 = t2;
     }// end for
     );

   DEBUG_REQUEST("BodyContourProvider:draw_3d_body_contour_shoulders",
      Vector3<double> rua1 = getKinematicChain().theLinks[KinematicChain::RBicep].M * Vector3<double>(bodyparts.upperArm[0].x, bodyparts.upperArm[0].y * (-1), bodyparts.upperArm[0].z);
      for (unsigned int i = 1; i < bodyparts.upperArm.size(); i++)
      {
        Vector3<double> rua2 = getKinematicChain().theLinks[KinematicChain::RBicep].M * Vector3<double>(bodyparts.upperArm[i].x, bodyparts.upperArm[i].y * (-1), bodyparts.upperArm[i].z);
        LINE_3D(ColorClasses::yellow, rua1, rua2);
        rua1 = rua2;
      }// end for
      Vector3<double> lua1 = getKinematicChain().theLinks[KinematicChain::LBicep].M * bodyparts.upperArm[0];
      for (unsigned int i = 1; i < bodyparts.upperArm.size(); i++)
      {
        Vector3<double> lua2 = getKinematicChain().theLinks[KinematicChain::LBicep].M * bodyparts.upperArm[i];
        LINE_3D(ColorClasses::yellow, lua1, lua2);
        lua1 = lua2;
      }// end for
      );
    DEBUG_REQUEST("BodyContourProvider:draw_3d_body_contour_forearms",
      Vector3<double> rla1 = getKinematicChain().theLinks[KinematicChain::RForeArm].M * Vector3<double>(bodyparts.lowerArm[0].x, bodyparts.lowerArm[0].y * (-1), bodyparts.lowerArm[0].z);
      for (unsigned int i = 1; i < bodyparts.lowerArm.size(); i++)
      {
        Vector3<double> rla2 = getKinematicChain().theLinks[KinematicChain::RForeArm].M * Vector3<double>(bodyparts.lowerArm[i].x, bodyparts.lowerArm[i].y * (-1), bodyparts.lowerArm[i].z);
        LINE_3D(ColorClasses::orange, rla1, rla2);
        rla1 = rla2;
      }// end for
      Vector3<double> lla1 = getKinematicChain().theLinks[KinematicChain::LForeArm].M * bodyparts.lowerArm[0];
      for (unsigned int i = 1; i < bodyparts.lowerArm.size(); i++)
      {
        Vector3<double> lla2 = getKinematicChain().theLinks[KinematicChain::LForeArm].M * bodyparts.lowerArm[i];
        LINE_3D(ColorClasses::orange, lla1, lla2);
        lla1 = lla2;
      }// end for
   );
   DEBUG_REQUEST("BodyContourProvider:draw_activated_cells",
     for(int i = 0; i < getBodyContour().xDensity; i++)
     {
       for (int j = 0; j < getBodyContour().yDensity; j++)
       {
         if (getBodyContour().grid[i][j].occupied)
         {
            IMAGE_DRAWING_CONTEXT;
            RECT_PX(ColorClasses::black, i*getBodyContour().stepSize, j*getBodyContour().stepSize, (i+1)*getBodyContour().stepSize, (j+1)*getBodyContour().stepSize);
         }
       }
     }
     );
}// end execute

// TODO: Comment
inline bool BodyContourProvider::withinImage(const Vector2<double> point, const CameraInfo& cameraInfo)
{
  if (point.x >= 0 && point.x <= cameraInfo.resolutionWidth
      && point.y >= 0 && point.y <= cameraInfo.resolutionHeight)
  {
    return true;
  } 
  else
  {
    return false;
  }
}
// TODO: Comment
inline void BodyContourProvider::initializeGrid()
{
  for(int i = 0; i < getBodyContour().xDensity; i++)
  {
    for (int j = 0; j < getBodyContour().yDensity; j++)
    {
      getBodyContour().grid[i][j].occupied = false;
      getBodyContour().grid[i][j].lineNumbers = -1;
    }
  }
}

// TODO: Comment

inline void BodyContourProvider::eraseGrid()
{
  getBodyContour().grid.clear();
}

// TODO:Comment
inline void BodyContourProvider::cellPos(BodyContour::Line line, Vector2<int>& firstCell, Vector2<int>& secondCell)
{
   firstCell.x = line.p1.x / getBodyContour().stepSize;
   firstCell.y = line.p1.y / getBodyContour().stepSize;
   secondCell.x = line.p2.x / getBodyContour().stepSize;
   secondCell.y = line.p2.y / getBodyContour().stepSize;
   if (firstCell.x == getBodyContour().xDensity)
   {
     firstCell.x = firstCell.x - 1;
   }
   if (firstCell.y == getBodyContour().yDensity)
   {
     firstCell.y = firstCell.y - 1;
   }
   if (firstCell.x == getBodyContour().xDensity && firstCell.y == getBodyContour().yDensity)
   {
     firstCell.x = firstCell.x - 1;
     firstCell.y = firstCell.y - 1;
   }
   if (secondCell.x == getBodyContour().xDensity)
   {
     secondCell.x = secondCell.x - 1;
   }
   if (secondCell.y == getBodyContour().yDensity)
   {
     secondCell.y = secondCell.y - 1;
   }
   if (secondCell.x == getBodyContour().xDensity && secondCell.y == getBodyContour().yDensity)
   {
     secondCell.x = secondCell.x - 1;
     secondCell.y = secondCell.y - 1;
   }
}

inline void BodyContourProvider::cellPos(Vector2<int> point, Vector2<int>& cell)
{
  cell.x = point.x / getBodyContour().stepSize;
  cell.y = point.y / getBodyContour().stepSize;
  if (cell.x == getBodyContour().xDensity)
  {
    cell.x = cell.x - 1;
  }
  if (cell.y == getBodyContour().yDensity)
  {
    cell.y = cell.y - 1;
  }
  if (cell.x == getBodyContour().xDensity && cell.y == getBodyContour().yDensity)
  {
    cell.x = cell.x - 1;
    cell.y = cell.y - 1;
  }
}

// TODO: Comment
inline void BodyContourProvider::setCells(BodyContour::Line line)
{
  Vector2<int> firstCell, secondCell;
  cellPos(line, firstCell, secondCell);
  int lineNumber = line.lineNumber;
  int F, x, y;
  int p1x = firstCell.x;
  int p1y = firstCell.y;
  int p2x = secondCell.x;
  int p2y = secondCell.y;
  if (p1x > p2x)  // Swap points if p1 is on the right of p2
  {
    swap(p1x, p2x);
    swap(p1y, p2y);
  }
  // Handle trivial cases separately for algorithm speed up.
  // Trivial case 1: m = +/-INF (Vertical line)
  if (p1x == p2x)
  {
    if (p1y > p2y)  // Swap y-coordinates if p1 is above p2
    {
      swap(p1y, p2y);
    }
    x = p1x;
    y = p1y;
    while (y < getBodyContour().yDensity)
    {
      getBodyContour().grid[x][y].lineNumbers = lineNumber;
      getBodyContour().grid[x][y].occupied = true;
      y++;
    }
    return;
  }
  // Trivial case 2: m = 0 (Horizontal line)
  else if (p1y == p2y)
  {
    x = p1x;
    y = p1y;
    for (int j = x; j <= p2x; j++)
    {
      for (int i = y; i < getBodyContour().yDensity; i++)
      {
        getBodyContour().grid[j][i].lineNumbers = lineNumber;
        getBodyContour().grid[j][i].occupied = true;
      }
    }
    return;
  }
  int dy            = p2y - p1y;  // y-increment from p1 to p2
  int dx            = p2x - p1x;  // x-increment from p1 to p2
  int dy2           = (dy << 1);  // dy << 1 == 2*dy
  int dx2           = (dx << 1);
  int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
  int dy2_plus_dx2  = dy2 + dx2;
  if (dy >= 0)    // m >= 0
  {
    // Case 1: 0 <= m <= 1 (Original case)
    if (dy <= dx)   
    {
      F = dy2 - dx;    // initial F
      x = p1x;
      y = p1y;
      for (int a = x; a <= p2x; a++)
      {
        getBodyContour().grid[a][y].lineNumbers = lineNumber;
        getBodyContour().grid[a][y].occupied = true;
        for (int c = y+1; c < getBodyContour().yDensity; c++)
        {
          getBodyContour().grid[a][c].lineNumbers = lineNumber;
          getBodyContour().grid[a][c].occupied = true;
        }
        if (F <= 0)
        {
          F += dy2;
        }
        else
        {
          y++;
          F += dy2_minus_dx2;
        }
      } // end for
    } // end if
    // Case 2: 1 < m < INF (Mirror about y=x line
    // replace all dy by dx and dx by dy)
    else
    {
      F = dx2 - dy;    // initial F
      y = p1y;
      x = p1x;
      while (y <= p2y)
      {
        getBodyContour().grid[x][y].lineNumbers = lineNumber;
        getBodyContour().grid[x][y].occupied = true;
        for (int c = y+1; c < getBodyContour().yDensity; c++)
        {
          getBodyContour().grid[x][c].lineNumbers = lineNumber;
          getBodyContour().grid[x][c].occupied = true;
        }
        if (F <= 0)
        {
          F += dx2;
        }
        else
        {
          x++;
          F -= dy2_minus_dx2;
        }
        y++;
      } // end while
    } // end else
  } // end if
  else    // m < 0
  {
    // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
    if (dx >= -dy)
    {
      F = -dy2 - dx;    // initial F
      x = p1x;
      y = p1y;
      while (x <= p2x)
      {
        getBodyContour().grid[x][y].lineNumbers = lineNumber;
        getBodyContour().grid[x][y].occupied = true;
        for (int c = y+1; c < getBodyContour().yDensity; c++)
        {
          getBodyContour().grid[x][c].lineNumbers = lineNumber;
          getBodyContour().grid[x][c].occupied = true;
        }
        if (F <= 0)
        {
          F -= dy2;
        }
        else
        {
          y--;
          F -= dy2_plus_dx2;
        }
        x++;
      }
    }
    // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
    // about y=x line, replace all dx by -dy and dy by dx)
    else    
    {
      F = dx2 + dy;    // initial F
      y = p1y;
      x = p1x;
      while (y >= p2y)
      {
        getBodyContour().grid[x][y].lineNumbers = lineNumber;
        getBodyContour().grid[x][y].occupied = true;
        for (int c = y+1; c < getBodyContour().yDensity; c++)
        {
          getBodyContour().grid[x][c].lineNumbers = lineNumber;
          getBodyContour().grid[x][c].occupied = true;
        }
        if (F <= 0)
        {
          F += dx2;
        }
        else
        {
          x++;
          F += dy2_plus_dx2;
        }
        y--;
      }
    }
  }
}

// TODO: Comment

inline void BodyContourProvider::pushLine(BodyContour::Line line, BodyContour& bodyContour)
{
  Vector2<int> cell1, cell2;
  int cell1Line, cell2Line, point1Line, point2Line;
  bool cell1occ, cell2occ;
  cellPos(line.p1, cell1);
  cellPos(line.p2, cell2);
  cell1Line = getBodyContour().grid[cell1.x][cell1.y].lineNumbers;
  cell1occ = getBodyContour().grid[cell1.x][cell1.y].occupied;
  cell2Line = getBodyContour().grid[cell2.x][cell2.y].lineNumbers;
  cell2occ = getBodyContour().grid[cell2.x][cell2.y].occupied;
  if (cell1occ)
  {
    point1Line = bodyContour.lines[cell1Line].id;
  } 
  else
  {
    point1Line = -1;
  }
  if (cell2occ)
  {
    point2Line = bodyContour.lines[cell2Line].id;
  } 
  else
  {
    point2Line = -1;
  }
  if (cell1occ && cell2occ && point1Line != line.id && point2Line != line.id)
  {
    return;
  }
  else
  {
    bodyContour.lines.push_back(line);
    setCells(line);
    lineNumber++;
  }
}

// TODO: Comment
inline void BodyContourProvider::add(const Pose3D& origin, const std::vector<Vector3<double> >& c, float sign, 
                              const CameraInfo& cameraInfo,  const CameraMatrix& cameraMatrix,
                              BodyContour& bodyContour, BodyContour::bodyPartID id)
{
  // some variables
  const Vector2<int> frameUpperLeft(0,0);
  const Vector2<int> frameLowerRight(cameraInfo.resolutionWidth-1, cameraInfo.resolutionHeight-1);
  
  Vector2<int> q1, q2, tempPoint(0,0);
  
  // estimate the position of contour-point based on current position of the limb
  Vector3<double> p1 = origin * Vector3<double>(c[0].x, c[0].y * sign, c[0].z);
  // project this point into the image
  q1 = CameraGeometry::relativePointToImage(cameraMatrix, cameraInfo, p1);
  // start building the contour lines
  for(unsigned i = 1; i < c.size(); i++)
  {
    Vector3<double> p2 = origin * Vector3<double>(c[i].x, c[i].y * sign, c[i].z);
    q2 = CameraGeometry::relativePointToImage(cameraMatrix, cameraInfo, p2);

    BodyContour::Line tempLine = BodyContour::Line(q1, q2, lineNumber, id);

    // estimate, whether the points are within the image-boundaries
    bool p1InImage = withinImage(tempLine.p1, cameraInfo);
    bool p2InImage = withinImage(tempLine.p2, cameraInfo);
    // if the points aren't behind the robot and at least one point
    // is within image boundaries
    if (q1.x != -1 && q2.x != -1 && (p1InImage || p2InImage))
    {
      if (p1InImage && p2InImage )
      {
        pushLine(tempLine, bodyContour);
      }//end if
      //at least one point is not image
      else
      {
        //temporary points
        Vector2<int> pointOne;
        Vector2<int> pointTwo;
        Math::LineSegment segment(tempLine.p1, tempLine.p2);
        //the first point is not in image
        if (!p1InImage)
        {
          Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, segment, pointOne, pointTwo);
          BodyContour::Line tempLine2(pointOne, pointTwo, lineNumber, id);
          pushLine(BodyContour::Line(tempLine2.p1, tempLine.p2, lineNumber, id), bodyContour);
        }
        else
        {
          Geometry::getIntersectionPointsOfLineAndRectangle(frameUpperLeft, frameLowerRight, segment, pointOne, pointTwo);
          BodyContour::Line tempLine2(pointOne, pointTwo, lineNumber, id);
          pushLine(BodyContour::Line(tempLine.p1, tempLine2.p2, lineNumber, id), bodyContour);
        }
      }//end else
    }//end if
  p1 = p2;
  q1 = q2;
  }// end for
}// end add