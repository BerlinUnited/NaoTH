#ifndef _MultiBallPercept_H
#define _MultiBallPercept_H

#include <vector>

#include <Tools/Math/Vector2.h>

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/CameraInfo.h>

class MultiBallPercept : public naoth::Printable
{
public:
    class BallPercept{

    public:
        Vector2d centerInImage;
        double   radiusInImage;
        Vector2d positionOnField;

        naoth::CameraInfo::CameraID cameraId;

        virtual void print(std::ostream& stream) const
        {
            stream << "centerInImage = " << centerInImage << std::endl;
            stream << "radiusInImage = " << radiusInImage << std::endl;
            stream << "positionOnField = " << positionOnField << std::endl;
            stream << "cameraId = " << cameraId << std::endl;
        }

        virtual ~BallPercept() {}
    };

private:
    std::vector<MultiBallPercept::BallPercept> percepts;

public:
    MultiBallPercept(){}

    const std::vector<MultiBallPercept::BallPercept>& getPercepts() const {
      return percepts;
    }

    void add(MultiBallPercept::BallPercept& percept){
        percepts.push_back(percept);
    }

    void reset(){
        percepts.clear();
    }

    bool wasSeen() const {
      return !percepts.empty();
    }

    std::vector<MultiBallPercept::BallPercept>::const_iterator begin() const {
        return percepts.begin();
    }

    std::vector<MultiBallPercept::BallPercept>::const_iterator end() const {
        return percepts.end();
    }

    naoth::FrameInfo frameInfoWhenBallWasSeen;

    virtual void print(std::ostream& stream) const;

    typedef std::vector<MultiBallPercept::BallPercept>::const_iterator ConstABPIterator;

};

namespace naoth
{
  template<>
  class Serializer<MultiBallPercept>
  {
  public:
    static void serialize(const MultiBallPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, MultiBallPercept& representation);
  };

}

#endif // _MultiBallPercept_H
