/**
* @file SampleSet.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class SampleSet
*/

#include "SampleSet.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Math/Common.h"

void SampleSet::sort(bool descending) 
{
  if(samples.size() > 1) {
    quicksort(descending?1:-1, 0, ((int)samples.size())-1);
  }
}

void SampleSet::quicksort(int d, int low, int high) 
{
  int i = low;
  int j = high;

  Sample help;

  /* compare value */
  double z = samples[(low + high) / 2].likelihood*d;

  /* partition */
  do {
    /* find member above ... */
    while(samples[i].likelihood*d > z) i++;

    /* find element below ... */
    while(samples[j].likelihood*d < z) j--;

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
  if(low < j) {
   quicksort(d, low, j);
  }

  if(i < high) {
    quicksort(d, i, high);
  }
}//end quicksort


void SampleSet::normalize(double offset)
{
  double sum = 0.0;
  for(size_t i = 0; i < samples.size(); i++) {
    samples[i].likelihood = samples[i].likelihood;
    sum += samples[i].likelihood;
  }

  if(sum == 0) { 
    return; 
  }

  double offset_sum = 1.0+offset*(double)samples.size();

  for(size_t i = 0; i < samples.size(); i++) {
    samples[i].likelihood = ((samples[i].likelihood/sum) + offset)/offset_sum;
  }
}//end normalize


void SampleSet::resetLikelihood()
{
  double likelihood = 1.0/static_cast<double>(samples.size());
  setLikelihood(likelihood);
}

void SampleSet::setLikelihood(double v)
{
  for(size_t i = 0; i < samples.size(); i++) {
    samples[i].likelihood = v;
  }
}

Sample SampleSet::meanOfLargestCluster(Moments2<2>& moments) const
{
  ASSERT(samples.size() > 0);

  // TODO: make it better
  std::vector<int> cluster(samples.size());
  int maxIndex = 0;
  double maxNumber = 0;

  for(size_t i = 0; i < samples.size(); i++)
  {
    if(samples[i].cluster >= 0 && samples[i].cluster < (int)samples.size())
    {
      int idx = samples[i].cluster;
      cluster[idx]++;

      if(maxNumber < cluster[idx]) {
        maxIndex = idx;
        maxNumber = cluster[idx];
      }
    }//end if
  }//end for

  return meanOfCluster(moments, maxIndex);
}//end meanOfLargestCluster


Sample SampleSet::meanOfCluster(Moments2<2>& moments, int idx) const 
{
  ASSERT(samples.size() > 0);

  Vector2d averageTranslation;
  Vector2d averageRotation;
  double numberOfSamples = 0;

  // calculate the covariance of the largest cluster
  for(size_t i = 0; i < samples.size(); i++)
  {
    const Sample& sample = samples[i];
    if(sample.cluster == idx)
    {
      averageTranslation += sample.translation;
      averageRotation.x += cos(sample.rotation);
      averageRotation.y += sin(sample.rotation);
      moments.add(sample.translation);
      numberOfSamples++;
    }
  }

  Sample result;
  result.cluster = idx;

  if(numberOfSamples > 0) {
    result.translation = averageTranslation/numberOfSamples;
    result.rotation = (averageRotation/numberOfSamples).angle();
  }

  return result;
}//end meanOfCluster

const Sample& SampleSet::getMostLikelySample() const
{
  ASSERT(samples.size() > 0);

  double maxLikelihood = samples[0].likelihood;
  int maxIdx = 0;

  for(size_t i = 1; i < samples.size(); i++)
  {
    if(maxLikelihood < samples[i].likelihood) {
      maxLikelihood = samples[i].likelihood;
      maxIdx = (int)i;
    }
  }

  return samples[maxIdx];
}

void SampleSet::drawCluster(DrawingCanvas2D& canvas, unsigned int clusterId) const
{
  ASSERT(samples.size() > 0);

  for (size_t i = 0; i < samples.size(); i++)
  {
    if (samples[i].cluster == (int)clusterId) {
      canvas.pen("FFFFFF", 20);
    } else { 
      canvas.pen("000000", 20);
    }

    canvas.drawArrow(samples[i].translation.x, samples[i].translation.y, 
          samples[i].translation.x + 100*cos(samples[i].rotation), 
          samples[i].translation.y + 100*sin(samples[i].rotation));
  }
}

void SampleSet::drawImportance(DrawingCanvas2D& canvas, bool /*arrows*/) const
{
  ASSERT(samples.size() > 0);

  // normalize the colors (black: less importent, red more importent)
  double minValue = samples[0].likelihood;
  double maxValue = samples[0].likelihood;
  for (size_t i = 1; i < samples.size(); i++)
  {
    maxValue = std::max(samples[i].likelihood, maxValue);
    minValue = std::min(samples[i].likelihood, minValue);
  }
  double colorDiff = log(maxValue) - log(minValue);

  for (size_t i = 0; i < samples.size(); i++)
  {
    Color color;
    if(colorDiff > 0) {
      color[0] = Math::clamp((log(samples[i].likelihood) - log(minValue))/colorDiff,0.0,1.0);
    }
    
    canvas.pen(color, 20);

    canvas.drawArrow(samples[i].translation.x, samples[i].translation.y, 
          samples[i].translation.x + 100*cos(samples[i].rotation), 
          samples[i].translation.y + 100*sin(samples[i].rotation));
  }
}

