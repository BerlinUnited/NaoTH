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
    
  // angleToSeenGoal
  g.set_angletoseengoal(representation.angleToSeenGoal);
  
  // goalCentroid
  g.mutable_goalcentroid()->set_x(representation.goalCentroid.x);
  g.mutable_goalcentroid()->set_y(representation.goalCentroid.y);
  g.mutable_goalcentroid()->set_z(representation.goalCentroid.z);

  // numberOfSeenPosts
  g.set_numberofseenposts(representation.numberOfSeenPosts);

  // post
  for(unsigned int i=0; i < representation.numberOfSeenPosts && i < representation.MAXNUMBEROFPOSTS; i++)
  {
    naothmessages::GoalPercept::GoalPost* p = g.add_post();
    const GoalPercept::GoalPost& post = representation.post[i];

    // basePoint
    p->mutable_basepoint()->set_x(post.basePoint.x);
    p->mutable_basepoint()->set_y(post.basePoint.y);

    // topPoint
    // TODO

    // color
    p->set_color((naothmessages::Color) post.color);

    // type
    p->set_type((naothmessages::GoalPercept::GoalPost::PostType) post.type);

    // positionReliable
    p->set_positionreliable(post.positionReliable);

    // seenHeight
    p->set_seenheight(post.seenHeight);

    // position
    p->mutable_position()->set_x(post.position.x);
    p->mutable_position()->set_y(post.position.y);
  }//end for

  google::protobuf::io::OstreamOutputStream buf(&stream);
  g.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<GoalPercept>::deserialize(std::istream& stream, GoalPercept& representation)
{
  // clear the percept befor reading from stream
  representation.reset();

  // deserialize
  naothmessages::GoalPercept g;
  google::protobuf::io::IstreamInputStream buf(&stream);
  g.ParseFromZeroCopyStream(&buf);

  // angleToSeenGoal
  if(g.has_angletoseengoal())
  {
    representation.angleToSeenGoal = g.angletoseengoal();
  }

  // goalCentroid
  if(g.has_goalcentroid())
  {
    representation.goalCentroid.x = g.goalcentroid().x();
    representation.goalCentroid.y = g.goalcentroid().y();
    representation.goalCentroid.z = g.goalcentroid().z();
  }

  // numberOfSeenPosts
  if(g.has_numberofseenposts())
  {
    representation.numberOfSeenPosts = g.numberofseenposts();
  }

  // post
  for(unsigned int i=0; i < (unsigned int)g.post_size() && i < representation.numberOfSeenPosts && i < representation.MAXNUMBEROFPOSTS; i++)
  {
    const naothmessages::GoalPercept::GoalPost& p = g.post(i);
    GoalPercept::GoalPost& post = representation.post[i];

    // basePoint
    post.basePoint.x = p.basepoint().x();
    post.basePoint.y = p.basepoint().y();

    // topPoint
    // TODO

    // color
    post.color = (ColorClasses::Color) p.color();

    // type
    post.type = (GoalPercept::GoalPost::PostType) p.type();

    // positionReliable
    post.positionReliable = p.positionreliable();

    // seenHeight
    post.seenHeight = p.seenheight();

    // position
    post.position.x = p.position().x();
    post.position.y = p.position().y();
  }//end for
}//end deserialize