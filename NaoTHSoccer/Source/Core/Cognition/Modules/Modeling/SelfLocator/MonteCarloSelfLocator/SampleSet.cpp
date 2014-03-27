/**
* @file SampleSet.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class SampleSet
*/

#include "SampleSet.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Math/Common.h"

void SampleSet::sort() 
{
  quicksort(0, numberOfParticles-1);
}

void SampleSet::quicksort(int low, int high) 
{
  int i = low;
  int j = high;

  Sample help;

  /* compare value */
  double z = samples[(low + high) / 2].likelihood;

  /* partition */
  do {
    /* find member above ... */
    while(samples[i].likelihood > z) i++;

    /* find element below ... */
    while(samples[j].likelihood < z) j--;

    if(i <= j) 
    {
      /* swap two elements */
      help = samples[i];
      samples[i] = samples[j]; 
      samples[j] = help;

      i++; 
      j--;
    }//end if
  } while(i <= j);

  /* recurse */
  if(low < j) 
   quicksort(low, j);

  if(i < high) 
    quicksort(i, high); 
}//end quicksort


void SampleSet::normalize(double offset)
{
  double sum = 0.0;
  for(unsigned int i = 0; i < numberOfParticles; i++)
  {
    samples[i].likelihood = samples[i].likelihood;
    sum += samples[i].likelihood;
  }

  if(sum == 0) return;

  double offset_sum = 1.0+offset*numberOfParticles;

  for(unsigned int i = 0; i < numberOfParticles; i++)
  {
    samples[i].likelihood = ((samples[i].likelihood/sum) + offset)/offset_sum;
  }
}//end normalize


void SampleSet::resetLikelihood()
{
  double likelihood = 1.0/static_cast<double>(numberOfParticles);
  for(unsigned int i = 0; i < numberOfParticles; i++)
  {
    samples[i].likelihood = likelihood;
  }
}//end resetLikelihood


// TODO: make it more efficient (!!!)
Sample SampleSet::meanOfLargestCluster(Moments2<2>& moments) const
{
  ASSERT(samples.size() > 0);

  // TODO: make it better
  std::vector<int> cluster(numberOfParticles);
  std::vector<Vector2<double> > averageTranslation(numberOfParticles);
  std::vector<Vector2<double> > averageRotation(numberOfParticles);
  unsigned int maxIndex = 0;
  double maxNumber = 0;

  for(unsigned int i = 0; i < numberOfParticles; i++)
  {
    if(samples[i].cluster >= 0 && samples[i].cluster < (int)numberOfParticles)
    {
      int idx = samples[i].cluster;
      cluster[idx]++;
      averageTranslation[idx] += samples[i].translation;
      averageRotation[idx].x += cos(samples[i].rotation);
      averageRotation[idx].y += sin(samples[i].rotation);

      if(maxNumber < cluster[idx])
      {
        maxIndex = idx;
        maxNumber = cluster[idx];
      }
    }//end if
  }//end for

  Sample result;
  result.translation = averageTranslation[maxIndex]/maxNumber;
  result.rotation = averageRotation[maxIndex].angle();
  result.cluster = maxIndex;


  // calculate the covariance of the largest cluster
  for(unsigned int i = 0; i < numberOfParticles; i++)
  {
    const Sample& sample = samples[i];
    if(sample.cluster == (int)maxIndex)
    {
      moments.add(sample.translation);
    }
  }//end for

  return result;
}//end meanOfLargestCluster

const Sample& SampleSet::getMostLikelySample() const
{
  ASSERT(samples.size() > 0);

  double maxLikelihood = samples[0].likelihood;
  int maxIdx = 0;

  for(unsigned int i = 1; i < numberOfParticles; i++)
  {
    if(maxLikelihood < samples[i].likelihood) {
      maxLikelihood = samples[i].likelihood;
      maxIdx = i;
    }
  }

  return samples[maxIdx];
}

void SampleSet::drawCluster(unsigned int clusterId) const
{
  ASSERT(samples.size() > 0);
  FIELD_DRAWING_CONTEXT;
  for (size_t i = 0; i < samples.size(); i++)
  {
    if (samples[i].cluster == (int)clusterId) {
      PEN("FFFFFF", 20);
    } else { 
      PEN("000000", 20);
    }

    ARROW(samples[i].translation.x, samples[i].translation.y, 
          samples[i].translation.x + 100*cos(samples[i].rotation), 
          samples[i].translation.y + 100*sin(samples[i].rotation));
  }
}

void SampleSet::drawImportance(bool arrows) const
{
  ASSERT(samples.size() > 0);
  FIELD_DRAWING_CONTEXT;

  // normalize the colors (black: less importent, red more importent)
  double minValue = samples[0].likelihood;
  double maxValue = samples[0].likelihood;
  for (unsigned int i = 1; i < samples.size(); i++)
  {
    maxValue = std::max(samples[i].likelihood, maxValue);
    minValue = std::min(samples[i].likelihood, minValue);
  }
  double colorDiff = log(maxValue) - log(minValue);

  for (size_t i = 0; i < samples.size(); i++)
  {
    DebugDrawings::Color color;
    if(colorDiff > 0) {
      color[0] = Math::clamp((log(samples[i].likelihood) - log(minValue))/colorDiff,0.0,1.0);
    }
    
    PEN(color, 20);

    ARROW(samples[i].translation.x, samples[i].translation.y, 
          samples[i].translation.x + 100*cos(samples[i].rotation), 
          samples[i].translation.y + 100*sin(samples[i].rotation));
  }//end for
}

