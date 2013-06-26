#ifndef _Assoziation_h_
#define _Assoziation_h_

#include <vector>

/**
* This class models weighted associations between two groups of objects A and B.
* Thereby, each element a or be can have only one association.
* If a new association for the pair (a,b) is added, all the old associations for a and b are removed.
*/
class Assoziation
{
public:
  Assoziation(int size_a, int size_b)
    : a4b(size_b, -1),
      b4a(size_a, -1),
      w4a(size_a, 0)
  {}

  void addAssociation(int a, int b, double w)
  {
    // remove old associations
    removeAssociationForA(a);
    removeAssociationForB(b);

    // add new one
    a4b.at(b) = a;
    b4a.at(a) = b;
    w4a.at(a) = w;
  }

  void removeAssociationForB(int b) {
    int a = a4b.at(b);
    if(a != -1) {
      removeAssociationForA(a);
    }
  }

  void removeAssociationForA(int a)
  {
    int b = b4a.at(a);
    if(b != -1) {
      a4b.at(b) = -1;
    }
    b4a.at(a) = -1;
    w4a.at(a) = 0;
  }

  double getW4A(int a) { return w4a.at(a); }
  double getW4B(int b) { int a = a4b.at(b); return (a==-1)?0.0:getW4A(a); }
  int getB4A(int a) { return b4a.at(a); }
  int getA4B(int b) { return a4b.at(b); }
    
private:
  std::vector<int> a4b;    // mapping b -> a
  std::vector<int> b4a;    // mapping a -> b
  std::vector<double> w4a; // mapping a -> w
};//end class Assoziation


#endif // _Assoziation_h_
