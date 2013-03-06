/**
* @file AGLSampleSet.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class AGLSampleSet
*/

#include "AGLSampleSet.h"
#include "Tools/Debug/DebugDrawings.h"


void AGLSampleSet::sort()
{
  quicksort(0, samples.size()-1);
}//end sort

void AGLSampleSet::quicksort(int low, int high)
{
  int i = low;
  int j = high;

  AGLSample help;

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
void AGLSampleSet::normalize()
{
  double sum = 0;
  for(unsigned int i = 0; i < samples.size(); i++)
  {
    sum += samples[i].likelihood;
  }

  if(sum == 0) return;

  for(unsigned int i = 0; i < samples.size(); i++)
  {
    samples[i].likelihood /= sum;
  }
}//end normalize


void AGLSampleSet::setValid()
{
  isValid = true;
}//end setValid

void AGLSampleSet::setUnValid()
{
  isValid = false;
}//end setUnvalid

bool AGLSampleSet::getIsValid() const
{
  return isValid;
}//end getIsValid;

void AGLSampleSet::resetLikelihood()
{
  double likelihood = 1.0/static_cast<double>(samples.size());
  for(unsigned int i = 0; i < samples.size(); i++)
  {
    samples[i].likelihood = likelihood;
  }
}//end resetLikelihood


// TODO: make it more efficient (!!!)
AGLSample AGLSampleSet::meanOfLargestCluster(Moments2<2>& moments)
{
  // TODO: make it better
  std::vector<int> cluster(samples.size());
  std::vector<Vector2<double> > averageTranslation(samples.size());
  //std::vector<Vector2<double> > averageRotation(samples.size());
  unsigned int maxIndex = 0;
  double maxNumber = 0;

  for(unsigned int i = 0; i < samples.size(); i++)
  {
    if(samples[i].cluster >= 0 && samples[i].cluster < (int)samples.size())
    {
      int idx = samples[i].cluster;
      cluster[idx]++;
      averageTranslation[idx] += samples[i].translation;
      //averageRotation[idx].x += cos(samples[i].rotation);
      //averageRotation[idx].y += sin(samples[i].rotation);

      if(maxNumber < cluster[idx])
      {
        maxIndex = idx;
        maxNumber = cluster[idx];
      }
    }//end if
  }//end for

  AGLSample result;
  result.translation = averageTranslation[maxIndex]/maxNumber;
  //result.rotation = averageRotation[maxIndex].angle();
  result.cluster = maxIndex;


  // calculate the covariance of the largest cluster
  for(unsigned int i = 0; i < samples.size(); i++)
  {
    const AGLSample& sample = samples[i];
    if(sample.cluster == (int)maxIndex)
    {
      moments.add(sample.translation);
    }
  }//end for

  return result;
}//end meanOfLargestCluster


void AGLSampleSet::drawCluster(unsigned int clusterId)
{
  FIELD_DRAWING_CONTEXT;
  for (unsigned int i = 0; i < samples.size(); i++)
  {
    if (samples[i].cluster == (int)clusterId)
      PEN("FFFFFF", 20);
    else
      PEN("000000", 20);

    /*ARROW(samples[i].translation.x, samples[i].translation.y,
          samples[i].translation.x + 100*cos(samples[i].rotation),
          samples[i].translation.y + 100*sin(samples[i].rotation));*/

  }//end for
}//end drawSamples

