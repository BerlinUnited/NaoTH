/**
* @file SampleSet.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class SampleSet
*/

#include "SampleSet.h"
#include "Tools/Debug/DebugDrawings.h"


void SampleSet::sort() 
{
  quicksort(0, numberOfParticles-1);
}//end sort

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
    while(samples[i].likelihood < z) i++;

    /* find element below ... */
    while(samples[j].likelihood > z) j--;

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


// TODO: optimize?!
void SampleSet::normalize()
{
  double sum = 0;
  for(int i = 0; i < numberOfParticles; i++)
  {
    sum += samples[i].likelihood;
  }

  if(sum == 0) return;

  for(int i = 0; i < numberOfParticles; i++)
  {
    samples[i].likelihood /= sum;
  }
}//end normalize


void SampleSet::resetLikelihood()
{
  double likelihood = 1.0/static_cast<double>(numberOfParticles);
  for(int i = 0; i < numberOfParticles; i++)
  {
    samples[i].likelihood = likelihood;
  }
}//end resetLikelihood


// TODO: make it more efficient (!!!)
Sample SampleSet::meanOfLargestCluster(Moments2<2>& moments)
{
  // TODO: make it better
  std::vector<int> cluster(numberOfParticles);
  std::vector<Vector2<double> > averageTranslation(numberOfParticles);
  std::vector<Vector2<double> > averageRotation(numberOfParticles);
  int maxIndex = 0;
  double maxNumber = 0;

  for(int i = 0; i < numberOfParticles; i++)
  {
    if(samples[i].cluster >= 0 && samples[i].cluster < numberOfParticles)
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
  for(int i = 0; i < numberOfParticles; i++)
  {
    const Sample& sample = samples[i];
    if(sample.cluster == maxIndex)
    {
      moments.add(sample.translation);
    }
  }//end for

  return result;
}//end meanOfLargestCluster

void SampleSet::drawCluster(int clusterId)
{
  FIELD_DRAWING_CONTEXT;
  for (int i = 0; i < numberOfParticles; i++)
  {
    if (samples[i].cluster == clusterId) 
      PEN("FFFFFF", 20);
    else 
      PEN("000000", 20);

    ARROW(samples[i].translation.x, samples[i].translation.y, 
          samples[i].translation.x + 100*cos(samples[i].rotation), 
          samples[i].translation.y + 100*sin(samples[i].rotation));
  }//end for
}//end drawSamples

