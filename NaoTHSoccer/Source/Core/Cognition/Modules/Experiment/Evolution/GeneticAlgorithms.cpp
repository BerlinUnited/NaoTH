/**
 * @file GeneticAlgorithms.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include <cmath>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "GeneticAlgorithms.h"
#include "Tools/Math/Common.h"
#include "Tools/Debug/NaoTHAssert.h"

using namespace std;

double GeneticAlgorithms::transmit(double father, const double mother)
{
	return (Math::random(-1.0,1.0)>0)?father:mother;
}

double GeneticAlgorithms::crossover(double father, double mother)
{
  double k = Math::random(0.0, 1.0);
  return k * father + (1 - k) * mother;
}

double GeneticAlgorithms::mutation(double father, double mother)
{
  return (crossover(father, mother) + Math::random(-1.0f, 1.0f))*Math::random(0.0f, 2.0f);
}

double GeneticAlgorithms::coition(double father, double mother, double transmitRate, double crossoverRate, double mutationRate)
{
  double wayRate = Math::random(0.0, transmitRate + crossoverRate + mutationRate);
  if (wayRate < transmitRate)
  {
    return transmit(father, mother);
  } else if (wayRate < transmitRate + crossoverRate)
  {
    return crossover(father, mother);
  } else
  {
    return mutation(father, mother);
  }
}

GeneticAlgorithms::Individual::Individual(const Individual& father, const Individual& mother, double transmitRate, double crossoverRate, double mutationRate)
:fitness(-1)
{
  ASSERT(father.gene.size() == mother.gene.size() );
  
  for (map<string, double>::const_iterator g=father.gene.begin(); g!=father.gene.end(); ++g)
  {
    gene[g->first] = coition(g->second, mother.gene.find(g->first)->second, transmitRate, crossoverRate, mutationRate);
  }
}

GeneticAlgorithms::GeneticAlgorithms()
:parentsNum(10),
  population(100),
  surviveNum(10),
  maxGeneration(50),
  transmitRate(1),
  crossoverRate(1),
  mutationRate(1)
{
  
}

vector<GeneticAlgorithms::Individual> GeneticAlgorithms::newGeneration(const vector<GeneticAlgorithms::Individual>& old)
{
  vector<Individual> children;
  // survive indivisuals
  for( size_t i=0; i<surviveNum&&i<old.size(); i++)
  {
    children.push_back(old[old.size()-1-i]);
  }

  // generate children
  while (children.size() < population)
  {
    // selected parents
    const Individual& father = selectParent(old);
    const Individual& mother = selectParent(old);
    children.push_back( Individual(father, mother, transmitRate, crossoverRate, mutationRate) );
  }
  
  return children;
}

const GeneticAlgorithms::Individual& GeneticAlgorithms::selectParent(const std::vector<Individual>& old)
{
  //@note: all the parents have the same possibility to be seleceted
  double maxId = min(old.size(), parentsNum);
  double r = Math::random(0.0, maxId);
  return old[old.size() -1 - (size_t)floor(r)];
}

void GeneticAlgorithms::init(const vector<Individual>& initial)
{
  generations.clear();
  generations.push_back( initial );
}

bool GeneticAlgorithms::isFinished() const
{
  return generations.size() > maxGeneration;
}

GeneticAlgorithms::Individual& GeneticAlgorithms::getIndividual()
{
  vector<Individual>& lastGeneration = generations.back();
  vector<Individual>::iterator iter = lastGeneration.begin();
  while( iter != lastGeneration.end() && iter->fitness >= 0 )
  {
    iter++;
  }
  if ( iter != lastGeneration.end() )
  {
    return *iter;
  }
  else
  {
    sort(lastGeneration.begin(), lastGeneration.end());
    stringstream filename;
    filename << "ga/" << generations.size() <<".txt";
    saveGeneration( lastGeneration, filename.str() );
    generations.push_back( newGeneration(lastGeneration) );
    return generations.back().front();
  }
}

void GeneticAlgorithms::saveGeneration(const std::vector<Individual>& gen, const std::string& file)
{
  ofstream os(file.c_str());
  if ( os )
  {
    for ( std::vector<Individual>::const_iterator iter=gen.begin(); iter!=gen.end(); ++iter )
    {
      os << *iter << endl;
    }
  }
}

std::ostream& operator <<(std::ostream& ost, const GeneticAlgorithms::Individual& v)
{
  ost<<v.fitness<<"------------------------------------\n";
  for ( map<string, double>::const_iterator iter=v.gene.begin(); iter!=v.gene.end(); ++iter)
  {
    ost<<iter->first<<" = "<<iter->second<<";\n";
  }
  ost<<endl;
  return ost;
}
