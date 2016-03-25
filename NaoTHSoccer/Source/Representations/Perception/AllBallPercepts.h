#ifndef ALLBALLPERCEPTS_H
#define ALLBALLPERCEPTS_H

#include <vector>

#include <Tools/Math/Vector2.h>

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>

class AllBallPercepts : public naoth::Printable
{
public:
    class BallPercept{

    public:
        Vector2d centerInImage;
        double   radiusInImage;
        Vector2d bearingBasedOffsetOnField;

        enum Image {bottom, top} image;

        virtual void print(std::ostream& stream) const
        {
            stream << "centerInImage = " << centerInImage << std::endl;
            stream << "radiusInImage = " << radiusInImage << std::endl;
            stream << "bearingBasedOffsetOnField = " << bearingBasedOffsetOnField << std::endl;
        }//end print
    };

private:
    std::vector<AllBallPercepts::BallPercept> allBallPercepts;

public:
    AllBallPercepts(){}

    void add(AllBallPercepts::BallPercept& percept){
        allBallPercepts.push_back(percept);
    }

    void reset(){
        allBallPercepts.clear();
    }

    bool ballWasSeen() const {
        return allBallPercepts.size() != 0;
    }

    std::vector<AllBallPercepts::BallPercept>::const_iterator begin() const {
        return allBallPercepts.begin();
    }

    std::vector<AllBallPercepts::BallPercept>::const_iterator end() const {
        return allBallPercepts.end();
    }

    naoth::FrameInfo frameInfoWhenBallWasSeen;

    virtual void print(std::ostream& stream) const;

    typedef std::vector<AllBallPercepts::BallPercept>::const_iterator ConstABPIterator;

};

namespace naoth
{
  template<>
  class Serializer<AllBallPercepts>
  {
  public:
    static void serialize(const AllBallPercepts& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, AllBallPercepts& representation);
  };

}

#endif // ALLBALLPERCEPTS_H
