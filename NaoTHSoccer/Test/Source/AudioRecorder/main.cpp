
#include <iostream>
#include <DCM/NaoRobot/AudioRecorder.h>
#include <Tools/ThreadUtil.h>

using namespace naoth;

int main()
{
  AudioControl controlData;
  AudioData audioData;
  
  AudioRecorder rec;
  
  // start recording
  controlData.capture = true;
  controlData.numChannels = 4;
  controlData.sampleRate = 8000;
  rec.set(controlData);
  
  unsigned int timestamp = 0;
  while(true) 
  {
    rec.get(audioData);
    
    if(audioData.timestamp > timestamp) {
      //std::cout.write((char*)audioData.samples.data(), audioData.samples.size()*sizeof(short));
      
      int max = 0;
      for(size_t i = 0; i < audioData.samples.size(); i++) {
        max = max < audioData.samples[i] ? audioData.samples[i] : max;
      }
      std::cout << max << std::endl;
      timestamp = audioData.timestamp;
    }
    
    ThreadUtil::sleep(33);
  }
  
  //std::cout << "test" << std::endl;
}
