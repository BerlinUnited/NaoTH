/**
* @file AttentionEngine.cpp
*/

#include "AttentionEngine.h"
#include <iostream>

AttentionEngine::AttentionEngine()
{
  go_left = true;
  getAttentionModel().mostInterestingPoint = Vector2d(1000, 100);
}

AttentionEngine::~AttentionEngine()
{
}

void AttentionEngine::execute()
{
  size_t frame_number = getFrameInfo().getFrameNumber();
  //getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime()) < 3000
  
  if (frame_number % 100 == 0){
    std::cout << "switch" << std::endl;
    go_left = !go_left;
  }
  
  if(go_left){
    getAttentionModel().mostInterestingPoint = Vector2d(160, 200);
  }
  else{
    getAttentionModel().mostInterestingPoint = Vector2d(160.0, -200);
  }

  std::cout << getAttentionModel().mostInterestingPoint << std::endl;
  
}
