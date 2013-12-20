/**
* @file GoalDetector.cpp
*
* Implementation of class GoalDetector
*
*/

#include "GoalDetector.h"

#include "Tools/CameraGeometry.h"
#include <Tools/Debug/DebugBufferedOutput.h>

#include <algorithm>


GoalDetector::GoalDetector()
: 
  cameraID(CameraInfo::Top),
  blobFinder(getColoredGridTop()),
  blobFinderBottom(getColoredGrid())
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:TopCam:mark_color_scans", "mark the results of the color scan algorithm", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:BottomCam:mark_color_scans", "mark the results of the color scan algorithm", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:TopCam:mark_post_scans", "mark the scanlines along the posts", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:BottomCam:mark_post_scans", "mark the scanlines along the posts", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:TopCam:spiderSearch", "enable output for spider search", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:BottomCam:spiderSearch", "enable output for spider search", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:TopCam:draw_blobs", "draw search blobs", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:BottomCam:draw_blobs", "draw search blobs", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:TopCam:mark_goal", "mark the goal on the getImage()", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:BottomCam:mark_goal", "mark the goal on the getImage()", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:TopCam:back_proj_test", "...", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:GoalDetector:BottomCam:back_proj_test", "...", false);
}

void GoalDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getGoalPercept().reset();

  //if there is no field percept, then, there is also no goal ?!
  /*
  if(!theFieldPercept.isValid())
  {
    return;
  }
  */

  // estimate the horizon
  Vector2<double> p1(getArtificialHorizon().begin());
  Vector2<double> p2(getArtificialHorizon().end());
  int heightOfHorizon = (int)((p1.y + p2.y) * 0.5 + 0.5);

  // image over the horizon
  if(heightOfHorizon > (int)getImage().cameraInfo.resolutionHeight-10) return;

  // clamp the scanline
  p1.y = Math::clamp((int)p1.y, 10, (int)getImage().cameraInfo.resolutionHeight-10);
  p2.y = Math::clamp((int)p2.y, 10, (int)getImage().cameraInfo.resolutionHeight-10);


  // calculate the post candidates along of the horizon
  Candidate candidates[maxNumberOfCandidates];
  int numberOfCandidates = scanForCandidates(p1, p2, candidates);
  
  // fallback: try to scan along the center of the image
  if(numberOfCandidates == 0)
  {
    Vector2<double> c1(0,getImage().cameraInfo.getOpticalCenterY());
    Vector2<double> c2(getImage().cameraInfo.resolutionWidth-1,getImage().cameraInfo.getOpticalCenterY());
    numberOfCandidates = scanForCandidates(c1, c2, candidates);
  }//end if

  // try once again...
  if(numberOfCandidates == 0)
  {
    Vector2<double> c1(0,getImage().cameraInfo.resolutionHeight/3);
    Vector2<double> c2(getImage().cameraInfo.resolutionWidth-1,getImage().cameraInfo.resolutionHeight/3);
    numberOfCandidates = scanForCandidates(c1, c2, candidates);
  }//end if


  // estimate the post base points
  vector<GoalPercept::GoalPost> postvector;
  estimatePostsByScanlines(candidates, numberOfCandidates, postvector);
  //estimatePostsByBlobs(candidates, numberOfCandidates, postvector);


  if(postvector.empty()) return;

  // sort the posts by height in the image
  // the first is the biggest
  GoalPercept::GoalPost post; // only for comparison
  sort(postvector.begin(), postvector.end(), post); 



  // minimal height (in px) of an accepted goal post
  int minimalHeightOfAGoalPost = 20;
  int numberOfPostsFound = 0;

  // we are searching for two goal posts
  GoalPercept::GoalPost postOne;
  GoalPercept::GoalPost postTwo;

  int distToBottomImage = getImage().cameraInfo.resolutionHeight - max(postOne.basePoint.y, postTwo.basePoint.y) - 1;
  // look max 5 pixel down
  Vector2<int> extraBase(0, min(distToBottomImage,5));

  // if the longest post is to short
  if(postvector.begin()->seenHeight < minimalHeightOfAGoalPost)
  {
    return;
  }


  // index of the first found post
  unsigned int idxFirst = 0;
  // search for the first goal post candidate
  for(idxFirst = 0; idxFirst < postvector.size(); idxFirst++)
  {
    if(postvector[idxFirst].seenHeight >= minimalHeightOfAGoalPost)
       // consider the field percept ONLY if it is valid
       //&& theFieldPercept.getLargestValidPoly(getCameraMatrix().horizon).isInside(postvector[idxFirst].basePoint + extraBase))
    {
      numberOfPostsFound = 1;
      break;
    }
  }//end for


  // if a post were found search for the second one
  if(numberOfPostsFound > 0)
  {
    // at least one post was seen
    postOne = postvector[idxFirst];
    idxFirst++;

    for(unsigned int i = idxFirst; i < postvector.size(); i++)
    {
      postTwo = postvector[i];

      if( abs(postTwo.basePoint.x - postOne.basePoint.x) > 50 &&
          postTwo.seenHeight > minimalHeightOfAGoalPost &&
          postOne.color == postTwo.color) //&& // posts have the same color
          // consider the field percept ONLY if it is valid
          //theFieldPercept.getLargestValidPoly(getCameraMatrix().horizon).isInside(postvector[i].basePoint + extraBase))
      {
        numberOfPostsFound = 2;
        break;
      }//end if
    }//end for
  }//end if first found



  if(numberOfPostsFound > 1)
  {
    // sort: which one is left or right
    if(postOne.basePoint.x > postTwo.basePoint.x)
    {
      postOne.type = GoalPercept::GoalPost::rightPost;
      postTwo.type = GoalPercept::GoalPost::leftPost;
    }else
    {
      postOne.type = GoalPercept::GoalPost::leftPost;
      postTwo.type = GoalPercept::GoalPost::rightPost;
    }


    //TODO: handle the case if the projection is not possible
    // position on the ground is not reliable if the post cannot be projected
    postOne.positionReliable =
      CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        postOne.basePoint.x, postOne.basePoint.y, 0.0,
        postOne.position);

    //TODO: handle the case if the projection is not possible
    // position on the ground is not reliable if the post cannot be projected
    postTwo.positionReliable = 
      CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        postTwo.basePoint.x, postTwo.basePoint.y, 0.0,
        postTwo.position);

    //TODO: try to calculate the position by the top of the post

    // translate by the post radius
    postOne.position.normalize(postOne.position.abs() + getFieldInfo().goalpostRadius);
    postTwo.position.normalize(postTwo.position.abs() + getFieldInfo().goalpostRadius);

    postOne.positionReliable = postOne.positionReliable && checkIfPostReliable(postOne.basePoint);
    postTwo.positionReliable = postTwo.positionReliable && checkIfPostReliable(postTwo.basePoint);

    getGoalPercept().add(postOne);
    getGoalPercept().add(postTwo);
  }
  else if(numberOfPostsFound > 0) // only one post found
  {
    //TODO: handle the case if the projection is not possible
    postOne.positionReliable = 
      CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        postOne.basePoint.x, postOne.basePoint.y, 0.0,
        postOne.position);

    postOne.positionReliable = postOne.positionReliable && checkIfPostReliable(postOne.basePoint);

    postOne.type = GoalPercept::GoalPost::unknownPost;
    getGoalPercept().add(postOne);
  }//end else




  /*************************************************/
  // calculate the centroid
  // at least one post was seen
  // TODO: clean it up (calculation of the centroid)
  if(getGoalPercept().getNumberOfSeenPosts() > 0)
  {

    //Vector2<double> centroid;
    //double mainAxisAngle = -getMajorAxis( goalColor, centroid);

    Vector2<double> centroid = getGoalPercept().getPost(0).basePoint -
      Vector2<double>(0.0, getGoalPercept().getPost(0).seenHeight*0.5); 

    if(getGoalPercept().getNumberOfSeenPosts() > 1)
    {
      centroid += getGoalPercept().getPost(1).basePoint -
            Vector2<double>(0.0, getGoalPercept().getPost(1).seenHeight*0.5);

      centroid /= 2.0;
    }//end if

    Vector2<double> angles = CameraGeometry::angleToPointInImage(
            getCameraMatrix(), 
            getImage().cameraInfo,
            (int)centroid.x, 
            (int)centroid.y);

    getGoalPercept().angleToSeenGoal = angles.x;

    getGoalPercept().goalCentroid = CameraGeometry::imagePixelToWorld(
            getCameraMatrix(), 
            getImage().cameraInfo,
            centroid.x,
            centroid.y,
            3000.0);

    DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
      if(cameraID == CameraInfo::Top)
      {
        TOP_CIRCLE_PX(ColorClasses::red, (int)centroid.x, (int)centroid.y, 5);
      }
    );
    
    DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
      if(cameraID == CameraInfo::Bottom)
      {
        CIRCLE_PX(ColorClasses::red, (int)centroid.x, (int)centroid.y, 5);
      }
    );
  }//end if
  /************************************************/



  /////////////////////////////////////////////////////////
  // do some drawings
  DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
    if(cameraID == CameraInfo::Top)
    {
      for(int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
      {
        const GoalPercept::GoalPost& post = getGoalPercept().getPost(i);
        ColorClasses::Color drawingColor;

        if(post.type == GoalPercept::GoalPost::leftPost && post.positionReliable)
          drawingColor=ColorClasses::gray;
        else if (post.type == GoalPercept::GoalPost::rightPost && post.positionReliable)
          drawingColor=ColorClasses::white;
        else if (post.type == GoalPercept::GoalPost::unknownPost && post.positionReliable)
          drawingColor=ColorClasses::pink;
        else 
          drawingColor=ColorClasses::red;

        TOP_LINE_PX(drawingColor, post.topPoint.x, post.topPoint.y, post.basePoint.x, post.basePoint.y);
        TOP_CIRCLE_PX(drawingColor, post.topPoint.x, post.topPoint.y, 3);
        TOP_CIRCLE_PX(post.color, post.topPoint.x, post.topPoint.y, 2);
        TOP_CIRCLE_PX(drawingColor, post.basePoint.x, post.basePoint.y, 3);
        TOP_CIRCLE_PX(post.color, post.basePoint.x, post.basePoint.y, 2);
      }//end for
    }
  );

  DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
    if(cameraID == CameraInfo::Bottom)
    {
      for(int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
      {
        const GoalPercept::GoalPost& post = getGoalPercept().getPost(i);
        ColorClasses::Color drawingColor;

        if(post.type == GoalPercept::GoalPost::leftPost && post.positionReliable)
          drawingColor=ColorClasses::gray;
        else if (post.type == GoalPercept::GoalPost::rightPost && post.positionReliable)
          drawingColor=ColorClasses::white;
        else if (post.type == GoalPercept::GoalPost::unknownPost && post.positionReliable)
          drawingColor=ColorClasses::pink;
        else 
          drawingColor=ColorClasses::red;

        LINE_PX(drawingColor, post.topPoint.x, post.topPoint.y, post.basePoint.x, post.basePoint.y);
        CIRCLE_PX(drawingColor, post.topPoint.x, post.topPoint.y, 3);
        CIRCLE_PX(post.color, post.topPoint.x, post.topPoint.y, 2);
        CIRCLE_PX(drawingColor, post.basePoint.x, post.basePoint.y, 3);
        CIRCLE_PX(post.color, post.basePoint.x, post.basePoint.y, 2);
      }//end for
    }
  );

  DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:back_proj_test",
    if(cameraID == CameraInfo::Top)
    {
      static GoalPercept::GoalPost lastGoalPost;
      static unsigned int lastFrame = 0;

      if(getGoalPercept().getNumberOfSeenPosts() > 0)
      {
        if(getFrameInfo().getFrameNumber() - lastFrame == 1)
        {
          double differenceInImage = (lastGoalPost.basePoint - getGoalPercept().getPost(0).basePoint).abs();
          PLOT("GoalDetector:differenceInImage", differenceInImage);

          // back projection test
          Vector3<double> localPostPosition3d(lastGoalPost.position.x, lastGoalPost.position.y, 0.0);
          Vector2<int> projectedPost;
          
          if(CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo,
                                                  localPostPosition3d, projectedPost))
          {
            double backPojDifferenceInImage = (projectedPost - getGoalPercept().getPost(0).basePoint).abs();
            PLOT("GoalDetector:backPojDifferenceInImage", backPojDifferenceInImage);
          }
        }//end if

        lastGoalPost = getGoalPercept().getPost(0);
        lastFrame = getFrameInfo().getFrameNumber();
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:back_proj_test",
    if(cameraID == CameraInfo::Bottom)
    {
      static GoalPercept::GoalPost lastGoalPost;
      static unsigned int lastFrame = 0;

      if(getGoalPercept().getNumberOfSeenPosts() > 0)
      {
        if(getFrameInfo().getFrameNumber() - lastFrame == 1)
        {
          double differenceInImage = (lastGoalPost.basePoint - getGoalPercept().getPost(0).basePoint).abs();
          PLOT("GoalDetector:differenceInImage", differenceInImage);

          // back projection test
          Vector3<double> localPostPosition3d(lastGoalPost.position.x, lastGoalPost.position.y, 0.0);
          Vector2<int> projectedPost;
          
          if(CameraGeometry::relativePointToImage(getCameraMatrix(), getImage().cameraInfo,
                                                  localPostPosition3d, projectedPost))
          {
            double backPojDifferenceInImage = (projectedPost - getGoalPercept().getPost(0).basePoint).abs();
            PLOT("GoalDetector:backPojDifferenceInImage", backPojDifferenceInImage);
          }

          double backPojDifferenceInImage = (projectedPost - getGoalPercept().getPost(0).basePoint).abs();
          PLOT("GoalDetector:backPojDifferenceInImage", backPojDifferenceInImage);
        }//end if

        lastGoalPost = getGoalPercept().getPost(0);
        lastFrame = getFrameInfo().getFrameNumber();
      }
    }
  );

}//end horizonScan


Vector2<int> GoalDetector::extendCandidate(ColorClasses::Color color, const Vector2<int>& start)
{
  Vector2<double> p1, p2;
  p1 = getArtificialHorizon().begin();
  p2 = getArtificialHorizon().end();
  
  Vector2<int> direction(p2 - p1);
  
  Vector2<int> pointOne = scanColorLine(color, start, direction);
  Vector2<int> pointTwo = scanColorLine(color, start, -direction);

  return (pointOne + pointTwo)/2;
}//end extendCandidate


void GoalDetector::estimatePostsByScanlines(
  const Candidate (&candidates)[maxNumberOfCandidates],
  int numberOfCandidates,
  vector<GoalPercept::GoalPost>& postvector)
{
  
  // find the base points
  for(int i = 0; i < numberOfCandidates; i++)
  {
    // orthogonal to horizon (down)
    // TODO: check rounding
    Vector2<int> directionDown((getArtificialHorizon().end() - getArtificialHorizon().begin()).rotateLeft());
    Vector2<int> directionUp(-directionDown);
    Vector2<int> basePoint = scanColorLine(candidates[i].color, candidates[i].point, directionDown);
    Vector2<int> topPoint = scanColorLine(candidates[i].color, candidates[i].point, directionUp);

    if((basePoint - candidates[i].point).abs() > 0)
    {
      Vector2<int> centeredBasePoint = extendCandidate(candidates[i].color, basePoint);

      if((centeredBasePoint - candidates[i].point).abs() > 0)
      {
        directionDown = centeredBasePoint - candidates[i].point;
        directionUp = -directionDown;
        // rescan
        Vector2<int> correctedBasePoint = scanColorLine(candidates[i].color, candidates[i].point, directionDown);
        Vector2<int> correctedTopPoint = scanColorLine(candidates[i].color, candidates[i].point, directionUp);

        if(correctedBasePoint.y >= basePoint.y)
          basePoint = correctedBasePoint;
        if(correctedTopPoint.y <= topPoint.y)
          topPoint = correctedTopPoint;
      }//end if
    }//end if

    // create new post
    GoalPercept::GoalPost post;
    post.color = candidates[i].color;
    post.basePoint = basePoint;
    post.topPoint = topPoint;
    post.seenHeight = (topPoint - basePoint).abs();

    postvector.push_back(post);

    DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_post_scans",
      if(cameraID == CameraInfo::Top)
      {
        TOP_CIRCLE_PX(ColorClasses::green, basePoint.x, basePoint.y, 2);
        TOP_CIRCLE_PX(ColorClasses::red, topPoint.x, topPoint.y, 2);
      }
    );
    DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_post_scans",
      if(cameraID == CameraInfo::Bottom)
      {
        CIRCLE_PX(ColorClasses::green, basePoint.x, basePoint.y, 2);
        CIRCLE_PX(ColorClasses::red, topPoint.x, topPoint.y, 2);
      }
    );
  }//end for
}//end estimatePostsByScanlines



Vector2<int> GoalDetector::scanColorLine(
  ColorClasses::Color color, 
  const Vector2<int>& start, 
  const Vector2<int>& direction)
{
  // TODO: make parameter
  int maxNumberOfSkippedPixel = 10;

  BresenhamLineScan scanLine(start, direction, getImage().cameraInfo);
  int nonColorCount = 0;
  Vector2<int> point(start);
  Vector2<int> end(start);

  for(int k = 0; k < scanLine.numberOfPixels; k++)
  {
    scanLine.getNext(point);
    Pixel pixel = getImage().get(point.x, point.y);
    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);

    DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_color_scans",
      if(cameraID == CameraInfo::Top)
      {
        TOP_POINT_PX(ColorClasses::green, point.x, point.y);
      }
    );

    DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_color_scans",
      if(cameraID == CameraInfo::Bottom)
      {
        POINT_PX(ColorClasses::green, point.x, point.y);
      }
    );

    if( currentPixelColor == color )
      end = point;
    else if(nonColorCount < maxNumberOfSkippedPixel)
      nonColorCount++;
    else
      break;
  }//end for

  return end;
}//end scanColorLine



void GoalDetector::estimatePostsByBlobs(
  const Candidate (&candidates)[maxNumberOfCandidates],
  int numberOfCandidates,
  vector<GoalPercept::GoalPost>& postvector)
{
  int maxNumberOfSkippedPixel = 10;

  for(int i = 0; i < numberOfCandidates; i++)
  {

    Vector2<int> candidate(candidates[i].point);
    
    Blob blob = spiderExpandArea(
      candidate, 
      candidates[i].color, 
      maxNumberOfSkippedPixel, 
      320);

    // draw expanded blob
    DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:draw_blobs",
      if(cameraID == CameraInfo::Top)
      {
        TOP_LINE_PX(ColorClasses::green,
          blob.vertices[7].x, blob.vertices[7].y, blob.vertices[0].x, blob.vertices[0].y);
        for(int i = 0; i < 7; i++)
        {
          TOP_LINE_PX(ColorClasses::green,
            blob.vertices[i].x, blob.vertices[i].y, blob.vertices[i+1].x, blob.vertices[i+1].y);
        }
        Vector2<int> meanPoint = blob.vertices.getMean();
        TOP_CIRCLE_PX(ColorClasses::green, meanPoint.x, meanPoint.y, 2);
      }
    );

    DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:draw_blobs",
      if(cameraID == CameraInfo::Bottom)
      {
        LINE_PX(ColorClasses::green,
          blob.vertices[7].x, blob.vertices[7].y, blob.vertices[0].x, blob.vertices[0].y);
        for(int i = 0; i < 7; i++)
        {
          LINE_PX(ColorClasses::green,
            blob.vertices[i].x, blob.vertices[i].y, blob.vertices[i+1].x, blob.vertices[i+1].y);
        }
        Vector2<int> meanPoint = blob.vertices.getMean();
        CIRCLE_PX(ColorClasses::green, meanPoint.x, meanPoint.y, 2);
      }
    );

    Vector2<int> leftBottom = blob.getClosestPoint(Vector2<int>(0, getImage().cameraInfo.resolutionHeight));
    Vector2<int> rightBottom = blob.getClosestPoint(Vector2<int>(getImage().cameraInfo.resolutionWidth, getImage().cameraInfo.resolutionHeight));

    Vector2<int> leftTop = blob.getClosestPoint(Vector2<int>(0, 0));
    Vector2<int> rightTop = blob.getClosestPoint(Vector2<int>(getImage().cameraInfo.resolutionWidth, 0));
  
    DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
      if(cameraID == CameraInfo::Top)
      {
        TOP_RECT_PX(ColorClasses::green, leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);
      }
    );

    DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
      if(cameraID == CameraInfo::Bottom)
      {
        RECT_PX(ColorClasses::green, leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);
      }
    );

    double height = std::max(leftBottom.y, rightBottom.y) - std::min(leftTop.y, rightTop.y);

    GoalPercept::GoalPost post;
    post.basePoint = (leftBottom + rightBottom)/2;
    post.seenHeight = height;
    post.color = candidates[i].color;

    postvector.push_back(post);
  }//end for

}//end estimatePostsByBlobs



int GoalDetector::scanForCandidates(
  const Vector2<int>& startPoint, 
  const Vector2<int>& endPoint, 
  GoalDetector::Candidate (&candidates)[maxNumberOfCandidates])
{

  int maxNumberOfSkippedPixel = 10;
  Vector2<double> runSum;
  double numOfPoints = 0;
  int nonColorCount = 0;
  // used for rounding
  Vector2<double> half(0.5,0.5);
  int idx = 0;

  BresenhamLineScan scanLine(startPoint, endPoint);
  Vector2<int> point(startPoint);

  for(int i = 0; i < scanLine.numberOfPixels; i++)
  {
    scanLine.getNext(point);
    Pixel pixel = getImage().get(point.x, point.y);
    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);

//    double angleBegin = 0;
//    double angleEnd = 0;
    
    /*
    DEBUG_REQUEST("ImageProcessor:GoalDetector:mark_goal",
      POINT_PX(ColorClasses::black,  point.x, getImage().cameraInfo.resolutionHeight-1 - pixel.y);
      POINT_PX(ColorClasses::yellow, point.x, getImage().cameraInfo.resolutionHeight-1 - pixel.u);
      POINT_PX(ColorClasses::blue,   point.x, getImage().cameraInfo.resolutionHeight-1 - pixel.v);
    );
    */

    if( currentPixelColor == ColorClasses::skyblue || 
        currentPixelColor == ColorClasses::yellow )
    {
      // first point of goal color
//      if(numOfPoints == 0)
//      {
//        angleBegin = getPointsAngle(point);
//      }

      runSum += point;
      numOfPoints++;
      candidates[idx].color = currentPixelColor;
      
      DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
        if(cameraID == CameraInfo::Top)
        {
          TOP_POINT_PX(currentPixelColor, point.x, point.y);
        }
      );
      DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
        if(cameraID == CameraInfo::Bottom)
        {
          POINT_PX(currentPixelColor, point.x, point.y);
        }
      );
    }
    else
    {
      // last point of goal color
      if(numOfPoints > 0 && nonColorCount > maxNumberOfSkippedPixel)
      {
        //angleEnd = getPointsAngle(point);
        //calculateMeanAngle(angleBegin, angleEnd);
        candidates[idx].width = numOfPoints;
        candidates[idx].point = (runSum/numOfPoints + half);
        candidates[idx].angle = getPointsAngle(candidates[idx].point);
        
        DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
          if(cameraID == CameraInfo::Top)
          {
            TOP_CIRCLE_PX(currentPixelColor, candidates[idx].point.x, candidates[idx].point.y, 2);
            // draw the estimated angle
            TOP_LINE_PX(ColorClasses::red, 
              candidates[idx].point.x,
              candidates[idx].point.y,
              candidates[idx].point.x + (int)(candidates[idx].width * cos(candidates[idx].angle)*2) ,
              candidates[idx].point.y + (int)(candidates[idx].width * sin(candidates[idx].angle)*2));
            TOP_POINT_PX(ColorClasses::red, point.x, point.y);
          }
        );

        DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
          if(cameraID == CameraInfo::Bottom)
          {
            CIRCLE_PX(currentPixelColor, candidates[idx].point.x, candidates[idx].point.y, 2);
            // draw the estimated angle
            LINE_PX(ColorClasses::red, 
              candidates[idx].point.x,
              candidates[idx].point.y,
              candidates[idx].point.x + (int)(candidates[idx].width * cos(candidates[idx].angle)*2) ,
              candidates[idx].point.y + (int)(candidates[idx].width * sin(candidates[idx].angle)*2));
            POINT_PX(ColorClasses::red, point.x, point.y);
          }
        );

        nonColorCount = 0;
        numOfPoints = 0;
        runSum.x = 0.0;
        runSum.y = 0.0;
        idx++;
      }
      else
      {
        DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
          if(cameraID == CameraInfo::Top)
          {
            TOP_POINT_PX(ColorClasses::gray, point.x, point.y);
          }
        );

        DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
          if(cameraID == CameraInfo::Bottom)
          {
            POINT_PX(ColorClasses::gray, point.x, point.y);
          }
        );
        nonColorCount++;
      }
    }//end else

    if(idx >= maxNumberOfCandidates) break;
  }//end for

  return idx;
}//end scanAlongHorizon



bool GoalDetector::checkIfPostReliable(Vector2<int>& post)
{
  int searchLength = 15;

  if(post.x < 0 || post.y < 0 || 
     post.x >= (int)getImage().cameraInfo.resolutionWidth ||
     post.y >= (int)getImage().cameraInfo.resolutionHeight)
    return false;

  for(int i = post.y; i < std::min((int)getImage().cameraInfo.resolutionHeight-1, post.y+searchLength); i++)
  {
    Pixel pixel = getImage().get(post.x,i);
    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);

    DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
      if(cameraID == CameraInfo::Top)
      {
        TOP_POINT_PX(currentPixelColor, post.x, i);
      }
    );

    DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
      if(cameraID == CameraInfo::Bottom)
      {
        POINT_PX(currentPixelColor, post.x, i);
      }
    );

    if(currentPixelColor == ColorClasses::green)
      return true;
  }//end for

  return false;
}//end checkIfPostReliable


double GoalDetector::getMajorAxis(const ColorClasses::Color goalColor, Vector2<double>& centroid)
{

  int numberOfPoints=getColoredGrid().numberOfColorPoints[goalColor];

  Moments2<2> moments;

  //iterate over all pixels with goal color and calculate moments of their distribution
  for(int p = 0; p < numberOfPoints; p++)
  {
    moments.add(getColoredGrid().getImagePoint(goalColor, p));
  }

  Vector2<double> major;
  Vector2<double> minor;

  centroid = moments.getCentroid();
  moments.getAxes(major, minor);

  //draw the major axis of the distribution
  DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:mark_goal",
    if(cameraID == CameraInfo::Top)
    {
      TOP_CIRCLE_PX(ColorClasses::green, (unsigned int)(centroid.x), (unsigned int)(centroid.y), 3);
      TOP_LINE_PX(ColorClasses::red, (unsigned int)(centroid.x), (unsigned int)(centroid.y), (unsigned int)(centroid.x+major.x), (unsigned int)(centroid.y+major.y));
    }
  );

  DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:mark_goal",
    if(cameraID == CameraInfo::Bottom)
    {
      CIRCLE_PX(ColorClasses::green, (unsigned int)(centroid.x), (unsigned int)(centroid.y), 3);
      LINE_PX(ColorClasses::red, (unsigned int)(centroid.x), (unsigned int)(centroid.y), (unsigned int)(centroid.x+major.x), (unsigned int)(centroid.y+major.y));
    }
  );

  return major.angle();
}//end getMajorAxis



GoalDetector::Blob GoalDetector::spiderExpandArea(
                          const Vector2<int>& startingPoint, 
                          ColorClasses::Color color, 
                          int maxPointsToSkip, 
                          int maxLengthOfBeams)
{
  const int NumberOfScanDirections = 8;
  Vector2<int> searchMask[] = 
  {
    Vector2<int>(-1,  0),
    Vector2<int>(-1, -1),
    Vector2<int>( 0, -1),
    Vector2<int>( 1, -1),
    Vector2<int>( 1,  0),
    Vector2<int>( 1,  1),
    Vector2<int>( 0,  1),
    Vector2<int>(-1,  1)
  };

  Blob blob;
  
  bool greenPointFound = false;

  for(int i = 0; i < NumberOfScanDirections; i++)             //scan in 4 diagonal directions
  {

    Vector2<int>& direction = searchMask[i];  // currently search direction 
    Vector2<int> currentPoint(startingPoint); // currently prcessed point
    Vector2<int> borderPoint(startingPoint);  //vector for found borderpoint;
    bool borderPointFound = false;            //no point found up to now
    int skipped = 0;                          //count for skipped (wrong colored) Points

    //expand in the selected direction
    for(int j = 0; j < maxLengthOfBeams; j++)
    {
      currentPoint += direction; //move current point first to avoid finding startingpoint as borderpoint (noise-reduction)

      // check the current point
      if( skipped > maxPointsToSkip ||
        // point outside the getImage()?
        (unsigned int)currentPoint.x > getImage().cameraInfo.resolutionWidth - 1 ||
        currentPoint.x < 0 ||
        (unsigned int)currentPoint.y > getImage().cameraInfo.resolutionHeight - 1 ||
        currentPoint.y < 0 )
      {
        break;
      }//end if


      ////////////////////////////////
      // process the current point  
      ////////////////////////////////
      Pixel pixel = getImage().get(currentPoint.x,currentPoint.y);
      ColorClasses::Color currentPixelColor = getColorTable64().getColorClass( 
        pixel); //classify color

      bool hasRightColor = (currentPixelColor == color);

      greenPointFound = greenPointFound || (currentPixelColor == ColorClasses::green && direction.x > 0);

      DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:spiderSearch",
        if(cameraID == CameraInfo::Top)
        {
          if(hasRightColor)
          {
            TOP_POINT_PX(ColorClasses::gray, currentPoint.x, currentPoint.y);
          }
          else
          {
            TOP_POINT_PX(ColorClasses::black, currentPoint.x, currentPoint.y);
          }
        }
      );
      DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:spiderSearch",
        if(cameraID == CameraInfo::Bottom)
        {
          if(hasRightColor)
          {
            POINT_PX(ColorClasses::gray, currentPoint.x, currentPoint.y);
          }
          else
          {
            POINT_PX(ColorClasses::black, currentPoint.x, currentPoint.y);
          }
        }
      );

      if(hasRightColor) 
      {
        borderPoint = currentPoint;
        skipped = 0;
        borderPointFound = true;
      }
      else
      {
        skipped++;
      }//end if

    }//end for

    blob.add(borderPoint);

    // additional treatement for border points ?!
    if(borderPointFound)
    {    
      DEBUG_REQUEST("ImageProcessor:GoalDetector:TopCam:spiderSearch",
        if(cameraID == CameraInfo::Top)
        {
          TOP_POINT_PX(color, borderPoint.x, borderPoint.y);
        }
      );

      DEBUG_REQUEST("ImageProcessor:GoalDetector:BottomCam:spiderSearch",
        if(cameraID == CameraInfo::Bottom)
        {
          POINT_PX(color, borderPoint.x, borderPoint.y);
        }
      );
    }
  }// for i < directions

  blob.greenPointFound = greenPointFound;
  return blob; 
}//end spiderExpandArea


double GoalDetector::calculateMeanAngle(const double& angle1, const double& angle2)
{
  //calculate unit vectors for both angles and add them
  double x=cos(2*angle1)+cos(2*angle2);
  double y=sin(2*angle1)+sin(2*angle2);
  
  //calculate sum vectors angle
  return(atan2(y, x)/2);
}//end calculateMeanAngle


double GoalDetector::getPointsAngle(const Vector2<int>& point)
{
  int pixelEnvironment[3][3]; 
  int x, y;

  //just get pixelenvironment of the pixel in question
  //for pixels outside the image the last pixel in there row/column is taken respectively
  for(int offsetX=-1; offsetX < 2; offsetX++)
  {
    for(int offsetY=-1; offsetY < 2; offsetY++)
    {
      x = point.x + offsetX;
      y = point.y + offsetY;
      Math::clamp(x, 1, (signed int)getImage().cameraInfo.resolutionWidth-1);
      Math::clamp(y, 1, (signed int)getImage().cameraInfo.resolutionHeight-1);
      Pixel pixel = getImage().get(x,y);
      pixelEnvironment[offsetX+1][offsetY+1] = pixel.y;
    }//end for offsetY
  }//end for

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  double gradientX =   
           pixelEnvironment[0][2]
        +2*pixelEnvironment[1][2]
        +  pixelEnvironment[2][2]
        -  pixelEnvironment[0][0]
        -2*pixelEnvironment[1][0]
        -  pixelEnvironment[2][0];

  double gradientY =   
           pixelEnvironment[0][0]
        +2*pixelEnvironment[0][1]
        +  pixelEnvironment[0][2]
        -  pixelEnvironment[2][0]
        -2*pixelEnvironment[2][1]
        -  pixelEnvironment[2][2];  
  
  //calculate the corresponding angle from the gradients
  return atan2(gradientY, gradientX);
}//end getPointsAngle



