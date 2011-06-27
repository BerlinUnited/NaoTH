/**
 * @file GoalPercept.cpp
 * 
 * Definition of class GoalPercept
 */ 

#include "GoalPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<GoalPercept>::serialize(const GoalPercept& representation, std::ostream& stream)
{
  naothmessages::GoalPercept g;
    
  g.set_angletoseengoal(representation.angleToSeenGoal);
  
  g.mutable_goalcentroid()->set_x(representation.goalCentroid.x);
  g.mutable_goalcentroid()->set_y(representation.goalCentroid.y);
  g.mutable_goalcentroid()->set_z(representation.goalCentroid.z);

  g.set_numberofseenposts(representation.numberOfSeenPosts);

  for(unsigned int i=0; i < representation.numberOfSeenPosts && i < representation.MAXNUMBEROFPOSTS; i++)
  {
    naothmessages::GoalPost* p = g.add_post();
    p->mutable_basepoint()->set_x(representation.post[i].basePoint.x);
    p->mutable_basepoint()->set_y(representation.post[i].basePoint.y);

    p->mutable_position()->set_x(representation.post[i].position.x);
    p->mutable_position()->set_y(representation.post[i].position.y);

    p->set_color((naothmessages::Color) representation.post[i].color);

    p->set_type((naothmessages::GoalPost_PostType) representation.post[i].type);

    p->set_positionreliable(representation.post[i].positionReliable);

    p->set_seenheight(representation.post[i].seenHeight);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  g.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<GoalPercept>::deserialize(std::istream& stream, GoalPercept& representation)
{
  // clear the percept befor reading from stream
  representation.reset();

  naothmessages::GoalPercept g;
  google::protobuf::io::IstreamInputStream buf(&stream);
  g.ParseFromZeroCopyStream(&buf);

  if(g.has_angletoseengoal())
  {
    representation.angleToSeenGoal = g.angletoseengoal();
  }
  if(g.has_goalcentroid())
  {
    representation.goalCentroid.x = g.goalcentroid().x();
    representation.goalCentroid.y = g.goalcentroid().y();
    representation.goalCentroid.z = g.goalcentroid().z();
  }
  if(g.has_numberofseenposts())
  {
    representation.numberOfSeenPosts = g.numberofseenposts();
  }
  for(unsigned int i=0; i < (unsigned int)g.post_size() && i < representation.numberOfSeenPosts && i < representation.MAXNUMBEROFPOSTS; i++)
  {
    representation.post[i].basePoint.x = g.post(i).basepoint().x();
    representation.post[i].basePoint.y = g.post(i).basepoint().y();

    representation.post[i].position.x = g.post(i).position().x();
    representation.post[i].position.y = g.post(i).position().y();

    representation.post[i].color = (ColorClasses::Color) g.post(i).color();

    representation.post[i].type = (GoalPercept::GoalPost::PostType) g.post(i).type();

    representation.post[i].positionReliable = g.post(i).positionreliable();

    representation.post[i].seenHeight = g.post(i).seenheight();
  }
}//end deserialize