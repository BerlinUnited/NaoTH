#include "BallDetectorEvaluator.h"

#include <strstream>

#include <Representations/Perception/BallCandidates.h>

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &dir)
  : file(dir), logFileScanner(dir)
{
  ballDetector = registerModule<BallDetector>("BallDetector", true);
}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

void BallDetectorEvaluator::execute()
{
  if(ballDetector != NULL)
  {
    // read in each frame
    for(LogFileScanner::FrameIterator it = logFileScanner.begin(); it != logFileScanner.end(); it++)
    {

      LogFileScanner::Frame frame;
      logFileScanner.readFrame(*it, frame);

      // deserialize the ball candidates (bottom an top camera)
      BallCandidates ballCandidatesBottom;
      BallCandidates ballCandidatesTop;

      auto frameBallCandidate = frame.find("BallCandidate");
      auto frameBallCandidateTop = frame.find("BallCandidateTop");
      if(frameBallCandidate!= frame.end())
      {
        std::istrstream stream(frameBallCandidate->second.data.data(), frameBallCandidate->second.data.size());
        naoth::Serializer<BallCandidates>::deserialize(stream, ballCandidatesBottom);
      }
      if(frameBallCandidateTop != frame.end())
      {
        std::istrstream stream(frameBallCandidateTop->second.data.data(), frameBallCandidateTop->second.data.size());
        naoth::Serializer<BallCandidatesTop>::deserialize(stream, ballCandidatesTop);
      }

//      std::cout << "Test frame " << *it << std::endl;

      ballDetector->execute();
    }
  }
}
