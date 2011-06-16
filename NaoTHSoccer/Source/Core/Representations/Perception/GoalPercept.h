/** 
* @file GoalPercept.h
*
* Declaration of class GoalPercept
*/

#ifndef __GoalPercept_h_
#define __GoalPercept_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/ColorClasses.h"
#include "Tools/DataStructures/Printable.h"

class GoalPercept : public Printable
{ 
public:
  GoalPercept()
    : 
    numberOfSeenPosts(0)
  {}

  class GoalPost
  {
  public:
    GoalPost(): color(ColorClasses::none),type(unknownPost), positionReliable(false), seenHeight(0){};
    ~GoalPost(){};

    /* possible types of goalposts */
    enum PostType 
    {
      rightPost, 
      leftPost, 
      unknownPost
    }; 

    /* the posts base point in the image */
    Vector2<int> basePoint;

    /* the posts base point in the image */
    Vector2<int> topPoint;

    /* color of the post */
    ColorClasses::Color color;

    /* posts type */
    PostType type;

    /* indicates whether the base point was seen inside the field */
    bool positionReliable;

    /* the seen height of the post */
    double seenHeight;

    /* the posts base point on the ground */
    Vector2<double> position;


    static const char* getPostTypeName(PostType type)
    {
      switch(type) 
      {
        case rightPost:   return "right post";
        case leftPost:    return "left post"; 
        case unknownPost: return "unknown post"; 
        default:          return "unknown post type!"; 
      };
    }//end getPostTypeName

    bool operator() (const GoalPost& postOne, const GoalPost& postTwo) 
    { 
      return (postOne.seenHeight > postTwo.seenHeight);
    }

  };//end class GoalPost



  /* maximum number of posts */
  const static unsigned int MAXNUMBEROFPOSTS = 6;

  /* angle to the centroid of the goal-colored pixel in the image */
  double angleToSeenGoal;

  /* estimated position of the goal-centroid relative to the robot */
  Vector3<double> goalCentroid;


  void add(const Vector2<int>& basePoint, const Vector2<double>& position, GoalPost::PostType type, ColorClasses::Color color, bool reliable)
  {
    if(numberOfSeenPosts < MAXNUMBEROFPOSTS)
    {
      post[numberOfSeenPosts].basePoint         = basePoint;
      post[numberOfSeenPosts].position          = position;
      post[numberOfSeenPosts].type              = type;
      post[numberOfSeenPosts].color             = color;
      post[numberOfSeenPosts].positionReliable  = reliable;
      numberOfSeenPosts++;
    }//end if
  }//end add

  void add(const GoalPost& goalPost)
  {
    if(numberOfSeenPosts < MAXNUMBEROFPOSTS)
    {
      post[numberOfSeenPosts] = goalPost;
      numberOfSeenPosts++;
    }//end if
  }//end add


  /* reset percept */
  void reset()
  {
    numberOfSeenPosts = 0;
    angleToSeenGoal = 0; 
  }//end reset


  virtual void print(ostream& stream) const
  {
    stream << "angleToSeenGoal=" << angleToSeenGoal << endl;
    stream << "goalCentroid=" << goalCentroid << endl;
    for(unsigned int n=0; n<numberOfSeenPosts; n++)
    {
      stream << "=====Post " << n << "=====" << endl;
      stream << "post type= " << GoalPost::getPostTypeName(post[n].type) << endl;
      stream << "basePoint=(" << post[n].basePoint.x << ";" << post[n].basePoint.y << ")" << endl;
      stream << "position=(" << post[n].position.x << ";" << post[n].position.y << ")" << endl;
      stream << "color=" << ColorClasses::getColorName(post[n].color) << endl;
      stream << "reliable=" << (post[n].positionReliable? "true" : "false") << endl;
    }//end for
  }//end print
/*
  virtual void toDataStream(ostream& stream) const
  {
    naothmessages::GoalPercept g;
    
    g.set_angletoseengoal(angleToSeenGoal);
    
    g.mutable_goalcentroid()->set_x(goalCentroid.x);
    g.mutable_goalcentroid()->set_y(goalCentroid.y);
    g.mutable_goalcentroid()->set_z(goalCentroid.z);

    g.set_numberofseenposts(numberOfSeenPosts);

    for(unsigned int i=0; i < numberOfSeenPosts && i < MAXNUMBEROFPOSTS; i++)
    {
      naothmessages::GoalPost* p = g.add_post();
      p->mutable_basepoint()->set_x(post[i].basePoint.x);
      p->mutable_basepoint()->set_y(post[i].basePoint.y);

      p->mutable_position()->set_x(post[i].position.x);
      p->mutable_position()->set_y(post[i].position.y);

      p->set_color((naothmessages::Color) post[i].color);

      p->set_type((naothmessages::GoalPost_PostType) post[i].type);

      p->set_positionreliable(post[i].positionReliable);

      p->set_seenheight(post[i].seenHeight);
    }

    google::protobuf::io::OstreamOutputStreamLite buf(&stream);
    g.SerializeToZeroCopyStream(&buf);
  }
  
  virtual void fromDataStream(istream& stream)
  {
    // clear the percept befor reading from stream
    this->reset();

    naothmessages::GoalPercept g;
    google::protobuf::io::IstreamInputStreamLite buf(&stream);
    g.ParseFromZeroCopyStream(&buf);

    if(g.has_angletoseengoal())
    {
      angleToSeenGoal = g.angletoseengoal();
    }
    if(g.has_goalcentroid())
    {
      goalCentroid.x = g.goalcentroid().x();
      goalCentroid.y = g.goalcentroid().y();
      goalCentroid.z = g.goalcentroid().z();
    }
    if(g.has_numberofseenposts())
    {
      numberOfSeenPosts = g.numberofseenposts();
    }
    for(unsigned int i=0; i < (unsigned int)g.post_size() && i < numberOfSeenPosts && i < MAXNUMBEROFPOSTS; i++)
    {
      post[i].basePoint.x = g.post(i).basepoint().x();
      post[i].basePoint.y = g.post(i).basepoint().y();

      post[i].position.x = g.post(i).position().x();
      post[i].position.y = g.post(i).position().y();

      post[i].color = (ColorClasses::Color) g.post(i).color();

      post[i].type = (GoalPost::PostType) g.post(i).type();

      post[i].positionReliable = g.post(i).positionreliable();

      post[i].seenHeight = g.post(i).seenheight();
    }
  }
*/
  //////////////////////////////////////
  // getters
  unsigned int getNumberOfSeenPosts() const { return numberOfSeenPosts; }
  const GoalPost& getPost(unsigned int i) const { ASSERT(i < numberOfSeenPosts); return post[i]; }

private:
  /* number of seen posts */
  unsigned int numberOfSeenPosts;

  /* array of recognized posts in arbitrary order */
  GoalPost post[MAXNUMBEROFPOSTS];

};//end GoalPercept

#endif //__GoalPercept_h_

