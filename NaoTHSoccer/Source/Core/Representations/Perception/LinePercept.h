/** 
 * @file BallPercept.h
 * @author <a href="mailto:mohr@informatik.hu-berlin.de">Christian Mohr</a>
 * Declaration of class LinePercept
 */

#ifndef __LinePercept_h_
#define __LinePercept_h_

#include <vector>

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Line.h"
#include "Tools/Math/Geometry.h"

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"



class LinePercept : public Printable, public Streamable
{ 
public:
  class FieldLineSegment : public Math::LineSegment
  {
    public:
      enum LineType
      {
        unknown, //equals ColorClasses::none
        O, //line along field bounds, equals ColorClasses::orange
        I, //line inside of field, equals ColorClasses::yellow
        C, //circle line, equals ColorClasses::skyblue
        none //invalid line, equals ColorClasses::white
      };

      enum LineID
      {
        center_line,
        goal_line_own,
        goal_line_opponent,
        unknown_id
      };

      FieldLineSegment()
        :
        thickness(0.0),
        slope(0.0),
        angle(0.0),
        valid(false),
        partOfCircle(false),
        seen_id(unknown_id)
      {}

      void set(const Vector2<int>& begin, const Vector2<int>& end)
      {
        base = begin;
        direction = end - begin;
        length = direction.abs();
        direction.normalize();
      }

      void set(const CameraMatrix& cameraMatrix, const CameraInfo& cameraInfo, const Vector2<int>& begin, const Vector2<int>& end)
      {
        base = begin;
        direction = end - begin;
        length = direction.abs();
        direction.normalize();

        //TODO: handle the case if the projection is not possible
        Vector2<double> beginLineOnField;
        Geometry::imagePixelToFieldCoord(cameraMatrix, cameraInfo, begin.x, begin.y, 0.0, beginLineOnField);

        //TODO: handle the case if the projection is not possible
        Vector2<double> endLineOnField;
        Geometry::imagePixelToFieldCoord(cameraMatrix, cameraInfo, end.x, end.y, 0.0, endLineOnField);
        lineOnField = Math::LineSegment(beginLineOnField, endLineOnField);
      }

      void setBegin(const Vector2<int>& begin)
      {
        base = begin;
      }

      void setEnd(const Vector2<int>& end)
      {
        direction = end - base;
        length = direction.abs();
        direction.normalize();
      }

      void setEnd(const CameraMatrix& cameraMatrix, const CameraInfo& cameraInfo, const Vector2<int>& end)
      {
        direction = end - base;
        length = direction.abs();
        direction.normalize();

        //TODO: handle the case if the projection is not possible
        Vector2<double> beginLineOnField;
        Geometry::imagePixelToFieldCoord(cameraMatrix, cameraInfo, base.x, base.y, 0.0, beginLineOnField);

        //TODO: handle the case if the projection is not possible
        Vector2<double> endLineOnField;
        Geometry::imagePixelToFieldCoord(cameraMatrix, cameraInfo, end.x, end.y, 0.0, endLineOnField);
        lineOnField = Math::LineSegment(beginLineOnField, endLineOnField);
      }

      void fillProtobuf(naothmessages::FieldLineSegment* segment) const
      {
        segment->mutable_lineinimage()->mutable_base()->set_x(base.x);
        segment->mutable_lineinimage()->mutable_base()->set_y(base.y);
        segment->mutable_lineinimage()->mutable_direction()->set_x(direction.x);
        segment->mutable_lineinimage()->mutable_direction()->set_y(direction.y);
        segment->mutable_lineinimage()->set_length(length);

        segment->mutable_lineonfield()->mutable_base()->set_x(lineOnField.getBase().x);
        segment->mutable_lineonfield()->mutable_base()->set_y(lineOnField.getBase().y);
        segment->mutable_lineonfield()->mutable_direction()->set_x(lineOnField.getDirection().x);
        segment->mutable_lineonfield()->mutable_direction()->set_y(lineOnField.getDirection().y);
        segment->mutable_lineonfield()->set_length(lineOnField.getLength());

        segment->set_angle(angle);
        segment->set_beginextendcount(beginExtendCount);
        segment->set_endextendcount(endExtendCount);
        segment->set_slope(slope);
        segment->set_thickness(thickness);
        segment->set_valid(valid);
      }//end fillProtobuf

      void readFromProtobuf(const naothmessages::FieldLineSegment* segment)
      {
        if(segment->has_lineinimage())
        {
          base.x = segment->lineinimage().base().x();
          base.y = segment->lineinimage().base().y();
          direction.x = segment->lineinimage().direction().x();
          direction.y = segment->lineinimage().direction().y();
        }

        if(segment->has_lineonfield())
        {
          Vector2<double> onFieldBase;
          onFieldBase.x = segment->lineonfield().base().x();
          onFieldBase.y = segment->lineonfield().base().y();

          Vector2<double> onFieldDirection;
          onFieldDirection.x = segment->lineonfield().direction().x();
          onFieldDirection.y = segment->lineonfield().direction().y();

          double length = segment->lineonfield().length();
          lineOnField = LineSegment(onFieldBase,onFieldBase+onFieldDirection*length);
        }

        if(segment->has_angle())
          angle = segment->angle();

        if(segment->has_beginextendcount() && segment->has_endextendcount())
        {
          beginExtendCount = segment->beginextendcount();
          endExtendCount = segment->endextendcount();
        }

        if(segment->has_slope())
          slope = segment->slope();

        if(segment->has_thickness())
          thickness = segment->thickness();

        if(segment->has_valid())
          valid = segment->valid();
      }//end readFromProtobuf

      // information in image
      int beginExtendCount;
      int endExtendCount;
      double thickness;
      double slope;
      double angle;

      bool valid; // TODO: when exactly is a line segment valid?

      /** mark if this segment apears to be a part of the middle circle */
      bool partOfCircle;
      // TODO: is the type filled properly?
      // if yes, we don't need "partOfCircle"
      LineType type;

      // estimated id of the line, e.g., center line, goal line etc.
      LineID seen_id;

      // the projection of the line segment on the ground
      Math::LineSegment lineOnField;
  };//end class FieldLineSegment


  // TODO: 
  // currently used only in 3DSim to represent the seen corners
  class Flag
  {
  public:
    Flag(const Vector2<double>& seenPosOnField, const Vector2<double>& absolutePosOnField)
      : 
      seenPosOnField(seenPosOnField),
      absolutePosOnField(absolutePosOnField)
    {}
    Vector2<double> seenPosOnField;
    Vector2<double> absolutePosOnField; // model of the flag (i.e. its known absolute osition on the field)
  };//end class Flag



  class Intersection
  {
  public:

    Intersection()
    {
      type = Math::Intersection::unknown;
    }

    Intersection(const CameraMatrix& cameraMatrix, const CameraInfo& cameraInfo, const Vector2<double>& pos)
      :
      pos(pos)
    {
      type = Math::Intersection::unknown;
      Geometry::imagePixelToFieldCoord(cameraMatrix, cameraInfo, pos.x, pos.y, 0.0, posOnField);
    }

    void setSegments(int segOne, int segTwo, double distOne, double distTwo)
    {
      segmentIndices[0] = segOne;
      segmentsDistanceToIntersection[0] = distOne;
      segmentIndices[1] = segTwo;
      segmentsDistanceToIntersection[1] = distTwo;
    };

    Vector2<unsigned int>& getSegmentIndices()
    {
      return segmentIndices;
    };

    Vector2<double>& getSegmentsDistancesToIntersection()
    {
      return segmentsDistanceToIntersection;
    }

    void setPosOnField(const Vector2<double>& p)
    {
      posOnField = p;
    }

    void setPosInImage(const Vector2<unsigned int>& p)
    {
      pos = p;
    }

    void setType(Math::Intersection::IntersectionType typeToSet)
    {
      type = typeToSet;
    }

    Math::Intersection::IntersectionType getType() const
    {
      return type;
    }

    Vector2<double> getPos()
    {
      return pos;
    }

    const Vector2<double>& getPosOnField() const
    {
      return posOnField;
    }

    void fillProtobuf(naothmessages::Intersection* intersection) const
    {
      intersection->set_type((naothmessages::Intersection_IntersectionType) type);
      intersection->mutable_posinimage()->set_x(pos.x);
      intersection->mutable_posinimage()->set_y(pos.y);
      intersection->mutable_posonfield()->set_x(posOnField.x);
      intersection->mutable_posonfield()->set_y(posOnField.y);

      intersection->set_segmentoneindex(segmentIndices[0]);
      intersection->set_segmenttwoindex(segmentIndices[1]);
      intersection->set_segmentonedistance(segmentsDistanceToIntersection[0]);
      intersection->set_segmenttwodistance(segmentsDistanceToIntersection[1]);
    }

    void readFromProtobuf(const naothmessages::Intersection* intersection)
    {
      if(intersection->has_type())
        type = (Math::Intersection::IntersectionType) intersection->type();

      if(intersection->has_posinimage())
      {
        pos.x = intersection->posinimage().x();
        pos.y = intersection->posinimage().y();
      }

      if(intersection->has_posonfield())
      {
        posOnField.x = intersection->posonfield().x();
        posOnField.y = intersection->posonfield().y();
      }

      if(intersection->has_segmentoneindex() && intersection->has_segmenttwoindex())
      {
        segmentIndices[0] = intersection->segmentoneindex();
        segmentIndices[1] = intersection->segmenttwoindex();
      }

      if(intersection->has_segmentonedistance() && intersection->has_segmenttwodistance())
      {
        segmentsDistanceToIntersection[0] = intersection->segmentonedistance();
        segmentsDistanceToIntersection[1] = intersection->segmenttwodistance();
      }
    }

  private:
    Math::Intersection::IntersectionType type;
    Vector2<unsigned int> segmentIndices;
    Vector2<double> segmentsDistanceToIntersection;
    Vector2<double> pos;
    Vector2<double> posOnField;

  };

  const static int INITIAL_NUMBER_OF_LINES = 11;

  // seen lines
  std::vector<FieldLineSegment> lines;
  // seen corners
  std::vector<Intersection> intersections;
  // seen flags (only S3D)
  std::vector<Flag> flags;

  // middle circle was seen
  bool middleCircleWasSeen;
  Vector2<double> middleCircleCenter;
  bool middleCircleOrientationWasSeen;
  Vector2<double> middleCircleOrientation;

  LinePercept()
  {
    reset();
  }

  ~LinePercept() {}
  
  /* reset percept */
  void reset()
  {
    lines.clear();
    lines.reserve(INITIAL_NUMBER_OF_LINES);

    intersections.clear();
    intersections.reserve(INITIAL_NUMBER_OF_LINES);

    flags.clear();

    middleCircleWasSeen = false;
    middleCircleCenter.x = 0.0;
    middleCircleCenter.y = 0.0;
    
  }//end reset

  virtual void print(ostream& stream) const
  {
    for(unsigned int i = 0; i < lines.size(); i++)
    {
      const FieldLineSegment& line = lines[i];
      stream << "== Line " << i << " ==\n"
        << "valid = " << (line.valid ? "true" : "false") << '\n'
        << "begin = (" << line.begin() << ") end = (" << line.end() << ")\n"
        << "lineOnField = (" << line.lineOnField.begin() << ") -- (" << line.lineOnField.end() << ")\n"
        << "thickness = (" << line.thickness << ")\n"
        << "slope = (" << line.slope << ")\n"
        << "angle = (" << line.angle << ")\n"
      ;
    }
//    for(unsigned int i = 0; i < intersections.size(); i++)
//    {
//      const Intersection& intersection = intersections[i];
//      stream << "== Intersection " << i << " ==\n"
//        << "position = (" << intersection.getPos() << ") on field = (" << intersection.getPosOnField() << ")\n"
//        << "indices of line segments = (" << intersection.getSegmentIndices()[0] << " , " << intersection.getSegmentIndices()[1] << ")\n"
//        << "distances inside line to intersection = (" << intersection.getSegmentsDistancesToIntersection()[0] << " , " << intersection.getSegmentsDistancesToIntersection()[1] << ")\n"
////        << "slope = (" << intersection. << ")\n"
////        << "angle = (" << intersection.angle << ")\n"
//      ;
//    }

    cout<<"\n";
    for(unsigned int i=0; i<flags.size(); i++)
    {
      stream <<"== Flag "<< i<< "==\n"
        <<"seenPosOnField = "<<flags[i].seenPosOnField<<"\n"
        <<"absolutePosOnField = "<<flags[i].absolutePosOnField<<"\n"
        ;
    }
  }

  /*
  virtual void toDataStream(ostream& stream) const
  {
    naothmessages::LinePercept p;

    for(size_t i=0; i < lines.size(); i++)
    {
      naothmessages::FieldLineSegment* newLineSegment = p.add_lines();
      lines[i].fillProtobuf(newLineSegment);
    }

    for(size_t i=0; i < intersections.size(); i++)
    {
      naothmessages::Intersection* newIntersection = p.add_intersections();
      intersections[i].fillProtobuf(newIntersection);
    }

    if(middleCircleWasSeen)
    {
      p.mutable_middlecirclecenter()->set_x(middleCircleCenter.x);
      p.mutable_middlecirclecenter()->set_y(middleCircleCenter.y);
    }
    else
    {
      p.clear_middlecirclecenter();
    }

    google::protobuf::io::OstreamOutputStreamLite buf(&stream);
    p.SerializeToZeroCopyStream(&buf);
  }

  virtual void fromDataStream(istream& stream)
  {
    // reset the percept befor copy from stream
    this->reset();

    naothmessages::LinePercept p;
    google::protobuf::io::IstreamInputStreamLite buf(&stream);
    p.ParsePartialFromZeroCopyStream(&buf);

    for(int i=0; i < p.lines_size(); i++)
    {
      FieldLineSegment line;
      line.readFromProtobuf(&p.lines(i));
      lines.push_back(line);
    }

    for(int i=0; i < p.intersections_size(); i++)
    {
      Intersection intersection;
      intersection.readFromProtobuf(&p.intersections(i));
      intersections.push_back(intersection);
    }

    middleCircleWasSeen = false;
    if(p.has_middlecirclecenter())
    {
      middleCircleWasSeen = true;
      middleCircleCenter.x = p.middlecirclecenter().x();
      middleCircleCenter.y = p.middlecirclecenter().y();
    }

  }
*/
};


#endif //__LinePercept_h_


