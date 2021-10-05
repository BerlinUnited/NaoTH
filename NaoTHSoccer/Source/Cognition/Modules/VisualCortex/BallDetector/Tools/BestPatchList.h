/**
* @file BestPatchList.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _BestPatchList_H_
#define _BestPatchList_H_

#include <Tools/Math/Vector2.h>
#include <list>
#include <vector>

class BestPatchList
{

public:
  class Patch 
  {
  public:
    Patch() : value(-1) {}
    Patch(const Vector2i& min, const Vector2i& max, double value) 
      : min(min), max(max), value(value)
    {}
    Patch(int minX, int minY, int maxX, int maxY, double value) 
      : min(minX, minY), max(maxX, maxY), value(value)
    {}
    Vector2i min;
    Vector2i max;
    double value;
  };

  // make it cons iterable
  typedef std::list<Patch> PatchList;
  typedef std::list<Patch>::const_iterator iterator;
  typedef std::list<Patch>::const_reverse_iterator reverse_iterator;
  const iterator begin() const { return patches.cbegin(); }
  const iterator end() const { return patches.cend(); }
  const reverse_iterator rbegin() const { return patches.crbegin(); }
  const reverse_iterator rend() const { return patches.crend(); }


public:

  void add(int minX, int minY, int maxX, int maxY, double value) {
    add(Patch(minX, minY, maxX, maxY, value));
  }


  void add(const Patch& patch)
  {
    // add the first patch
    if(patches.empty()) {
      patches.push_back(patch);
      return;
    }

    // check if there are overlaping patches
    bool stop = false;
    for (std::list<Patch>::iterator i = patches.begin(); i != patches.end(); /*nothing*/)
    {
      if ( overlap(patch,(*i)) ) {

        // if the new patch is better, remove the old one
        if(patch.value > (*i).value) { 
          i = patches.erase(i);
        } else {
          stop = true;
          ++i;
        }
      } else {
        ++i;
      }
    }

    // there is an overlaping patch in the list which is better than the new
    if(stop) {
      return;
    }

    if(patches.empty()) {
      patches.push_back(patch);
      return;
    }

    // insertion sort - inset the new patch accoring to it's value
    for(std::list<Patch>::iterator i = patches.begin(); i != patches.end(); ++i)
    {
      // insert
      if(patch.value < (*i).value) {
        i = patches.insert(i,patch);
        break;
      } else if(nextIter(i) == patches.end()) {
        patches.push_back(patch);
        break;
      }
    }

    /*
    // TODO: do we need a limited size feature?
    if(patches.size() > 30) {
      patches.pop_front();
    }
    */
  }

  void addMean(int minX, int minY, int maxX, int maxY, double value)
  {
    addMean(Patch(minX, minY, maxX, maxY, value));
  }

  void addMean(const Patch& patch)
  {
    // add the first patch
    if(patches.empty()) {
      patches.push_back(patch);
      return;
    }

    // check if there are overlaping patches
    bool newPatch = true;
    for (std::list<Patch>::iterator i = patches.begin(); i != patches.end(); /*nothing*/)
    {
      if ( overlap(patch,(*i)) ) {
          (*i).max = ((*i).max + patch.max)/2;
          (*i).min = ((*i).min + patch.min)/2;
          newPatch = false;
          ++i;
      } else {
        ++i;
      }
    }

    // there is an overlaping patch in the list which is better than the new
    if(newPatch) {
      patches.push_back(patch);
    }

    /*
    // TODO: do we need a limited size feature?
    if(patches.size() > 30) {
      patches.pop_front();
    }
    */
  }

  void addPlain(int minX, int minY, int maxX, int maxY, double value)
  {
    patches.emplace_back(minX, minY, maxX, maxY, value);
  }

  void clear() {
    patches.clear();
  }

  size_t size() const {
    return patches.size();
  }

  std::vector<Patch> asVector() const {
    std::vector<Patch> result;
    result.reserve(patches.size());
    // Add in reverse order, so the entries with the highest value come first
    for(BestPatchList::reverse_iterator i = patches.crbegin(); i != patches.crend(); ++i) {
      result.push_back(*i);
    }
    return result;
  }

private:
  PatchList patches;

private:

  template <typename Iter>
  Iter nextIter(Iter iter) {
      return ++iter;
  }

  /*
  //OLD
  bool overlap(const Patch& one, const Patch& two) {
    return std::max(std::abs(two.center.x - one.center.x), std::abs(two.center.y - one.center.y)) < (two.radius + one.radius);
  }
  */

  bool overlap(const Patch& one, const Patch& two) {
    return 
      one.min.x < two.max.x && 
      one.max.x > two.min.x &&
      one.min.y < two.max.y && 
      one.max.y > two.min.y;
  }

  bool overlapTest(const Patch& one, const Patch& two) {
    int w1 = (one.max.x - one.min.x)/4;
    int w2 = (two.max.x - two.min.x)/4;
    
    return 
      one.min.x+w1 < two.max.x-w2 && 
      one.max.x-w1 > two.min.x+w2 &&
      one.min.y+w1 < two.max.y-w2 && 
      one.max.y-w1 > two.min.y+w2;
  }

};

#endif // _BestPatchList_H_
