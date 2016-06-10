/**
* @file BestPatchList.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _BestPatchList_H_
#define _BestPatchList_H_

#include <Tools/Math/Vector2.h>
#include <list>

class BestPatchList
{

public:
  class Patch 
  {
  public:
    Patch() : radius(-1), value(-1) {}
    Patch(const Vector2i& center, double radius, double value) 
      : center(center), radius(radius), value(value)
    {}
    Vector2i center;
    double radius;
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

  void add(const Vector2i& center, double radius, double value) {
    add(Patch(center, radius, value));
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

  void clear() {
    patches.clear();
  }

  size_t size() const {
    return patches.size();
  }

private:
  PatchList patches;

private:

  template <typename Iter>
  Iter nextIter(Iter iter) {
      return ++iter;
  }

  bool overlap(const Patch& one, const Patch& two) {
    return std::max(std::abs(two.center.x - one.center.x), std::abs(two.center.y - one.center.y)) < (two.radius + one.radius);
  }

};

#endif // _BestPatchList_H_
