#ifndef ALLBALLPERCEPTS_H
#define ALLBALLPERCEPTS_H

#include <Representations/Perception/BallPercept.h>

#include <vector>

class AllBallPercepts
{
private:
    std::vector<BallPercept> allBallPercepts;

public:
    AllBallPercepts(){}

    void add(BallPercept& percept){
        allBallPercepts.push_back(percept);
    }

    void reset(){
        allBallPercepts.clear();
    }

    std::vector<BallPercept>::const_iterator begin() const {
        return allBallPercepts.begin();
    }

    std::vector<BallPercept>::const_iterator end() const {
        return allBallPercepts.end();
    }

    virtual void print(std::ostream& stream) const;

    //typedef std::vector<BallPercept>::const_iterator ABPIterator;
};

class AllBallPerceptsTop : public AllBallPercepts
{
public:
  virtual ~AllBallPerceptsTop() {}
};

/*namespace naoth
{
  template<>
  class Serializer<AllBallPercepts>
  {
  public:
    static void serialize(const AllBallPercepts& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, AllBallPercepts& representation);
  };

  template<>
  class Serializer<AllBallPerceptsTop> : public Serializer<AllBallPercepts>
  {};
}*/

#endif // ALLBALLPERCEPTS_H
